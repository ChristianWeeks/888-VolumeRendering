
//#include "volume_operators.h"
#include "grid.h"
#include <Magick++.h>
//#include "Image.h"
#include "OIIOFiles.h"
#include "boundingbox.h"
#include "Camera.h"
//#include "light.h"
#include "renderlog.h"
#include "WedgeAttribute.h"

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

const int frameStart = 1;
const int frameEnd =2; 

//BBsize is half the length of an edge of the size of our cube. So bSize = 3 means our bounding box is a cube with edges length 6
const float bbSize = 3.0;
const float gridSize = 2 * bbSize;
const float gridVoxelCount = 10;
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

float radius = 1.6;
float numDots = 2000000;
float clump = 2.0;

std::vector<lux::light> lights;
std::vector<lux::DeepShadowMap> lightGrids;

std::vector<std::shared_ptr<lux::Volume<float> > > volumes;
std::vector<std::shared_ptr<lux::Volume<lux::Color> > > colorVolumes;
//------------------------------------------------------------------------------

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
void CreateWisp(const lux::Vector P, lux::DensityGrid f, SimplexNoiseObject noise1, SimplexNoiseObject noise2){

    std::random_device rnd;
    std::mt19937 rng(rnd());
    std::uniform_real_distribution<> udist(-radius, radius);

    /*float x = 2.0 * noise1.eval(P[0], P[1], P[2]) - 1.0;
    float y = 2.0 * noise1.eval(P[0] + 10, P[1] + 10, P[2] + 10) - 1.0;
    float z = 2.0 * noise1.eval(P[0] + 20, P[1] + 20, P[2] + 20) - 1.0;*/
    for(int i = 0; i < numDots; i++){

        float randX = udist(rng);
        float randY = udist(rng);
        float randZ =  udist(rng);

        /*d[0] = 2.0 * noise1.eval(randX, randY, randZ) - 1.0;
        d[1] = 2.0 * noise1.eval(randX + 10, randY + 10, randZ + 10) - 1.0;
        d[2] = 2.0 * noise1.eval(randX + 20, randY + 20, randZ + 20) - 1.0;*/
        lux::Vector d(randX, randY, randZ);

        float radialDisp = std::pow(std::abs(noise1.eval(d[0], d[1], d[2])), clump);
        lux::Vector dSphere = d.unitvector();

        dSphere *= radialDisp;

        lux::Vector dot = P + dSphere * radius;

        lux::Vector d2;
        d2[0] = noise2.eval(dSphere[0], dSphere[1], dSphere[2]);
        d2[1] = noise2.eval(dSphere[0] + 0.1, dSphere[1] + 0.1, dSphere[2] + 0.1);
        d2[2] = noise2.eval(dSphere[0] - 0.1, dSphere[1] - 0.1, dSphere[2] - 0.1);

        dot += d2;
        f.bakeDot(dot, 0.3);
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
        }
        x1 += marchStep * toLight;
        marchLen += lightMarchStep;

    }
    return T;
}

double rayMarchDSM(lux::Vector x, lux::DeepShadowMap dsm){
    return exp(-K * dsm.trilinearInterpolate(x));
}

