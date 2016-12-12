#ifndef __SCENE_MANAGER_H__
#define __SCENE_MANAGER_H__

#include "grid.h"
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
        double rayMarchLightScatter(const Vector& x, light l, Volume<float> *vol) const;
        double rayMarchDSM(const Vector& x, const DeepShadowMap* dsm) const;
        void renderImage(int frameNumber);
        std::vector<std::string> getAnnotation();

        //These contain everything in our scene
        std::vector<light> lights;
        std::vector<DSMPtr> lightGrids;
        std::vector<FloatVolumeBase> volumes;
        std::vector<ColorVolumeBase> colorVolumes;

        Camera camera;
        int WRITE_RENDER_LOG;
        int WRITE_RENDER_ANNOTATION;
        double K;
        double emissive;

        double marchStep;
        double lightMarchStep;
        float bbSize;
        float gridSize;
        float DSMVoxelCount;
        float gridVoxelCount;
        ColorSlider cSlider;
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
