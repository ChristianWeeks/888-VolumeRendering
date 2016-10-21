
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
#include <random>



const double PI  =3.141592653589793238463;
//------------------------------------------------------------------------------
//GLOBAL SCENE SETTINGS
//------------------------------------------------------------------------------
//Light scatter / attentuation coefficient
const double K = 1.0;
const double emissive = 0.05;

//Discrete step for our marching.  Delta S in our equations
const double marchStep = 0.04;
const double lightMarchStep = 0.12;

const int frameStart = 0;
const int frameEnd = 1;

//BBsize is half the length of an edge of the size of our cube. So bSize = 3 means our bounding box is a cube with edges length 6
const float bbSize = 3.0;
const float gridSize = 2 * bbSize;
const float gridVoxelCount = 40;
//Width and Height may change based on input
int w = 480;
int h = 270;
int octaves = 3;
float roughness = 0.5;
float frequency = 0.4;
float fjump = 2.2;
float noiseMin = -1.8;
float noiseMax = 1.8;
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

lux::Color colorClamp(lux::Color c){
    if (c[0] < 0)
        c[0] = 0;
    if (c[1] < 0)
        c[1] = 0;
    if (c[2] < 0)
        c[2] = 0;
    if (c[0] > 1)
        c[0] = 1;
    if (c[1] > 1)
        c[1] = 1;
    if (c[2] > 1)
        c[2] = 1;
    return c;
}

lux::Color linearInterpolate(float start, float end, float pos, lux::Color v1, lux::Color v2){
    float len = end - start;
    pos = (pos - start) / len;
    return v1*(1-pos) + v2*pos;
}

//Wisp algorithm
void CreateWisp(const lux::Vector P, lux::FloatGrid f){

    float radius = 2.0;
    float numDots = 10000000;
    float clump = 1.0;
    SimplexNoiseObject noise1(octaves, roughness, 2.0, fjump, 0, 1, 0);
    SimplexNoiseObject noise2(octaves, roughness, 2.0, fjump, 0, 1, 0);
    std::random_device rnd;
    std::mt19937 rng(rnd());
    std::uniform_real_distribution<> udist(-radius, radius);

    /*float x = 2.0 * noise1.eval(P[0], P[1], P[2]) - 1.0;
    float y = 2.0 * noise1.eval(P[0] + 10, P[1] + 10, P[2] + 10) - 1.0;
    float z = 2.0 * noise1.eval(P[0] + 20, P[1] + 20, P[2] + 20) - 1.0;*/
    for(int i = 0; i < numDots; i++){
        lux::Vector d; 

        float randX = udist(rng);
        float randY = udist(rng);
        float randZ =  udist(rng);

        d[0] = 2.0 * noise1.eval(randX, randY, randZ) - 1.0;
        d[1] = 2.0 * noise1.eval(randX + 10, randY + 10, randZ + 10) - 1.0;
        d[2] = 2.0 * noise1.eval(randX + 20, randY + 20, randZ + 20) - 1.0;

        lux::Vector dSphere = d.unitvector();

        float radialDisp = std::pow(std::abs(noise1.eval(d[0], d[1], d[2])), clump);

        dSphere *= radialDisp;

        lux::Vector dot = P + dSphere * radius;

        /*lux::Vector d2;
        d2[0] = 2.0 * noise2.eval(randX, randY, randZ) - 1.0;
        d2[1] = 2.0 * noise2.eval(randX + 10, randY + 10, randZ + 10) - 1.0;
        d2[2] = 2.0 * noise2.eval(randX + 20, randY + 20, randZ + 20) - 1.0;

        dot += d2;*/
        f.bakeDensity(dot, 0.05);
    }

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
    SimplexNoiseObject colorNoise(color_octaves, color_roughness, color_frequency, color_fjump, color_noiseMin, color_noiseMax, 0);
        }
        marchLen += lightMarchStep;

    }
    return T;
}

