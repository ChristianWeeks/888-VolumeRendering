
//#include "volume_operators.h"
#include "grid.h"
//#include "Image.h"
#include "OIIOFiles.h"
#include "boundingbox.h"
#include "Camera.h"
#include "light.h"
#include <vector>
#include <string.h>
#include <stdio.h>
//#include <boost/shared_ptr.hpp>
#include <boost/timer.hpp>
#include <iostream>
#include <fstream>
#include <memory>

const double PI  =3.141592653589793238463;

//Light scatter / attentuation coefficient
double K = 1.0;
double emissive = 0.00001;

//Discrete step for our marching.  Delta S in our equations
double marchStep = 0.02;

std::vector<lux::light> lights;
std::vector<std::shared_ptr<lux::Volume<float> > > volumes;

std::vector<float> black = {0.0, 0.0, 0.0, 0.0};

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

std::vector<float> rayMarch(Camera cam, lux::Vector n, float start, float end){

    //extinction coefficient
    double totalMarchLength = end - start;
    //std::cout << "Start: " << start << "\nEnd: " << end << "\n"; 
    double marchLen = 0.0;
    double T = 1.0;
    //std::cout << '(' << ray[0] << ", "  << ray[1] << ", " << ray[2] << ")\n";
    std::vector<float> C {0.0, 0.0, 0.0, 1.0};
    //if (ray[0] < 0.5){
    //    C[0] = 1.0;
    //}
    std::vector<float> smokeColor {0.2, 0.3, 0.92, 1.0};
    lux::Vector x = cam.eye() + n*start;

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
                    C[0] += lights[i].c[0] * lightTransmissivity * tVal;
                    C[1] += lights[i].c[1] * lightTransmissivity * tVal;
                    C[2] += lights[i].c[2] * lightTransmissivity * tVal;
                }
                T *= deltaT;

                if (emissive) {
                    C[0] += smokeColor[0] * tVal * emissive;
                    C[1] += smokeColor[1] * tVal * emissive;
                    C[2] += smokeColor[2] * tVal * emissive;
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
    auto sphere = std::make_shared<lux::SphereVolume<float> >(2);
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
    auto pyroSphere = std::make_shared<lux::PyroSphereVolume>(2.0, 1.0);


    lux::FloatGrid pyroSphereGrid = lux::FloatGrid(pyroSphere, lux::Vector(-4, -4, -4), 8.0, 200);
    auto griddedPyro = std::make_shared<lux::GriddedVolume>(pyroSphereGrid);
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
    //volumes.push_back(pyroSphere);
    volumes.push_back(griddedPyro);
    //volumes.push_back(leftArm);
    //volumes.push_back(rightArm);
    //volumes.push_back(new lux::Scale_S_Volume(sphere, lux::Vector(0.0, 0.0, 0.0), 2.0));
    //volumes.push_back(new lux::Scale_S_Volume(sphere, lux::Vector(0.0, 0.0, 0.0), 2.0));
    //volumes.push_back(new lux::TranslateVolume<float>(box, lux::Vector(0.0, 0.0, -2.0)));
    //volumes.push_back(new lux::TranslateVolume<float>(cone, lux::Vector(0.0, 0.4, -2.0)));
}

void renderImage(Camera cam, BoundingBox bb, float bbSize, int w, int h, std::string filepath){
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
                std::vector<float> C = rayMarch(cam, r, intersects[0], intersects[1]);
                img.setPixel( i, j, C);
            }
            else{
                img.setPixel(i, j, black);
            }
           // std::cout << "4\n";
        }

        //Print our progress to make sure we aren't frozen
        if (i % progressMod == 0){
            std::cout << (i*100/w) << "%\n";
        }
    }

    float elapsedTime = t.elapsed();
    std::cout << "Time: " << elapsedTime << "\n";

    //Write our image
    std::ofstream logFile;
    logFile.open(filepath + ".log");
    logFile << "W: \t\t\t" << w << "\n"; 
    logFile << "H: \t\t\t" << h << "\n";
    logFile << "BB Size: \t\t" << bbSize << "\n";
    logFile << "TotalTime: \t\t" << elapsedTime << "\n";
    logFile.close();
    filepath += ".exr";
    lux::writeOIIOImage(filepath.c_str(), img, 1.0, 1.0);
}

int main(int argc, char **argv){

    int w = 480;
    int h = 270;
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

    std::string filepath("output/");
    if (argc >= 2){
        filepath += std::string(argv[1]);
    }
    else{
        filepath += std::string("lastOutput");
    }
    initLights();
    initVolumes();

    Camera mainCam;
    mainCam.setEyeViewUp(lux::Vector(0.0, 0.0, 10), lux::Vector(0,0,-1), lux::Vector(0,1,0));
    float bbSize = 3.0;
    BoundingBox bb(lux::Vector(-bbSize, -bbSize, -bbSize), lux::Vector(bbSize, bbSize, bbSize));

    renderImage(mainCam, bb, bbSize, w, h, filepath);

    return 0;
}
