
//#include "volume_operators.h"
#include "grid.h"
//#include "Image.h"
#include "OIIOFiles.h"
#include "boundingbox.h"
#include "Camera.h"
#include "light.h"
#include "renderlog.h"
//#include "colorSlider.h"
#include <vector>
#include <stdio.h>
//#include <boost/shared_ptr.hpp>
#include <boost/timer.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <memory>

const double PI  =3.141592653589793238463;
//------------------------------------------------------------------------------
//GLOBAL SCENE SETTINGS
//------------------------------------------------------------------------------
//Light scatter / attentuation coefficient
const double K = 1.0;
const double emissive = 1.0;

//Discrete step for our marching.  Delta S in our equations
const double marchStep = 0.07;

const int frameStart = 0;
const int frameEnd = 168;

//BBsize is half the length of an edge of the size of our cube. So bSize = 3 means our bounding box is a cube with edges length 6
const float bbSize = 6.0;
const float gridSize = 4.0;
const float gridVoxelCount = 300;
//Width and Height may change based on input
int w = 480;
int h = 270;
int octaves = 5;
float roughness = 0.7;
float frequency = 0.3;
float fjump = 2.2;
float noiseMin = -2.5;
float noiseMax = 1.7;
Camera mainCam;

int color_octaves = 1;
float color_roughness = 0.7;
float color_frequency = 0.15;
float color_fjump = 2.2;
float color_noiseMin = -0.4; 
float color_noiseMax = 0.7;
//------------------------------------------------------------------------------

std::vector<lux::light> lights;
std::vector<std::shared_ptr<lux::Volume<float> > > volumes;
std::vector<std::shared_ptr<lux::Volume<lux::Color> > > colorVolumes;

std::vector<float> black = {0.0, 0.0, 0.0, 0.0};

lux::Color linearInterpolate(float start, float end, float pos, lux::Color v1, lux::Color v2){
    float len = end - start;
    pos = (pos - start) / len;
    return v1*(1-pos) + v2*pos;
}

lux::Color getColor(float density){
    lux::Color slowColor(0.1, 0.05, 1.0, 1.0);    //at weight 0.0
    lux::Color medColor(0.7, 0.1, 0.4, 1.0);     //at weight 0.4
    lux::Color medFastColor(0.9, 0.3, 0.2, 1.0);     //at weight 0.5
    lux::Color fastColor(0.95, 1.0, 0.1, 1.0);    //at weight 0.85
    lux::Color fastestColor(1.0, 1.0, 1.0, 1.0); //at weight 1.0

    lux::Color newColor;
    if(density < 0.6)
        newColor = linearInterpolate(0.0, 0.6, density, slowColor, medColor);
    else if (density >= 0.6 && density < 0.8)
        newColor = linearInterpolate(0.6, 0.8, density, medColor, medFastColor);
    else if (density >= 0.8 && density < 0.85)
        newColor = linearInterpolate(0.8, 0.85, density, medFastColor, fastColor);
    else if (density >= 0.85)
        newColor = linearInterpolate(0.85, 1.00, density, fastColor, fastestColor);
    //newColor[3] = 1;// - (1-density)*0.4;*/
    return newColor;
}

//Returns the light transmissivity from a light to a point in the volume
double rayMarchLightScatter(lux::Vector x, lux::light l, lux::Volume<float> *vol){

    double marchLen = 0.0;
    lux::Vector toLight(l.pos - x);
    double dist = toLight.magnitude();
    toLight.normalize();
    double T = 1.0;
    lux::Vector x1(x);

    while (marchLen < dist){

        double density = vol->eval(x1);
        if(density > 0.0){

            T *= exp(-density * marchStep * K);
            x1 += marchStep * toLight;
        }
        marchLen += marchStep;

    }
    return T;
}

lux::Color rayMarch(Camera cam, lux::Vector n, float start, float end){

    //extinction coefficient
    double totalMarchLength = end - start;
    //std::cout << "Start: " << start << "\nEnd: " << end << "\n";
    double marchLen = 0.0;
    double T = 1.0;
    //std::cout << '(' << ray[0] << ", "  << ray[1] << ", " << ray[2] << ")\n";
    lux::Color C {0.0, 0.0, 0.0, 1.0};
    lux::Color fieldColor;
    //if (ray[0] < 0.5){
    //    C[0] = 1.0;
    //}
    //std::vector<float> smokeColor {0.2, 0.3, 0.92, 1.0};
    lux::Vector x = cam.eye() + n*start;
    lux::Color color(0.2, 0.3, 0.92, 1.0);
    while (marchLen < totalMarchLength){
        //Check each volume
        for(int j = 0; j < volumes.size(); j++){
            double density = volumes[j].get()->eval(x);
            if (density > 0.0){
                //std::cout << density << "\n";

                //step 1
                double deltaT = exp(-density * marchStep * K);
                double tVal = T * (1 - deltaT);

                //March to each light
                for (int i = 0; i < lights.size(); i++){
                    double lightTransmissivity = rayMarchLightScatter(x, lights[i], volumes[j].get());
                    C += lights[i].c * lightTransmissivity * tVal;
                }
                T *= deltaT;

                if (emissive) {
                    //Boosting the emission of the peaks in our noise function- this makes the star clusters appear brighter 
                    float em = emissive;
                    if (density > 0.85){
                        float diff = density - 0.85;
                        em += diff * 13;
                    }
                    color = getColor(density);
                    //The field color's contribution will be scaled by the density, so sparse areas don't become super bright
                    fieldColor = colorVolumes[0].get()->eval(x);
                    color += lux::colorClamp(fieldColor);
                    C += color * tVal * em;
                }
            }
        }

        //step 4
        x += n * marchStep;
        marchLen += marchStep;
    }

    return C;
}


