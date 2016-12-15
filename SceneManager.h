#ifndef __SCENE_MANAGER_H__
#define __SCENE_MANAGER_H__

#include "fields.h"
#include "OIIOFiles.h"
#include "boundingbox.h"
#include "Camera.h"
#include "renderlog.h"
#include "ColorSlider.h"

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
        double rayMarchLightScatter(const Vector& x, light l, const FloatVolumeBase vol) const;
        double rayMarchDSM(const Vector& x, const DeepShadowMap* dsm) const;
        void renderImage(int frameNumber);
        

        std::vector<std::string> getAnnotation();

        //These contain everything in our scene
        std::vector<light> lights;
        void pushLight(light l){ lights.push_back(l);};
        void clearLights(){ lights.clear();};

        std::vector<DSMPtr> lightGrids;
        void pushDSM(DSMPtr lg){ lightGrids.push_back(lg);};
        void clearDSMs(){ lightGrids.clear();};

        std::vector<FloatVolumeBase> volumes;
        void pushFloatVolume(FloatVolumeBase f){ volumes.push_back(f);};
        void clearFloatVolumes(){ volumes.clear();};

        std::vector<ColorVolumeBase> colorVolumes;
        void pushColorVolume(ColorVolumeBase f){ colorVolumes.push_back(f);};
        void clearColorVolumes(){ colorVolumes.clear();};

        Camera camera;
        int WRITE_RENDER_LOG;
        int WRITE_RENDER_ANNOTATION;
        int ENABLE_LIGHTS;
        int ENABLE_DSM;
        double K;
        double emissive;

        double marchStep;
        double lightMarchStep;
        float bbSize;
        float gridSize;
        float DSMVoxelCount;
        float gridVoxelCount;
        //ColorSlider cSlider;
        RenderLog renderlog;
    private:

        std::string outFile;
        int startFrame;
        int endFrame;
        int width;
        int height;

        BoundingBox bb;

};
}
#endif