lux::Color rayMarch(Camera cam, lux::Vector n, float start, float end){

    //extinction coefficient
    double totalMarchLength = end - start;

    double marchLen = 0.0;
    double T = 1.0;
    lux::Color C {0.0, 0.0, 0.0, 1.0};
    lux::Color fieldColor;

    lux::Vector x = cam.eye() + n*start;
    lux::Color color(0.2, 0.90, 0.5, 1.0);
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
                    C += lights[i].c * lightTransmissivity * tVal; //colorVolumes[0].get()->eval(x);
                }
                T *= deltaT;

                if (emissive) {
                    //Boosting the emission of the peaks in our noise function- this makes the star clusters appear brighter 
                    //color = getColor(density);
                    //The field color's contribution will be scaled by the density, so sparse areas don't become super bright
                    fieldColor = colorVolumes[0].get()->eval(x) * 0.2;
                    color += colorClamp(fieldColor);
                    C += color * tVal * emissive;
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
    lights.push_back(lux::light(lux::Color(0.4, 0.80, 1.0, 1.0), lux::Vector(0.0, -1.0, 0.0), lux::Vector(-2.0, 5.0, 4.0), 1.0));
    //lights.push_back(lux::light(lux::Color(1.0, 0.4, 0.4, 1.0), lux::Vector(0.0, -1.0, 0.0), lux::Vector(2.0, -2.0, -4.0), 1.0));
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

   // SimplexNoiseObject simplex(octaves, roughness, frequency, fjump, noiseMin, noiseMax);
    //SimplexNoiseObject colorNoise(color_octaves, color_roughness, color_frequency, color_fjump, color_noiseMin, color_noiseMax);

   // auto sphere = std::make_shared<lux::SphereVolume<float> >(4);


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
    SimplexNoiseObject colorNoise(color_octaves, color_roughness, color_frequency, color_fjump, color_noiseMin, color_noiseMax, 0);

    //Write our image
    std::ostringstream ss;
    //ss << setfill(' ') << setw(20) << "Eye:" << setfill('-')  << setw(30) << mainCam.eye() << "\n";
    //ss << setfill(' ') << setw(20) << "View:" << setfill('-')  << setw(30) << mainCam.view() << "\n";
    ss << setfill(' ') << setw(20) << "W:" << setfill('-')  << setw(30) << w << "\n";
    ss << setfill(' ') << setw(20) << "H:" << setfill('-')  << setw(30)<< h << "\n";
    ss << setfill(' ') << setw(20) << "March Step:" << setfill('-')  << setw(30)<< marchStep << "\n";
    ss << setfill(' ') << setw(20) << "Light March Step:" << setfill('-')  << setw(30)<< marchStep << "\n";
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

    mainCam.setEyeViewUp(lux::Vector(12.0, 0.0, 12.0), lux::Vector(-1,0,-1), lux::Vector(0,1,0));
    BoundingBox bb(lux::Vector(-bbSize, -bbSize, -bbSize), lux::Vector(bbSize, bbSize, bbSize));
    initLights();
    //initVolumes();

    SimplexNoiseObject colorNoise(color_octaves, color_roughness, color_frequency, color_fjump, color_noiseMin, color_noiseMax, 0);
    auto colorVolume = std::make_shared<lux::SimplexNoiseColorVolume> (colorNoise, 0.0, 10.0, 20.0);
    colorVolumes.push_back(colorVolume);
    
    auto sphere = std::make_shared<lux::SphereVolume<float> >(2);
    auto constVol = std::make_shared<lux::ConstantVolume<float> >(0.0);
    lux::FloatGrid wispGrid = lux::FloatGrid(constVol, lux::Vector(-bbSize, -bbSize, -bbSize), gridSize + 0.1, 200);
    boost::timer wispTime;
    CreateWisp(lux::Vector(0, 0, 0), wispGrid);
    std::cout << "Wisp Build Time: " << wispTime.elapsed() << "\n";
    auto griddedWisp = std::make_shared<lux::GriddedVolume>(wispGrid);
    volumes.push_back(griddedWisp);

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

        //SimplexNoiseObject simplex(octaves, roughness, frequency, fjump, noiseMin, noiseMax, 0.01 * i);
        //auto pyroSphere = std::make_shared<lux::PyroSphereVolume>(2.2, 3.0, 2.0, simplex);
        //volumes.push_back(pyroSphere);

        RenderLog renderLog(filepath);
        //mainCam.setEyeViewUp(lux::Vector(9.0 - ((float)i * 0.02), 0.0, 9.0 - ((float)i * 0.02)), lux::Vector(-1,0,-1), lux::Vector(0,1,0));
        renderImage(mainCam, bb, renderLog);
        std::cout << filepath << "\n";
        volumes.clear();
    }
    //initLights();



    return 0;
}
