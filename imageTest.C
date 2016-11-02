
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

#include "ColorSlider.h"
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
double emissive = 2.0;

//Discrete step for our marching.  Delta S in our equations
const double marchStep = 0.04;
const double lightMarchStep = 0.12;

const int frameStart = 1; 
const int frameEnd = 2; 

//BBsize is half the length of an edge of the size of our cube. So bSize = 3 means our bounding box is a cube with edges length 6
const float bbSize = 3.5;
const float gridSize = 2 * bbSize;
const float lightGridVoxelCount = 200;
const float gridVoxelCount = 1200;
//Width and Height may change based on input
int w = 480;
int h = 270;
Camera mainCam;


lux::ColorSlider cSlider;

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
    lux::Color color(0.5, 1.0, 0.0, 1.0);
    while (marchLen < totalMarchLength){
        //Check each volume
        for(int j = 0; j < volumes.size(); j++){
            double density = volumes[j].get()->eval(x);
            if (density > 0.0){
                

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
                    color = cSlider.getColor(density);
                    //std::cout << color << "Density: " << density << "\n";
                    //The field color's contribution will be scaled by the density, so sparse areas don't become super bright
                    //fieldColor = colorVolumes[0].get()->eval(x) * 0.2;
                    //color += colorClamp(fieldColor);
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

    //Write our image
    std::ostringstream ss;
    //ss << setfill(' ') << setw(20) << "Eye:" << setfill('-')  << setw(30) << mainCam.eye() << "\n";
    //ss << setfill(' ') << setw(20) << "View:" << setfill('-')  << setw(30) << mainCam.view() << "\n";
    /*ss << setfill(' ') << setw(20) << "W:" << setfill('-')  << setw(30) << w << "\n";
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
    renderLog.writeToFile();*/

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
    mainCam.setEyeViewUp(lux::Vector(6.0, 0.0, 6.0), lux::Vector(-1,0,-1), lux::Vector(0,1,0));
    BoundingBox bb(lux::Vector(-bbSize, -bbSize, -bbSize), lux::Vector(bbSize, bbSize, bbSize));

    //lux::light l1(lux::light(lux::Color(0.4, 0.80, 1.0, 1.0), lux::Vector(0.0, -1.0, 0.0), lux::Vector(-2.0, 5.0, 4.0), 1.0));
    lux::light l1(lux::Color(0.7, 0.9, 0.1, 1.0), lux::Vector(0.0, -1.0, 0.0), lux::Vector(-3.0, 4.0, 0.0), 1.0);
    //lux::light l2(lux::Color(0.95, 0.4, 0.4, 1.0), lux::Vector(0.0, -1.0, 0.0), lux::Vector(3.0, 4.0, 0.0), 1.0);
    //lights.push_back(l1);
    //lights.push_back(l2);

    int color_octaves = 1;
    float color_roughness = 0.7;
    float color_frequency = 0.15;
    float color_fjump = 2.2;
    float color_noiseMin = -0.4; 
    float color_noiseMax = 0.7;

    SimplexNoiseObject colorNoise(color_octaves, color_roughness, color_frequency, color_fjump, color_noiseMin, color_noiseMax, 0);
    auto colorVolume = std::make_shared<lux::SimplexNoiseColorVolume> (colorNoise, 0.0, 10.0, 20.0);
    colorVolumes.push_back(colorVolume);

    lux::Color c_black(0.0, 0.0, 0.0, 0.0);    //at weight 0.0
    lux::Color c_red(0.8, 0.1, 0.0, 0.0);    //at weight 0.0
    lux::Color c_orange(1.0, 0.5, 0.1, 0.0);     //at weight 0.5
    lux::Color c_yellow(1.0, 1.0, 0.1, 0.0);    //at weight 0.85
    lux::Color c_white(1.0, 1.0, 1.0, 0.0); //at weight 1.0
    cSlider.addColor(0.0, c_black);
    cSlider.addColor(0.4, c_red);
    cSlider.addColor(2.0, c_orange);
    cSlider.addColor(3.0, c_yellow);
    cSlider.addColor(10.0, c_white);

    //Set up our wedge values
    int octave1 = 3;
    float rough1 = 0.5;
    float freq1 = 0.3;
    float fjump1 = 2.2;
    float noiseMin1 = 0.6;
    float noiseMax1 = 1.6;
    float offset1 = 0;

    int octave2 = 3;
    float rough2 = 0.5;
    float freq2 = 1.2;
    float fjump2 = 2.2;
    float noiseMin2 = -0.001;
    float noiseMax2 = 0.0;
    float offset2 = 0.0;

    WedgeAttribute offset1Wedge; 
    WedgeAttribute offset2Wedge; 
    WedgeAttribute radius;

    WedgeAttribute nMin1Wedge;
    WedgeAttribute nMax1Wedge;

    WedgeAttribute nMin2Wedge;
    WedgeAttribute nMax2Wedge;

    WedgeAttribute freqWedge;

    WedgeAttribute numDots;
    WedgeAttribute clump;
    clump.key(1, 0.3);
    clump.key(120, 2.0);
    numDots.key(48, 2000000);
    /*numDots.key(72, 3000000);
    numDots.key(108, 3000000);
    numDots.key(115, 17000000);*/

    nMin2Wedge.key(72, -0.3);
    nMax2Wedge.key(72, 0.3);

   /* nMin2Wedge.key(83, -0.1);
    nMax2Wedge.key(83, 0.1);

    nMin2Wedge.key(105, -0.1);
    nMax2Wedge.key(105, 0.1);

    nMin2Wedge.key(110, -0.2);
    nMax2Wedge.key(110, 0.2);

    nMin2Wedge.key(128, -0.6);
    nMax2Wedge.key(128, 0.6);*/

    freqWedge.key(75, 1.0);

 
    /*nMin1Wedge.key(103, 1.0);
    nMax1Wedge.key(103, 1.8);

    nMin1Wedge.key(110, 0.8);
    nMax1Wedge.key(110, 1.2);*/

    radius.key(30, 1.0);
    /*radius.key(48, 0.8);
    radius.key(72, 0.25);
    radius.key(76, 0.2);
    radius.key(95, 0.2);
    //radius.key(97, 0.5);
    radius.key(101, 0.2);
    radius.key(105, 0.2);
    radius.key(110, 1.4);
    radius.key(120, 1.8);
    radius.key(125, 2.0);
    radius.key(135, 2.1);*/

    offset1Wedge.key(1, 0.00);
    offset2Wedge.key(1, 0.00);

/*    offset1Wedge.key(70, 1.5);
    offset2Wedge.key(70, 1.5);

    offset1Wedge.key(93, 1.8);
    offset2Wedge.key(93, 1.8);

    offset1Wedge.key(200, 3.0);
    offset2Wedge.key(200, 3.0);*/

    /*nMin1Wedge.key(20, -1.0);
    nMax1Wedge.key(20, 1.0);
    nMin1Wedge.key(31, 0.99);
    nMax1Wedge.key(31, 1.01);
    nMin1Wedge.key(50, 0.00);
    nMax1Wedge.key(50, 2.00);*/
    nMin1Wedge.key(72, 0.6);
    nMax1Wedge.key(72, 1.4);
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
        offset1 = offset1Wedge.get(i);
        offset2 = offset2Wedge.get(i);
        noiseMin1 = nMin1Wedge.get(i);
        noiseMax1 = nMax1Wedge.get(i);
        freq1 = freqWedge.get(i);

        noiseMin2 = nMin2Wedge.get(i);
        noiseMax2 = nMax2Wedge.get(i);
        //WEDGE----------------------------------------------

        //-------------------------------------------------------------------------------------------------------------------------------------
        // SET UP OUR NOISE OBJECT, VOLUMES, GRIDS, AND SHADOW MAPS HERE
        //-------------------------------------------------------------------------------------------------------------------------------------
        SimplexNoiseObject wispNoise1(octave1, rough1, freq1, fjump1, noiseMin1, noiseMax1, offset1);
        SimplexNoiseObject wispNoise2(octave2, rough2, freq2, fjump2, noiseMin2, noiseMax2, offset2);

        //Set up our volume
        //auto sphereVol = std::make_shared<lux::SphereVolume<float> >(2.0);
        auto constVol = std::make_shared<lux::ConstantVolume<float> > (0.0);
        //auto multVol = std::make_shared<lux::MultVolume<float> >(sphereVol, constVol);

        boost::timer wispTimer;
        auto wispGrid = std::make_shared<lux::DensityGrid>(constVol, lux::Vector(-bbSize, -bbSize, -bbSize), gridSize + 0.2, gridVoxelCount);
        wispGrid.get()->StampWisp(lux::Vector(0, 0, 0), wispNoise1, wispNoise2, clump.get(i), radius.get(i), numDots.get(i));
        auto griddedWisp = std::make_shared<lux::GriddedVolume>(wispGrid);
        std::cout << "Wisp Build Time: " << wispTimer.elapsed() << "\n";

        //boost::timer dsmTimer;
        //lux::DeepShadowMap dsm1(l1, lightMarchStep, griddedWisp, lux::Vector(-bbSize, -bbSize, -bbSize), gridSize, lightGridVoxelCount);
        //lux::DeepShadowMap dsm2(l2, lightMarchStep, griddedWisp, lux::Vector(-bbSize, -bbSize, -bbSize), gridSize, lightGridVoxelCount);
        //lightGrids.push_back(dsm1);
        //lightGrids.push_back(dsm2);
        //std::cout << "DSM Construction Time: " << dsmTimer.elapsed() << "\n";

        volumes.push_back(griddedWisp);

        boost::timer renderTimer;
        //mainCam.setEyeViewUp(lux::Vector(9.0 - ((float)i * 0.02), 0.0, 9.0 - ((float)i * 0.02)), lux::Vector(-1,0,-1), lux::Vector(0,1,0));
        //-------------------------------------------------------------------------------------------------------------------------------------
        renderImage(mainCam, bb, renderLog);
        std::cout << filepath << "\n";
        std::cout << "Render Time: " << renderTimer.elapsed() << "\n";

        //Empty our volume vector for next iteration
        volumes.clear();

        //Annotate our image for the wedge
        std::ostringstream ssAnno;
        ssAnno << setfill(' ') << setw(10) << "Frame:" << setfill('.')  << setw(12) << i << "\n";
        ssAnno << setfill(' ') << setw(10) << "Radius:" << setfill('.')  << setw(12) << radius.get(i) << "\n";
        ssAnno << setfill(' ') << setw(10) << "Dots:" << setfill('.')  << setw(12)<< numDots.get(i) << "\n";
        ssAnno << setfill(' ') << setw(10) << "Clump:" << setfill('.')  << setw(12)<< clump.get(i) << "\n";
        ssAnno << setfill(' ') << setw(10) << "Emissive:" << setfill('.')  << setw(12)<< emissive << "\n";
        ssAnno << setfill(' ') << setw(10) << "K:" << setfill('.')  << setw(12)<< K << "\n\n";
        ssAnno << "Noise1\n";
        ssAnno << setfill(' ') << setw(10) << "Octaves:" << setfill('.')  << setw(12)<< octave1 << "\n";
        ssAnno << setfill(' ') << setw(10) << "Rough:" << setfill('.')  << setw(12)<< rough1 << "\n";
        ssAnno << setfill(' ') << setw(10) << "Freq:" << setfill('.')  << setw(12)<< freq1 << "\n";
        ssAnno << setfill(' ') << setw(10) << "noiseMin:" << setfill('.')  << setw(12)<< noiseMin1 << "\n";
        ssAnno << setfill(' ') << setw(10) << "noiseMax:" << setfill('.')  << setw(12)<< noiseMax1 << "\n";
        ssAnno << setfill(' ') << setw(10) << "Offset:" << setfill('.')  << setw(12)<< offset1 << "\n\n";
        ssAnno << "Noise2\n";
        ssAnno << setfill(' ') << setw(10) << "Octaves:" << setfill('.')  << setw(12)<< octave2 << "\n";
        ssAnno << setfill(' ') << setw(10) << "Rough:" << setfill('.')  << setw(12)<< rough2 << "\n";
        ssAnno << setfill(' ') << setw(10) << "Freq:" << setfill('.')  << setw(12)<< freq2 << "\n";
        ssAnno << setfill(' ') << setw(10) << "fJump:" << setfill('.')  << setw(12)<< fjump2 << "\n";
        ssAnno << setfill(' ') << setw(10) << "noiseMin:" << setfill('.')  << setw(12)<< noiseMin2 << "\n";
        ssAnno << setfill(' ') << setw(10) << "noiseMax:" << setfill('.')  << setw(12)<< noiseMax2 << "\n";
        ssAnno << setfill(' ') << setw(10) << "Offset:" << setfill('.')  << setw(12)<< offset2 << "\n\n";
        //std::cout << ssAnno.str();
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
        image.write(filepath + ".exr");

    }
    //initLights();
    std::cout <<"Total: " << totalTimer.elapsed() << "\n";



    return 0;
}
