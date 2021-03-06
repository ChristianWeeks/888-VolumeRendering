#include "SceneManager.h"
#include "WedgeAttribute.h"

#include <vector>
#include <stdlib.h>
#include <boost/timer.hpp>

const double PI  =3.141592653589793238463;
//------------------------------------------------------------------------------
//GLOBAL SCENE SETTINGS
//------------------------------------------------------------------------------
//Light scatter / attentuation coefficient
const double K = 1.0;
double emissive = 0.05;

//Discrete step for our marching.  Delta S in our equations
const double marchStep = 0.008;
const double lightMarchStep = 0.12;

const int frameStart = 40;
const int frameEnd =  41;

//BBsize is half the length of an edge of the size of our cube. So bSize = 3 means our bounding box is a cube with edges length 6
const float bbSize = 3.5;
const float gridSize = 2 * bbSize;
const float DSMVoxelCount = 200;
const float gridVoxelCount = 1000;
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

    int color_octaves = 1;
    float color_roughness = 0.7;
    float color_frequency = 0.6;
    float color_fjump = 2.2;
    float color_noiseMin = -0.4;
    float color_noiseMax = 0.7;

    SimplexNoiseObject colorNoise(color_octaves, color_roughness, color_frequency, color_fjump, color_noiseMin, color_noiseMax, 0);
    auto colorVolume = std::make_shared<lux::SimplexNoiseColorVolume> (colorNoise, 0.0, 10.0, 20.0);
    scene.colorVolumes.push_back(colorVolume);

    /*lux::Color c_black(0.0, 0.0, 0.0, 0.0);    //at weight 0.0
    lux::Color c_red(0.8, 0.1, 0.0, 0.0);    //at weight 0.0
    lux::Color c_orange(1.0, 0.5, 0.1, 0.0);     //at weight 0.5
    lux::Color c_yellow(1.0, 1.0, 0.1, 0.0);    //at weight 0.85
    lux::Color c_white(1.0, 1.0, 1.0, 0.0); //at weight 1.0*/
    lux::Color c_black(0.0, 0.0, 0.0, 0.0);    //at weight 0.0
    lux::Color c_red(0.05, 0.3, 0.9, 0.0);    //at weight 0.0
    lux::Color c_orange(0.0, 0.5, 1.0, 0.0);     //at weight 0.5
    lux::Color c_yellow(0.1, 1.0, 1.0, 0.0);    //at weight 0.85
    lux::Color c_white(1.0, 1.0, 1.0, 0.0); //at weight 1.0
    /*lux::Color c_black(0.0, 0.0, 0.0, 0.0);    //at weight 0.0
    lux::Color c_red(0.05, 0.9, 0.3, 0.0);    //at weight 0.0
    lux::Color c_orange(0.0, 1.0, 0.5, 0.0);     //at weight 0.5
    lux::Color c_yellow(0.1, 1.0, 0.8, 0.0);    //at weight 0.85
    lux::Color c_white(1.0, 1.0, 1.0, 0.0); //at weight 1.0*/

    /*lux::Color c_deepBlue(0.1, 0.05, 1.0, 0.0);    //at weight 0.0
    lux::Color c_purp(0.95, 0.05, 0.8, 0.0);    //at weight 0.0
    lux::Color c_orange(1.0, 0.5, 0.1, 0.0);     //at weight 0.5
    lux::Color c_yellow(0.95, 1.0, 0.1, 0.0);    //at weight 0.85
    lux::Color c_white(1.0, 1.0, 1.0, 0.0); //at weight 1.0
    lux::Color c_deepGreen(0.4, 1.0, 0.4, 0.0); //at weight 1.0*/
    scene.cSlider.addColor(0.0, c_black);
    scene.cSlider.addColor(4.0, c_red);
    scene.cSlider.addColor(20.0, c_orange);
    scene.cSlider.addColor(30.0, c_yellow);
    scene.cSlider.addColor(40.0, c_white);

    //Set up our wedge values
    int octave1 = 3;
    float rough1 = 0.5;
    float freq1 = 0.3;
    float fjump1 = 2.2;
    float noiseMin1 = 0.6;
    float noiseMax1 = 1.6;
    float offset1 = 0;

    int octave2 = 5;
    float rough2 = 0.5;
    float freq2 = 0.5;
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
    clump.key(1, 0.2);

    numDots.key(48, 6000000);
    numDots.key(72, 3000000);
    numDots.key(108, 3000000);
    numDots.key(115, 17000000);

    nMin2Wedge.key(72, -0.3);
    nMax2Wedge.key(72, 0.3);

    nMin2Wedge.key(83, -0.1);
    nMax2Wedge.key(83, 0.1);

    nMin2Wedge.key(105, -0.1);
    nMax2Wedge.key(105, 0.1);

    nMin2Wedge.key(110, -0.2);
    nMax2Wedge.key(110, 0.2);

    nMin2Wedge.key(128, -0.6);
    nMax2Wedge.key(128, 0.6);

    freqWedge.key(75, 1.0);

    nMin1Wedge.key(103, 1.0);
    nMax1Wedge.key(103, 1.8);

    nMin1Wedge.key(110, 0.8);
    nMax1Wedge.key(110, 1.2);

    radius.key(30, 1.0);
    radius.key(48, 0.8);
    radius.key(72, 0.25);
    radius.key(76, 0.2);
    radius.key(95, 0.2);
    //radius.key(97, 0.5);
    radius.key(101, 0.2);
    radius.key(105, 0.2);
    radius.key(110, 1.4);
    radius.key(120, 1.8);
    radius.key(125, 2.0);
    radius.key(135, 2.1);

    offset1Wedge.key(1, 0.00);
    offset2Wedge.key(1, 0.00);

    offset1Wedge.key(70, 1.5);
    offset2Wedge.key(70, 1.5);

    offset1Wedge.key(93, 1.8);
    offset2Wedge.key(93, 1.8);

    offset1Wedge.key(200, 2.3);
    offset2Wedge.key(200, 2.3);

    nMin1Wedge.key(20, -1.0);
    nMax1Wedge.key(20, 1.0);
    nMin1Wedge.key(31, 0.99);
    nMax1Wedge.key(31, 1.01);
    nMin1Wedge.key(50, 0.00);
    nMax1Wedge.key(50, 2.00);
    nMin1Wedge.key(72, 0.6);
    nMax1Wedge.key(72, 1.4);
    for (int i = frameStart; i < frameEnd; i++){

        offset1 = offset1Wedge.get(i);
        offset2 = offset2Wedge.get(i);
        noiseMin1 = nMin1Wedge.get(i);
        noiseMax1 = nMax1Wedge.get(i);
        freq1 = freqWedge.get(i);

        noiseMin2 = nMin2Wedge.get(i);
        noiseMax2 = nMax2Wedge.get(i);

        //-------------------------------------------------------------------------------------------------------------------------------------
        // SET UP OUR NOISE OBJECT, VOLUMES, GRIDS, AND SHADOW MAPS HERE
        //-------------------------------------------------------------------------------------------------------------------------------------
        SimplexNoiseObject wispNoise1(octave1, rough1, freq1, fjump1, noiseMin1, noiseMax1, offset1);
        SimplexNoiseObject wispNoise2(octave2, rough2, freq2, fjump2, noiseMin2, noiseMax2, offset2);
        scene.renderlog.addMap(scene.getAnnotation());
        scene.renderlog.addMap(wispNoise1.getAnnotation());
        scene.renderlog.addMap(wispNoise2.getAnnotation());

        //Set up our volume
        auto constVol = std::make_shared<lux::ConstantVolume<float> > (0.0);

        //Stamp our wisps
        boost::timer wispTimer;
        auto wispGrid = std::make_shared<lux::DensityGrid>(constVol, lux::Vector(-bbSize, -bbSize, -bbSize), gridSize + 0.2, gridVoxelCount);
        wispGrid.get()->StampWisp(lux::Vector(0, 0, 0), wispNoise1, wispNoise2, clump.get(i), radius.get(i), numDots.get(i));
        auto griddedWisp = std::make_shared<lux::GriddedVolume>(wispGrid);
        std::cout << "Wisp Build Time: " << wispTimer.elapsed() << "\n";

        scene.volumes.push_back(griddedWisp);

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
