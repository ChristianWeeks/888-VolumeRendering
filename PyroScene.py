import voyager as voy
import time
import os

def main():
    zeroVector = voy.Vector(0.0, 0.0, 0.0);
    K = 3.0;
    emissive = 0.00;
    marchStep = 0.02;
    lightMarchStep = 0.02;

    startFrame = 1;
    endFrame = 2; 

    gridLength = 6.0;

    gridVoxelCount = 600;
    lightVoxelCount = 100;

    outputName = "LightSliderTest"
    #################################################################################################
    #Setup our scene globals
    #################################################################################################
    scene = voy.SceneManager(outputName);
    #scene.setResolution();
    scene.K = K;
    scene.emissive = emissive;
    scene.marchStep = marchStep;
    scene.lightMarchStep = lightMarchStep;
    scene.ENABLE_DSM = 1;

    scene.gridSize = gridLength;
    scene.gridVoxelCount = gridVoxelCount;
    scene.setFrameRange(startFrame, endFrame);
    scene.WRITE_RENDER_ANNOTATION = 1;
    scene.camera.setEyeViewUp(voy.Vector(0.0, 0.0, 10.0), voy.Vector(0.0, 0.0, -1.0), voy.Vector(0, 1.0, 0.0));
    #################################################################################################


    rLight = voy.light(voy.Color(0.95, 0.8, 0.8, 1.0), voy.Vector(0.0, -1.0, 0.0), voy.Vector(4.0, 4.0, 4.0), 1.0);
    rLight.cSlider.USE_SLIDER = 1;
    ############################################################
    #YELLOW TO PURPLE
    ############################################################
    rLight.cSlider.addColor(0.0, voy.Color(0.4, 0.1, 1.0, 1.0));
    rLight.cSlider.addColor(0.8, voy.Color(1.0, 0.3, 0.4, 1.0));
    rLight.cSlider.addColor(0.9, voy.Color(1.0, 0.9, 0.1, 1.0));
    rLight.cSlider.addColor(1.0, voy.Color(1.0, 1.0, 1.0, 1.0));

    #rLight.cSlider.addColor(0.0, voy.Color(0.1, 0.3, 1.0, 1.0));
    #rLight.cSlider.addColor(0.8, voy.Color(1.0, 0.3, 0.4, 1.0));
    #rLight.cSlider.addColor(0.9, voy.Color(1.0, 0.9, 0.1, 1.0));
    #rLight.cSlider.addColor(1.0, voy.Color(1.0, 0.1, 0.4, 1.0));
    mainBB = voy.BoundingBox(voy.Vector(0.0, 0.0, 0.0), gridLength);
    #scene.boundingboxes.push_back(mainBB);

    #################
    #BACKGROUND
    #################


    backgroundBB = voy.BoundingBox(voy.Vector(0.0, 0.0, -15), 10.0);
    scene.boundingboxes.push_back(backgroundBB);
    sRad = 2.0;
    pyroExp = 0.2;
    pyroBound = 3.0;
    pyroOffset = 0.0
    print "Building Grid"
    #scene.pushDSM(bDSM);
    #scene.pushLight(bLight);
    for i in range(startFrame, endFrame):

        gridtime = time.time()
        pyroOffset += 0.01;
        pyroNoise = voy.SimplexNoiseObject(5, 0.5, 1.0, 2.2, -0.8, 0.8, pyroOffset);

        bgsphere = voy.PyroSphere(15.0, pyroBound, pyroExp, pyroNoise);
        tBgSphere = voy.Translatef(bgsphere, voy.Vector(0.0, 0.0, -25));
        backgroundGrid = voy.Gridf(tBgSphere, voy.Vector(0.0, 0.0, -15), 10.0, 300);
        griddedBg = voy.GriddedVolf(backgroundGrid);

        #sphere = voy.PyroSphere(sRad, pyroBound, pyroExp, pyroNoise);
        #grid = voy.Gridf(sphere, voy.Vector(0.0, 0.0, 0.0), gridLength, gridVoxelCount);
        #griddedVol = voy.GriddedVolf(grid);
        end = time.time();
        print "Grid Build Time: " + str(end - gridtime);

        #bLight = voy.light(voy.Color(0.7, 0.7, 0.95, 1.0), voy.Vector(0.0, -1.0, 0.0), voy.Vector(-5.0, -2.0, 5.0), 1.0);
        dsmtime = time.time();
        rDSM = voy.DSM(rLight, lightMarchStep, griddedBg, zeroVector, gridLength, lightVoxelCount);
        #rDSM.get().sourceLight.cSlider.addColor(0.0, voy.Color(0.0, 0.0, 1.0, 1.0));
        #rDSM.get().sourceLight.cSlider.addColor(1.0, voy.Color(1.0, 0.0, 0.0, 1.0));
        #bDSM = voy.DSM(bLight, lightMarchStep, griddedVol, zeroVector, gridLength, lightVoxelCount);
        end = time.time()
        print "DSM Build Time: " + str(end - dsmtime);

        ###############################################
        #Push our volumes and lights
        ###############################################
        scene.pushDSM(rDSM);
        scene.pushFloatVolume(griddedBg);
        #scene.pushFloatVolume(griddedVol);

        ###############################################
        #Create our annotations
        ###############################################
        #scene.renderlog.addVector(scene.getAnnotation());
        #scene.renderlog.addLine("PYRONOISE");
        #scene.renderlog.addLine("");
        #scene.renderlog.addLine("Exp");
        #scene.renderlog.addLine(str(pyroExp));
        #scene.renderlog.addLine("dBound");
        #scene.renderlog.addLine(str(pyroBound));
        #scene.renderlog.addVector(pyroNoise.getAnnotation());
        #scene.renderlog.addLine("GLOBALS");
        #scene.renderlog.addLine("");
        #scene.renderlog.addLine("Frame");
        #scene.renderlog.addLine(str(i));
        #scene.renderlog.addLine("Grid Length");
        #scene.renderlog.addLine(str(gridLength));
        #scene.renderlog.addLine("Radius");
        #scene.renderlog.addLine(str(sRad));


        print "---------------------Starting Render----------------------"
        t = time.time();
        scene.renderImage(i);
        #scene.renderlog.addLine("Render Time");
        #scene.renderlog.addLine(str(t.time()));
        ###############################################
        #Clear for the next iteration
        ###############################################
        scene.clearFloatVolumes();
        scene.clearDSMs();

        scene.renderlog.clearAnnotations();
    print "Rendering Done"
    os.system("it output/" + outputName + ".exr &"); 


if __name__ == '__main__':
    main();