void initLights(){
    lights.push_back(lux::light(lux::Color(0.6, 0.6, 1.0, 1.0), lux::Vector(0.0, -1.0, 0.0), lux::Vector(-4.0, 5.0, 1.0), 1.0));
    lights.push_back(lux::light(lux::Color(1.0, 0.6, 0.6, 1.0), lux::Vector(0.0, -1.0, 0.0), lux::Vector(4.0, 5.0, 0.0), 1.0));

}

void initVolumes(){

    //Implicit functions
    auto sphere = std::make_shared<lux::SphereVolume<float> >(4);
    auto sphereUp = std::make_shared<lux::TranslateVolume<float> >(sphere, lux::Vector(0.0, 1.0, 1.0));
    auto sphereDown = std::make_shared<lux::TranslateVolume<float> >(sphere, lux::Vector(0.0, -1.0, 1.0));
    auto sphereUnion = std::make_shared<lux::UnionVolume<float> >(sphereUp, sphereDown);
    auto box = std::make_shared<lux::BoxVolume<float> >(2.0, 6.0);
    auto boxMask = std::make_shared<lux::MaskVolume>(box);
    auto rotBox1 = std::make_shared<lux::Rotate_S_Volume>(boxMask, lux::Vector(0.0, 0.0, 0.0), lux::Vector(0.0, 1.0, 1.0), -45.0*PI/180.0);
    //auto rotBox2 = std::make_shared<lux::Rotate_S_Volume>(transTest, lux::Vector(0.0, 0.0, 0.0), lux::Vector(0.0, 0.0, 1.0), -54.0*PI/180.0);
    auto arm = std::make_shared<lux::Scale_S_Volume>(box, lux::Vector(0.0, 0.0, 0.0), 1.5);
    auto leftArm = std::make_shared<lux::TranslateVolume<float> >(arm, lux::Vector(-2.0, 1.0, 0.0));
    auto rightArm = std::make_shared<lux::TranslateVolume<float> >(arm, lux::Vector(2.0, 1.0, 0.0));

    SimplexNoiseObject simplex(octaves, roughness, frequency, fjump, noiseMin, noiseMax);
    SimplexNoiseObject colorNoise(color_octaves, color_roughness, color_frequency, color_fjump, color_noiseMin, color_noiseMax);

    auto noiseVolume = std::make_shared<lux::SimplexNoiseVolume>(simplex);
    
    lux::Color c(1.0, 0.0, 1.0, 1.0);
    auto colorVolume = std::make_shared<lux::SimplexNoiseColorVolume> (colorNoise, 0.0, 10.0, 20.0);
    colorVolumes.push_back(colorVolume);
    volumes.push_back(noiseVolume);
    //auto pyroSphere = std::make_shared<lux::PyroSphereVolume>(2.0, 1.0, 1.0, simplex);


    //lux::FloatGrid pyroSphereGrid = lux::FloatGrid(pyroSphere, lux::Vector(-gridSize, -gridSize, -gridSize), gridVoxelCount, 200);
    //auto griddedPyro = std::make_shared<lux::GriddedVolume>(pyroSphereGrid);
/*
    auto cylinder = std::make_shared<lux::CylinderVolume<float> >(lux::Vector(0.0, 1.0, 0.0), 0.2);
    auto cone = std::make_shared<lux::ConeVolume<float> >(lux::Vector(0.0, -1.0, 0.0), 1.0, 25.0*PI/180.0);

    //Operators
    auto scaleTest = std::make_shared<lux::Scale_S_Volume>(cone, lux::Vector(0.0, 0.0, 0.0), 2.0);
    auto transTest = std::make_shared<lux::TranslateVolume<float> >(scaleTest, lux::Vector(0.0, 1.0, 1.0));
    auto rotTest = std::make_shared<lux::Rotate_S_Volume>(transTest, lux::Vector(0.0, 0.0, 0.0), lux::Vector(0.0, 0.0, 1.0), -54.0*PI/180.0);
    auto blendTest = std::make_shared<lux::BlinnBlend_S_Volume>(transTest, rotTest, 0.5, 0.5, 1.0);
    auto unionTest = std::make_shared<lux::UnionVolume<float> >(transTest, rotTest);
    */
    //volumes.push_back(griddedPyro);
    //volumes.push_back(leftArm);
    //volumes.push_back(rightArm);
    //volumes.push_back(new lux::Scale_S_Volume(sphere, lux::Vector(0.0, 0.0, 0.0), 2.0));
    //volumes.push_back(new lux::Scale_S_Volume(sphere, lux::Vector(0.0, 0.0, 0.0), 2.0));
    //volumes.push_back(new lux::TranslateVolume<float>(box, lux::Vector(0.0, 0.0, -2.0)));
    //volumes.push_back(new lux::TranslateVolume<float>(cone, lux::Vector(0.0, 0.4, -2.0)));
}

