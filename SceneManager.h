#ifndef __SCENE_MANAGER_H__
#define __SCENE_MANAGER_H__

#include "fields.h"
#include "OIIOFiles.h"
#include "boundingbox.h"
#include "Camera.h"
#include "renderlog.h"
//#include "ColorSlider.h"

#include <boost/timer.hpp>
#include <omp.h>
#include <vector>
#include <stdio.h>
#include <iomanip>
#include <memory>
#include <random>
namespace lux{
class SceneManager{
    public:
        SceneManager(std::string filepath);
        ~SceneManager();

        void setFrameRange(int start, int end);
        void setResolution(int res);
        Color rayMarch(const Vector& n, float start, float end) ;
        double rayMarchLightScatter(const Vector& x, light l) const;
        double rayMarchDSM(const Vector& x, const DeepShadowMap* dsm) const;
        void renderImage(int frameNumber);
        

        std::vector<std::string> getAnnotation();

        //These contain everything in our scene
        std::vector<light> lights;
        void pushLight(light l){ lights.push_back(l);};
        void clearLights(){ lights.clear();};

        std::vector<DSMBase> lightGrids;
        void pushDSM(DSMBase lg){ lightGrids.push_back(lg);};
        void clearDSMs(){ lightGrids.clear();};

        std::vector<FloatVolumeBase> volumes;
        void pushFloatVolume(FloatVolumeBase f){ 
            volumes.push_back(f);
            };
        void clearFloatVolumes(){ 
            volumes.clear();
            boundingboxes.clear();
        
        };

        std::vector<ColorVolumeBase> colorVolumes;
        void pushColorVolume(ColorVolumeBase f){ colorVolumes.push_back(f);};
        void clearColorVolumes(){ colorVolumes.clear();};
        
        void pushBoundingBoxes(std::vector<BoundingBox> b){boundingboxes.insert(boundingboxes.end(), b.begin(), b.end());};
        std::vector<BoundingBox> boundingboxes;

        Camera camera;
        unsigned short WRITE_RENDER_LOG;
        unsigned short WRITE_RENDER_ANNOTATION;
        unsigned short ENABLE_LIGHTS;
        unsigned short ENABLE_DSM;
        unsigned short UNION_COLLISIONS;
        unsigned short ADD_COLLISIONS;
        unsigned short raysPerPixel;
        float MARCH_INCREASE_DIST;
        float MARCH_MULTIPLIER;
        double K;
        double emissive;
        light smokeColor;

        double marchStep;
        double lightMarchStep;
        //ColorSlider cSlider;
        RenderLog renderlog;
    private:

        std::string outFile;
        int startFrame;
        int endFrame;
        int width;
        int height;
};
}
#endif
