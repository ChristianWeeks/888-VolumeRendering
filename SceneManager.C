#include "SceneManager.h"

using namespace lux;

SceneManager::SceneManager(std::string filepath) :
    WRITE_RENDER_LOG(0),
    WRITE_RENDER_ANNOTATION(0),
    ENABLE_LIGHTS(1),
    ENABLE_DSM(0),
    UNION_COLLISIONS(1),
    ADD_COLLISIONS(0),
    raysPerPixel(1),
    MARCH_INCREASE_DIST(100),
    MARCH_MULTIPLIER(1.1),
    K(1.0),
    emissive(0.05),
    smokeColor(Color(1.0, 1.0, 1.0, 1.0), Vector(1.0, 0.0, 0.0), Vector(0.0, 0.0, 0.0), 1.0),
    marchStep(0.04),
    lightMarchStep(0.1),
    outFile(filepath),
    startFrame(0),
    endFrame(1),
    width(480),
    height(270)
    {
    camera.setEyeViewUp(Vector(0.0, 0.0, 6.0), Vector(0,0,-1), Vector(0,1,0));
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
    double activeMarchStep = marchStep;
    double increaseMarchCounter = 0.0;
    Vector x = camera.eye() + n*start;
    while (marchLen < totalMarchLength){
        if (T < 0.0000001){
            break;
        }

        //Check each volume
        float density = 0.0;
        for(unsigned int j = 0; j < volumes.size(); j++){
            if(UNION_COLLISIONS){
               /* if(volumes[j].get()->eval(x) > 5.0){
                    std::cout << "---------------------------\n";
                    std::cout << "x: " << x << "\n";
                    std::cout << "grid index: " << j << "\n";
                    std::cout << "Density: " << volumes[j].get()->eval(x) << "\n";
                }*/ 
                density = std::max(density, volumes[j].get()->eval(x));
            }
            else if(ADD_COLLISIONS){
                float newDensity = volumes[j].get()->eval(x);
                if(newDensity > 0){
                    density += newDensity;
                }
            }
        }
        if (density > 0.0){

            //step 1
            double deltaT = exp(-density * activeMarchStep * K);
            double tVal = T * (1 - deltaT);

            //March to each light
            if(ENABLE_LIGHTS){
                for (unsigned int i = 0; i < lights.size(); i++){
                    double lightTransmissivity = rayMarchLightScatter(x, lights[i]);
                    C += lights[i].getColor(lightTransmissivity) * lightTransmissivity * tVal; //colorVolumes[0].get()->eval(x);
                }
            }
            if(ENABLE_DSM){
                for (unsigned int i = 0; i < lightGrids.size(); i++){
                    double lightTransmissivity = rayMarchDSM(x, lightGrids[i].get());
                    if (lightTransmissivity > 0.0){
                        C += lightGrids[i].get()->sourceLight.getColor(lightTransmissivity) * lightTransmissivity * tVal; //colorVolumes[0].get()->eval(x);
                    }
                    /*if(density > 1.0){
                        std::cout << "----------------------------------\n";
                        std::cout << "transmission: " << lightTransmissivity << "\n";
                        std::cout << "density: " << density << "\n";
                        std::cout << "tVal: " << tVal << "\n";
                    }*/
                  //  std::cout << "transmission: " << lightTransmissivity << "\n";
                  //  std::cout << "C: " << lightGrids[i].get()->sourceLight.getColor(lightTransmissivity) << "\n";
                }
            }
            T *= deltaT;
            if (emissive) {
                //Boosting the emission of the peaks in our noise function- this makes the star clusters appear brighter
                //smokeColor = cSlider.getColor(density);
                Color emitColor = smokeColor.getColor(density) * density;
                //std::cout << color << "Density: " << density << "\n";
                //The field color's contribution will be scaled by the density, so sparse areas don't become super bright
                if (colorVolumes.size()){
                    fieldColor = colorVolumes[0].get()->eval(x);
                    fieldColor.clamp();
                    emitColor += fieldColor;
                }
                C += emitColor * tVal * emissive;
            }
        }
        
        //step 4
        x += n * activeMarchStep;
        marchLen += activeMarchStep;
        increaseMarchCounter += activeMarchStep;
        //Increase march length as we get further away from the camera
        if(increaseMarchCounter > MARCH_INCREASE_DIST){
            activeMarchStep *= MARCH_MULTIPLIER;
            increaseMarchCounter = 0.0;
        }
    }
    //Set alpha equal to the light transmissivity
    C[3] = 1 - T;
    return C;
}

double SceneManager::rayMarchLightScatter(const Vector& x, light l) const {
    double marchLen = 0.0;
    Vector toLight(l.pos - x);
    double dist = toLight.magnitude();
    toLight.normalize();
    double T = 1.0;
    Vector x1(x);

    while (marchLen < dist){

        float density = 0.0;
        for(unsigned int j = 0; j < volumes.size(); j++){
            if(UNION_COLLISIONS)
                density = std::max(density, volumes[j].get()->eval(x));
            else if(ADD_COLLISIONS)
                density += volumes[j].get()->eval(x);
        }
        if(density > 0.0){
            T *= exp(-density * marchStep * K);
        }
        x1 += marchStep * toLight;
        marchLen += lightMarchStep;

    }
    return T;
}