void renderImage(Camera cam, BoundingBox bb, RenderLog renderLog){
    lux::Image img;
    img.reset(w, h, 4);
    boost::timer t;
    //Fire a ray from eye through every pixel
    int progressMod = w / 10;
    for(int i = 0; i < w; i++){
        for(int j = 0; j < h; j++){
           // std::cout << "3\n";
            double normalizedPixelCoordX = (double)i / (double)w;
            double normalizedPixelCoordY = (double)j / (double)h;
            lux::Vector r = cam.view(normalizedPixelCoordX, normalizedPixelCoordY);
            Ray ray(cam.eye(), r);
            std::vector<float> intersects = bb.intersect(ray, 0, 50);
            if(intersects[0] != -1.0){
                lux::Color C = rayMarch(cam, r, intersects[0], intersects[1]);
                std::vector<float> colorVector= {static_cast<float>(C[0]), static_cast<float>(C[1]), static_cast<float>(C[2]), static_cast<float>(C[3])};
                img.setPixel( i, j, colorVector);
            }
            else{
                img.setPixel(i, j, black);
            }
        }

        //Print our progress to make sure we aren't frozen
        if (i % progressMod == 0){
            std::cout << (i*100/w) << "%\n";
        }
    }

    float elapsedTime = t.elapsed();
    std::cout << "Time: " << elapsedTime << "\n";

    //Write our image
    std::ostringstream ss;
    //ss << setfill(' ') << setw(20) << "Eye:" << setfill('-')  << setw(30) << mainCam.eye() << "\n";
    //ss << setfill(' ') << setw(20) << "View:" << setfill('-')  << setw(30) << mainCam.view() << "\n";
    ss << setfill(' ') << setw(20) << "W:" << setfill('-')  << setw(30) << w << "\n";
    ss << setfill(' ') << setw(20) << "H:" << setfill('-')  << setw(30)<< h << "\n";
    ss << setfill(' ') << setw(20) << "MarchStep:" << setfill('-')  << setw(30)<< marchStep << "\n";
    ss << setfill(' ') << setw(20) << "Bounding Box Size:" << setfill('-')  << setw(30)<< bbSize << "\n";
    ss << setfill(' ') << setw(20) << "TotalTime:" << setfill('-')  << setw(30)<< elapsedTime << "\n\n";
    ss << setfill(' ') << setw(20) << "Octaves:" << setfill('-')  << setw(30) << octaves << "\n";
    ss << setfill(' ') << setw(20) << "Roughness:" << setfill('-')  << setw(30) << roughness << "\n";
    ss << setfill(' ') << setw(20) << "Frequency:" << setfill('-')  << setw(30) << frequency << "\n";
    ss << setfill(' ') << setw(20) << "fJump:" << setfill('-')  << setw(30) << fjump << "\n";
    ss << setfill(' ') << setw(20) << "Noise Minumum:" << setfill('-')  << setw(30) << noiseMin << "\n";
    ss << setfill(' ') << setw(20) << "Noise Maximum:" << setfill('-')  << setw(30) << noiseMax << "\n";
    renderLog.addLine(ss.str());
    renderLog.writeToFile();

    std::string filepath = renderLog.getFilepath();
    filepath += ".exr";
    lux::writeOIIOImage(filepath.c_str(), img, 1.0, 1.0);
    std::cout << renderLog.getBody();
}

int main(int argc, char **argv){

    if (argc >= 3){
        if (strcmp(argv[2], "1080") == 0){
            w = 1920;
            h = 1080;
        }
        else if (strcmp(argv[2], "720") == 0){
            w = 1280;
            h = 720;
        }
    }

    mainCam.setEyeViewUp(lux::Vector(9.0, 0.0, 9.0), lux::Vector(-1,0,-1), lux::Vector(0,1,0));
    BoundingBox bb(lux::Vector(-bbSize, -bbSize, -bbSize), lux::Vector(bbSize, bbSize, bbSize));
    initVolumes();


    for (int i = frameStart; i < frameEnd; i++){
        std::ostringstream ss;
        ss << setfill('0') << setw(4) << i;
        std::string numPadding = ss.str();
        std::string filepath("output/");
        if (argc >= 2){
            filepath += std::string(argv[1]);
        }
        else{
            filepath += std::string("lastOutput");
        }
        if (frameEnd - frameStart > 1){
            filepath += ".";
            filepath += numPadding;
        }

        RenderLog renderLog(filepath);
        mainCam.setEyeViewUp(lux::Vector(9.0 - ((float)i * 0.02), 0.0, 9.0 - ((float)i * 0.02)), lux::Vector(-1,0,-1), lux::Vector(0,1,0));
        renderImage(mainCam, bb, renderLog);
        std::cout << filepath << "\n";
    }
    //initLights();



    return 0;
}