lux::Color rayMarch(Camera cam, lux::Vector n, float start, float end){

    //extinction coefficient
    double totalMarchLength = end - start;

    double marchLen = 0.0;
    double T = 1.0;
    lux::Color C {0.0, 0.0, 0.0, 0.0};
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
                for (int i = 0; i < lightGrids.size(); i++){
                    //double lightTransmissivity = rayMarchLightScatter(x, lights[i], volumes[j].get());
                    double lightTransmissivity = rayMarchDSM(x, lightGrids[i]);
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

    //lights.push_back(lux::light(lux::Color(1.0, 0.4, 0.4, 1.0), lux::Vector(0.0, -1.0, 0.0), lux::Vector(2.0, -2.0, -4.0), 1.0));
}

void renderImage(Camera cam, BoundingBox bb, RenderLog renderLog){
    lux::Image img;
    img.reset(w, h, 4);
    std::vector<float> black = {0.5, 0.5, 0.5, 0.0};
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
    std::cout << "Render Time: " << elapsedTime << "\n";
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
    //std::cout << renderLog.getBody();
}

int main(int argc, char **argv){

    boost::timer totalTimer;
    if (argc >= 3){
        if (strcmp(argv[2], "1080") == 0){
            w = 1920;
            h = 1080;
        }
        else if (strcmp(argv[2], "720") == 0){
            w = 1280;
            h = 720;
        }
        else if (strcmp(argv[2], "540") == 0){
            w = 960;
            h = 540;
        }
    }

    //Init Camera, Bounding box
    mainCam.setEyeViewUp(lux::Vector(7.0, 0.0, 7.0), lux::Vector(-1,0,-1), lux::Vector(0,1,0));
    BoundingBox bb(lux::Vector(-bbSize, -bbSize, -bbSize), lux::Vector(bbSize, bbSize, bbSize));

    //lux::light l1(lux::light(lux::Color(0.4, 0.80, 1.0, 1.0), lux::Vector(0.0, -1.0, 0.0), lux::Vector(-2.0, 5.0, 4.0), 1.0));
    lux::light l1(lux::light(lux::Color(.4, 0.80, 1.0, 1.0), lux::Vector(0.0, -1.0, 0.0), lux::Vector(-2.0, 5.0, -1.0), 1.0));
    lights.push_back(l1);

    SimplexNoiseObject colorNoise(color_octaves, color_roughness, color_frequency, color_fjump, color_noiseMin, color_noiseMax, 0);
    auto colorVolume = std::make_shared<lux::SimplexNoiseColorVolume> (colorNoise, 0.0, 10.0, 20.0);
    colorVolumes.push_back(colorVolume);

    //Set up our wedge values
    /*int octave1 = 1;
    float rough1 = 0.5;
    float freq1 = 0.3;
    float fjump1 = 2.2;

    int octave2 = 3;
    float rough2 = 0.5;
    float freq2 = 2.0;
    float fjump2 = 2.2;

    float noiseMin = 0.7;
    float noiseMax = 1.3;

    float noiseMin2 = -0.2;
    float noiseMax2 = 0.2;

    WedgeAttribute noiseMinWedge; 
    WedgeAttribute noiseMaxWedge; 
    noiseMinWedge.addKeyFrame(1, -0.01);
    noiseMaxWedge.addKeyFrame(1, 0.01);

    noiseMinWedge.addKeyFrame(20, -1.0);
    noiseMaxWedge.addKeyFrame(20, 1.0);
    noiseMinWedge.addKeyFrame(31, 0.99);
    noiseMaxWedge.addKeyFrame(31, 1.01);
    noiseMinWedge.addKeyFrame(50, 0.00);
    noiseMaxWedge.addKeyFrame(50, 2.00);*/

    for (int i = frameStart; i < frameEnd; i++){

        //Set up our filepath
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

        //WEDGE----------------------------------------------
        //freq1 = freq1Wedge.getValueAtFrame(i);
        //freq2 = freq2Wedge.getValueAtFrame(i);
        //octave1 = octave1Wedge.getValueAtFrame(i);
        //clump = clumpWedge.getValueAtFrame(i);
        //noiseMin2 = noiseMinWedge.getValueAtFrame(i);
        //noiseMax2 = noiseMaxWedge.getValueAtFrame(i);
        //WEDGE----------------------------------------------

        //-------------------------------------------------------------------------------------------------------------------------------------
        // SET UP OUR NOISE OBJECT, VOLUMES, GRIDS, AND SHADOW MAPS HERE
        //-------------------------------------------------------------------------------------------------------------------------------------
        //SimplexNoiseObject wispNoise1(octave1, rough1, freq1, fjump1, noiseMin, noiseMax, 0);
        //SimplexNoiseObject wispNoise2(octave2, rough2, freq2, fjump2, noiseMin2, noiseMax2, 0);

        //Set up our volume
        auto sphereVol = std::make_shared<lux::SphereVolume<float> >(2.0);
        auto constVol = std::make_shared<lux::ConstantVolume<float> > (4.0);
        auto multVol = std::make_shared<lux::MultVolume<float> >(sphereVol, constVol);

        lux::DeepShadowMap dsm1(l1, lightMarchStep, multVol, lux::Vector(-bbSize, -bbSize, -bbSize), gridSize, gridVoxelCount);
        boost::timer dsmTimer;
        lightGrids.push_back(dsm1);
        std::cout << "DSM Construction Time: " << dsmTimer.elapsed() << "\n";
        //auto constVol = std::make_shared<lux::ConstantVolume<float> >(0.0);

        //lux::DensityGrid wispGrid = lux::DensityGrid(constVol, lux::Vector(-bbSize, -bbSize, -bbSize), gridSize + 0.2, gridVoxelCount);
        //CreateWisp(lux::Vector(0, 0, 0), wispGrid, wispNoise1, wispNoise2);
        //auto griddedWisp = std::make_shared<lux::GriddedVolume>(wispGrid);
        volumes.push_back(multVol);

        //mainCam.setEyeViewUp(lux::Vector(9.0 - ((float)i * 0.02), 0.0, 9.0 - ((float)i * 0.02)), lux::Vector(-1,0,-1), lux::Vector(0,1,0));
        //-------------------------------------------------------------------------------------------------------------------------------------
        renderImage(mainCam, bb, renderLog);

        //Empty our volume vector for next iteration
        volumes.clear();

        //Annotate our image for the wedge
        std::ostringstream ssAnno;
        /*ssAnno << setfill(' ') << setw(10) << "Radius:" << setfill('.')  << setw(12) << radius << "\n";
        ssAnno << setfill(' ') << setw(10) << "Dots:" << setfill('.')  << setw(12)<< numDots << "\n";
        ssAnno << setfill(' ') << setw(10) << "Clump:" << setfill('.')  << setw(12)<< clump << "\n\n";
        ssAnno << "Noise1\n";
        ssAnno << setfill(' ') << setw(10) << "Octaves:" << setfill('.')  << setw(12)<< octave1 << "\n";
        ssAnno << setfill(' ') << setw(10) << "Rough:" << setfill('.')  << setw(12)<< rough1 << "\n";
        ssAnno << setfill(' ') << setw(10) << "Freq:" << setfill('.')  << setw(12)<< freq1 << "\n";
        ssAnno << setfill(' ') << setw(10) << "noiseMin:" << setfill('.')  << setw(12)<< noiseMin << "\n";
        ssAnno << setfill(' ') << setw(10) << "noiseMax:" << setfill('.')  << setw(12)<< noiseMax << "\n\n";
        ssAnno << "Noise2\n";
        ssAnno << setfill(' ') << setw(10) << "Octaves:" << setfill('.')  << setw(12)<< octave2 << "\n";
        ssAnno << setfill(' ') << setw(10) << "Rough:" << setfill('.')  << setw(12)<< rough2 << "\n";
        ssAnno << setfill(' ') << setw(10) << "Freq:" << setfill('.')  << setw(12)<< freq2 << "\n";
        ssAnno << setfill(' ') << setw(10) << "fJump:" << setfill('.')  << setw(12)<< fjump2 << "\n";
        ssAnno << setfill(' ') << setw(10) << "noiseMin:" << setfill('.')  << setw(12)<< noiseMin2 << "\n";
        ssAnno << setfill(' ') << setw(10) << "noiseMax:" << setfill('.')  << setw(12)<< noiseMax2 << "\n";
        std::cout << ssAnno.str();
        Magick::Geometry textBox(200, 200);
        Magick::GravityType gravity(Magick::NorthWestGravity);
        Magick::Image image;
        image.backgroundColor("#000000");
        image.boxColor("#000000");
        image.fillColor("#999999");
        image.read(filepath + ".exr");
        image.fontPointsize(12);
        image.font("courier");
        image.annotate(ssAnno.str(), gravity); 
        image.write(filepath + ".exr");*/

        std::cout << filepath << "\n";
    }
    //initLights();

    std::cout << "Total Time: " << totalTimer.elapsed() << "\n";


    return 0;
}
