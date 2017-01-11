#include "SceneManager.h"
#include "WedgeAttribute.h"

#include <vector>
#include <stdlib.h>
#include <boost/timer.hpp>

//const double PI  =3.141592653589793238463;
//------------------------------------------------------------------------------
//GLOBAL SCENE SETTINGS
//------------------------------------------------------------------------------
//Light scatter / attentuation coefficient
const double K = 1.0;
double emissive = 0.05;

//Discrete step for our marching.  Delta S in our equations
const double marchStep = 0.08;
const double lightMarchStep = 0.12;

const int startFrame = 30;
const int endFrame =  31;

//BBsize is half the length of an edge of the size of our cube. So bSize = 3 means our bounding box is a cube with edges length 6
const float bbSize = 3.5;
const float gridSize = 2 * bbSize;
const float DSMVoxelCount = 200;
const float gridVoxelCount = 800;
//Width and Height may change based on input
int w = 480;
int h = 270;
Camera mainCam;

lux::ColorSlider cSlider;

int main(int argc, char **argv){

    boost::timer totalTimer;
    lux::SceneManager scene(argv[1]);
    if (argc >= 3){
        scene.setResolution(atoi(argv[2]));
    }

    scene.K = K;
    scene.emissive = emissive;
    scene.marchStep = marchStep;
    scene.lightMarchStep = lightMarchStep;
    scene.bbSize = bbSize;
    scene.gridSize = gridSize;
    scene.DSMVoxelCount = DSMVoxelCount;
    scene.gridVoxelCount = gridVoxelCount;
    scene.WRITE_RENDER_ANNOTATION = 1;

    scene.setFrameRange(startFrame, endFrame);

    //Initialize noise
    /*int color_octaves = 1;
    float color_roughness = 0.7;
    float color_frequency = 0.7;
    float color_fjump = 2.2;
    float color_noiseMin = 0.0;
    float color_noiseMax = 1.0;

    SimplexNoiseObject colorNoise(color_octaves, color_roughness, color_frequency, color_fjump, color_noiseMin, color_noiseMax, 0);
    auto colorVolume = std::make_shared<lux::SimplexNoiseColorVolume> (colorNoise, 0.0, 10.0, 20.0);
    scene.colorVolumes.push_back(colorVolume);*/

    lux::Color c_black(0.0, 0.0, 0.0, 0.0);    //at weight 0.0
    lux::Color c_red(0.05, 0.3, 0.9, 0.0);    //at weight 0.0
    lux::Color c_orange(0.0, 0.5, 1.0, 0.0);     //at weight 0.5
    lux::Color c_yellow(0.1, 1.0, 1.0, 0.0);    //at weight 0.85
    lux::Color c_white(1.0, 1.0, 1.0, 0.0); //at weight 1.0
    scene.cSlider.addColor(0.0, c_black);
    scene.cSlider.addColor(4.0, c_red);
    scene.cSlider.addColor(20.0, c_orange);
    scene.cSlider.addColor(30.0, c_yellow);
    scene.cSlider.addColor(40.0, c_white);

    WedgeAttribute wClump;
    WedgeAttribute wFreq1;
    WedgeAttribute wFreq2;
    WedgeAttribute wUpBound1;
    WedgeAttribute wUpBound2;
    WedgeAttribute wLowBound1;
    WedgeAttribute wLowBound2; 
    WedgeAttribute radius;
    WedgeAttribute wOffset; 

    wOffset.key(1, 0.0);
    wOffset.key(400, 3.5);

    wFreq1.key(100, 3.0);
    wFreq2.key(50, 4.0);

    wLowBound2.key(100, -0.4);
    wUpBound2.key(100, 0.4);

    wLowBound1.key(200, -0.5);
    wUpBound1.key(200, 0.5);

    wClump.key(250, 1.0);

    radius.key(300, 1.4);
    //wClump.key(80, 2.0);

    //Set up our wedge values
    int octave1 = 4;
    float rough1 = 0.5;
    float freq1 = 0.00;
    float fjump1 = 2.2;
    float noiseMin1 = 0.0;
    float noiseMax1 = 1.0;
    float offset1 = 0;

    int octave2 = 4;
    float rough2 = 0.5;
    float freq2 = 2.0;
    float fjump2 = 2.2;
    float noiseMin2 = -0.4;
    float noiseMax2 = 0.4;
    float offset2 = 0.3;

    int numDots = 100000;

    for (int i = startFrame; i < endFrame; i++){

        //-------------------------------------------------------------------------------------------------------------------------------------
        // SET UP OUR NOISE OBJECT, VOLUMES, GRIDS, AND SHADOW MAPS HERE
        //-------------------------------------------------------------------------------------------------------------------------------------
        SimplexNoiseObject wispNoise1(octave1, rough1, wFreq1.get(i), fjump1, wLowBound1.get(i), wUpBound1.get(i), offset1);
        SimplexNoiseObject wispNoise2(octave2, rough2, wFreq2.get(i), fjump2, wLowBound2.get(i), wUpBound2.get(i), wOffset.get(i));
        /*scene.renderlog.addLine(std::string("GLOBALS"));
        scene.renderlog.addLine(std::string(""));
        scene.renderlog.addLine(std::string("Frame"));
        scene.renderlog.addLine(std::to_string(i));
        scene.renderlog.addLine(std::string("Clump"));
        scene.renderlog.addLine(std::to_string(wClump.get(i)));
        scene.renderlog.addLine(std::string("Radius"));
        scene.renderlog.addLine(std::to_string(radius.get(i)));
        scene.renderlog.addLine(std::string("Wisp Dots"));
        scene.renderlog.addLine(std::to_string(numDots));

        scene.renderlog.addVector(scene.getAnnotation());
        scene.renderlog.addLine(std::string("NOISE1"));
        scene.renderlog.addLine(std::string(""));

        scene.renderlog.addVector(wispNoise1.getAnnotation());
        scene.renderlog.addLine(std::string("NOISE2"));
        scene.renderlog.addLine(std::string(""));

        scene.renderlog.addVector(wispNoise2.getAnnotation());*/

        //Set up our volume
        auto constVol = new lux::ConstantVolumef(0.0);

        //Stamp our wisps
        boost::timer wispTimer;
        auto wispGrid = new lux::DensityGrid(constVol, lux::Vector(-bbSize, -bbSize, -bbSize), gridSize + 0.2, gridVoxelCount);
        wispGrid->StampWisp(lux::Vector(0, 0, 0), wispNoise1, wispNoise2, wClump.get(i), radius.get(i), numDots, 0);
        auto griddedWisp = new lux::GriddedVolume(wispGrid);
        std::cout << "Wisp Build Time: " << wispTimer.elapsed() << "\n";

        scene.volumes.push_back(lux::FloatVolumeBase(griddedWisp));

        //-------------------------------------------------------------------------------------------------------------------------------------
        boost::timer t;
        scene.renderImage(i);
        float elapsedTime = t.elapsed();
        std::cout << "Render Time: " << elapsedTime << "\n";

        //Empty our volume vector for next iteration
        scene.volumes.clear();
    }
    //initLights();
    std::cout <<"Total: " << totalTimer.elapsed() << "\n";

    return 0;
}
