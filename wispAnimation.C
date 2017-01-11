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

    //Initialize noise
    int color_octaves = 1;
    float color_roughness = 0.7;
    float color_frequency = 0.7;
    float color_fjump = 2.2;
    float color_noiseMin = 0.0;
    float color_noiseMax = 1.0;

    SimplexNoiseObject colorNoise(color_octaves, color_roughness, color_frequency, color_fjump, color_noiseMin, color_noiseMax, 0);
    auto colorVolume = std::make_shared<lux::SimplexNoiseColorVolume> (colorNoise, 0.0, 10.0, 20.0);
    scene.colorVolumes.push_back(colorVolume);

    //Init random device for placing clusters in scene
    std::random_device rnd;
    std::mt19937 rng(rnd());
    std::uniform_real_distribution<> udist(-1, 1);

    int numClusters = 1000;
    lux::Vector clusterCoords[1000];
    for (int i = 0; i < numClusters; i++){
        float randX = udist(rng);
        float randY = udist(rng);
        float randZ = udist(rng);

        clusterCoords[i][0] = scaled_octave_noise_3d(color_octaves, color_roughness, color_frequency, color_fjump, -(bbSize - 0.2), bbSize - 0.2, randX, randY, randZ);
        clusterCoords[i][1] = scaled_octave_noise_3d(color_octaves, color_roughness, color_frequency, color_fjump, -(bbSize - 0.2), bbSize - 0.2, randX + 0.5, randY + 0.5, randZ + 0.5);
        clusterCoords[i][2] = scaled_octave_noise_3d(color_octaves, color_roughness, color_frequency, color_fjump, -(bbSize - 0.2), bbSize - 0.2, randX - 0.5, randY - 0.5, randZ - 0.5);

    }

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

    //Set up our wedge values
    int octave1 = 4;
    float rough1 = 0.5;
    float freq1 = 4.0;
    float fjump1 = 2.2;
    float noiseMin1 = 0.0;
    float noiseMax1 = 2.0;
    float offset1 = 0;

    int octave2 = 4; 
    float rough2 = 0.5;
    float freq2 = 10.0;
    float fjump2 = 2.2;
    float noiseMin2 = -0.005;
    float noiseMax2 = 0.005;
    float offset2 = 0.0;

    float clump = 1.0;
    float radius = 0.2;
    float numDots = 400000;

    for (int i = frameStart; i < frameEnd; i++){

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
        for(int j = 0; j < numClusters; j++){
            wispGrid.get()->StampWisp(clusterCoords[j], wispNoise1, wispNoise2, clump, radius, numDots, udist(rng));
        }
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
