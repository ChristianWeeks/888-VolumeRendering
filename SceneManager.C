#include "SceneManager.h"

using namespace lux;

SceneManager::SceneManager(std::string filepath) :
    WRITE_RENDER_LOG(0),
    WRITE_RENDER_ANNOTATION(0),
    K(1.0),
    emissive(0.05),
    marchStep(0.04),
    lightMarchStep(0.1),
    bbSize(3.5),
    gridSize(7.2),
    DSMVoxelCount(200),
    gridVoxelCount(1000),
    outFile(filepath),
    startFrame(0),
    endFrame(1),
    width(480),
    height(270)
    {
    camera.setEyeViewUp(Vector(6.0, 0.0, 6.0), Vector(-1,0,-1), Vector(0,1,0));
    bb.setBounds(Vector(-bbSize, -bbSize, -bbSize), Vector(bbSize, bbSize, bbSize));
}

SceneManager::~SceneManager(){}

void SceneManager::setFrameRange(int start, int end){
    startFrame = start;
    endFrame = end;
}

void SceneManager::setResolution(int res){
    if (res == 1080){
        width = 1920;
        height = 1080;
    }
    else if (res == 720){
        width = 1280;
        height = 720;
    }
    else if (res == 540){
        width = 960;
        height = 540;
    }
    else{
        width = 480;
        height = 270;
    }
}

Color SceneManager::rayMarch(const Vector& n, float start, float end)  {
    //extinction coefficient
    double totalMarchLength = end - start;

    double marchLen = 0.0;
    double T = 1.0;
    Color C {0.0, 0.0, 0.0, 0.0};
    Color fieldColor;

    Vector x = camera.eye() + n*start;
    Color color(0.5, 1.0, 0.0, 1.0);
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
                    color *= density;
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

double SceneManager::rayMarchLightScatter(const Vector& x, light l, Volume<float> *vol) const {
    double marchLen = 0.0;
    Vector toLight(l.pos - x);
    double dist = toLight.magnitude();
    toLight.normalize();
    double T = 1.0;
    Vector x1(x);

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

double SceneManager::rayMarchDSM(const Vector& x, std::shared_ptr<DeepShadowMap> dsm) const {
    return exp(-K * dsm.get()->trilinearInterpolate(x));
}

void SceneManager::renderImage(int frameNumber){

    Image img;
    img.reset(width, height, 4);
    std::vector<float> black = {0.5, 0.5, 0.5, 0.0};
    //Fire a ray from eye through every pixel
    int progressMod = width / 10;
    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){
           // std::cout << "3\n";
            double normalizedPixelCoordX = (double)i / (double)width;
            double normalizedPixelCoordY = (double)j / (double)height;
            Vector r = camera.view(normalizedPixelCoordX, normalizedPixelCoordY);
            Ray ray(camera.eye(), r);
            std::vector<float> intersects = bb.intersect(ray, 0, 50);
            if(intersects[0] != -1.0){
                Color C = rayMarch(r, intersects[0], intersects[1]);
                std::vector<float> colorVector= {static_cast<float>(C[0]), static_cast<float>(C[1]), static_cast<float>(C[2]), static_cast<float>(C[3])};
                img.setPixel( i, j, colorVector);
            }
            else{
                img.setPixel(i, j, black);
            }
        }

        //Print our progress to make sure we aren't frozen
        if (i % progressMod == 0){
            std::cout << (i*100/width) << "%\n";
        }
    }


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
    std::string filepath("output/" + outFile);
    //Don't give it number padding if it is just a single render
    if (startFrame - endFrame > 1){
        filepath += ".";
        std::ostringstream ss;
        ss << setfill('0') << setw(4) << frameNumber;
        std::string numPadding = ss.str();
        filepath += numPadding;
    }

    //std::string filepath = renderLog.getFilepath();
    filepath += ".exr";
    writeOIIOImage(filepath.c_str(), img, 1.0, 1.0);
    //std::cout << renderLog.getBody();
}