double SceneManager::rayMarchDSM(const Vector& x, const DeepShadowMap* dsm) const {
    float lightInterp = dsm->trilinearInterpolate(x);
    if (lightInterp < 0.0)
        return 0.0;
    return exp(-K * lightInterp);
}

void SceneManager::renderImage(int frameNumber){

    std::mt19937 rng(20);
    std::uniform_real_distribution<> udist(-0.9, 0.9);
    //Generate our random point within the bounding box of our sphere
    boost::timer renderTimer;
    double startTime = omp_get_wtime();
    if (boundingboxes.size() == 0){
        std::cout << "\033[1;31mError: No bounding boxes pushed.\033[0m\n";
        return;
    }
    Image img;
    img.reset(width, height, 4);
    //Fire a ray from eye through every pixel
    int progressMod = width / 10;
    for(int i = 0; i < width; i++){
#pragma omp parallel for
        for(int j = 0; j < height; j++){
            std::vector<Vector> rayVectors;

            //Our first ray should always shoot through the center of the pixel
            double normalizedPixelCoordX = (double)i / (double)width;
            double normalizedPixelCoordY = (double)j / (double)height;
            Vector centerVec = camera.view(normalizedPixelCoordX, normalizedPixelCoordY);
            Ray ray(camera.eye(), centerVec);
            rayVectors.push_back(centerVec);

            //Subsequent rays jitter
            for(int AA = 1; AA < raysPerPixel; AA++){
                double AACoordX = normalizedPixelCoordX + (udist(rng) / (double)width);
                double AACoordY = normalizedPixelCoordY + (udist(rng) / (double)height);
                Vector AAvec = camera.view(AACoordX, AACoordY);
                rayVectors.push_back(AAvec);
            }


            //Get our pixel coordinates in the range of 0 - 1
            float startMarch = 9999999999.0;
            float endMarch = -1.0;

            //Test for intersections with every bounding box in the scene, and then march from the closest to the farthest intersection point
            //This method will not skip gaps between bounding boxes
            for(int k = 0; k < boundingboxes.size(); k++){
                std::vector<float> intersects = boundingboxes[k].intersect(ray, 0, 99999999999.0);
                if(intersects.size() == 2){
                    startMarch = std::min(startMarch, intersects[0]);
                    endMarch = std::max(endMarch, intersects[1]);
                    if (startMarch < 0)
                        startMarch = 0;
                }
            }

            if(endMarch != -1.0){
                Color C;
                for(int AA = 0; AA < raysPerPixel; AA++)
                    C += rayMarch(rayVectors[AA], startMarch, endMarch);
                C /= raysPerPixel;
                //Color C(1.0, 0.0, 0.0, 1.0);
                std::vector<float> colorVector= {static_cast<float>(C[0]), static_cast<float>(C[1]), static_cast<float>(C[2]), static_cast<float>(C[3])};
                img.setPixel( i, j, colorVector);
            }

        }

        //Print our progress to make sure we aren't frozen
        if (i % progressMod == 0){
            std::cout << (i*100/width) << "%\n";
        }
    }

    //Write our image
    std::ostringstream ss;

    //Don't give it number padding if it is just a single render
    std::string filename(outFile);
    filename += ".";
    ss << setfill('0') << setw(4) << frameNumber;
    std::string numPadding = ss.str();
    filename += numPadding;

    filename += ".exr";
    writeOIIOImage(filename.c_str(), img, 1.0, 1.0);
    std::cout << filename << "\n";

    if(WRITE_RENDER_LOG)
        renderlog.writeToFile(filename);

    if(WRITE_RENDER_ANNOTATION){
        std::ostringstream ss1;
        renderlog.addLine("Render Time");
        ss1 << (omp_get_wtime() - startTime);
        renderlog.addLine(ss1.str());
        renderlog.writeToImage(filename);
    }

    renderlog.annotations.clear();
}


std::vector<std::string> SceneManager::getAnnotation(){

    std::vector<std::string> annoVec;
    annoVec.push_back("SCENE");
    annoVec.push_back("");
    annoVec.push_back("K");
    annoVec.push_back(std::to_string(K));
    annoVec.push_back("Emissive");
    annoVec.push_back(std::to_string(emissive));
    annoVec.push_back("March Step");
    annoVec.push_back(std::to_string(marchStep));
    annoVec.push_back("Light March Step");
    annoVec.push_back(std::to_string(lightMarchStep));

    std::ostringstream ss;
    ss << camera.eye() << ",  " << camera.view();
    annoVec.push_back("Camera");
    annoVec.push_back(ss.str());

    return annoVec;
}
