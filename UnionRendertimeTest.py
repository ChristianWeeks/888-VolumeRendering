
import os
import voyager as voy
import time
import sys
import random

def main(args):

    outputName = args[1];
    currPath = os.path.dirname(os.path.realpath(__file__));
    outDir = os.path.join(currPath, "output", outputName)
    outFile = os.path.join(outDir, outputName)

    if not os.path.exists(outDir):
        os.mkdir(outDir);

    startFrame = int(args[2]);
    endFrame = int(args[3]); 

    zeroVector = voy.Vector(0.0, 0.0, 0.0);
    K = 3.0;
    emissive = 0.00;
    marchStep = 0.01;
    lightMarchStep = 0.03;

    gridLength = 4.0;

    gridVoxelCount = 400;
    lightVoxelCount = 100;
    if((gridLength / gridVoxelCount) < marchStep):
        print "Warning: March Step greater than Voxel Length; Voxel: " + str(gridLength / gridVoxelCount) + "; MarchStep: " + str(marchStep);
    if((gridLength / lightVoxelCount) < lightMarchStep):
        print "Warning: Light March Step greater than DSM Voxel Length; Voxel: " + str(gridLength / lightVoxelCount) + "; MarchStep: " + str(lightMarchStep);

    #################################################################################################
    #Setup our scene globals
    #################################################################################################
    scene = voy.SceneManager(outFile);
    scene.setResolution(540);
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


    rLight = voy.light(voy.Color(0.95, 0.8, 0.8, 1.0), voy.Vector(0.0, -1.0, 0.0), voy.Vector(3.0, 3.0, 0.0), 1.0);
    rLight.cSlider.USE_SLIDER = 1;
    ############################################################
    #YELLOW TO PURPLE
    ############################################################
    #rLight.cSlider.addColor(0.0, voy.Color(0.4, 0.1, 1.0, 1.0));
    #rLight.cSlider.addColor(0.8, voy.Color(1.0, 0.3, 0.4, 1.0));
    #rLight.cSlider.addColor(0.9, voy.Color(1.0, 0.9, 0.1, 1.0));
    #rLight.cSlider.addColor(1.0, voy.Color(1.0, 1.0, 1.0, 1.0));

    ############################################################
    #NEON
    ############################################################
    #rLight.cSlider.addColor(0.3, voy.Color(0.0, 0.1, 1.0, 1.0));
    #rLight.cSlider.addColor(0.8, voy.Color(0.1, 1.0, 0.8, 1.0));
    #rLight.cSlider.addColor(0.9, voy.Color(0.1, 1.0, 0.1, 1.0));
    #rLight.cSlider.addColor(1.0, voy.Color(1.0, 1.0, 1.0, 1.0));


    ############################################################
    #SOFT ICY
    ############################################################
    #rLight.cSlider.addColor(0.0, voy.Color(0.2, 0.05, 1.0, 1.0));
    #rLight.cSlider.addColor(0.5, voy.Color(0.3, 0.3, 1.0, 1.0));
    #rLight.cSlider.addColor(0.85, voy.Color(1.0, 0.3, 0.3, 1.0));
    #rLight.cSlider.addColor(1.0, voy.Color(1.0, 1.0, 1.0, 1.0));

    ############################################################
    #FIERY YELLOW RED SMOKE
    ############################################################
    rLight.cSlider.addColor(0.0, voy.Color(0.5, 0.0, 0.0, 1.0));
    rLight.cSlider.addColor(0.5, voy.Color(1.0, 0.0, 0.0, 1.0));
    rLight.cSlider.addColor(0.9, voy.Color(1.0, 1.0, 0.0, 1.0));
    rLight.cSlider.addColor(1.0, voy.Color(1.0, 1.0, 1.0, 1.0));

    print "Building Grid"
    #scene.pushDSM(bDSM);
    #scene.pushLight(bLight);
    offset = 0;
    for i in range(startFrame, endFrame):
        offset = i * 0.01;
        fogOffset = i * 0.001;
        wispDensity = 0.015;
        advectNoise = voy.SimplexNoiseObject(5, 0.5, 2.5, 2.2, 0.2, 2.0, 0);
        advectField = voy.Noisev(advectNoise, 0, 0, 0);

        gridtime = time.time()
        pyroNoise = voy.SimplexNoiseObject(5, 0.5, 1.0, 2.2, -1.4, 1.4, offset);

        sRad = 1.5;
        pyroBound = 3.0;
        pyroExp = 0.5;
        sphere = voy.PyroSphere(sRad, pyroBound, pyroExp, pyroNoise);
        unionSphere = voy.Unionf(sphere, sphere); 
        unionSphere2 = voy.Unionf(unionSphere, sphere);
        unionSphere3= voy.Unionf(unionSphere2, sphere);
        unionSphere4 = voy.Unionf(unionSphere3, sphere);
        unionSphere5 = voy.Unionf(unionSphere4, sphere);

        #unionVol = voy.Unionf(sphere, noiseVolume);
        grid = voy.Gridf(unionSphere5, voy.Vector(0.0, 0.0, 0.0), gridLength, gridVoxelCount);
        gridded = voy.GriddedVolf(grid);

        end = time.time()
        print "Grid Build Time: " + str(end - gridtime);

        dsmtime = time.time();
        rDSM = voy.DSM(rLight, lightMarchStep, gridded, zeroVector, gridLength, lightVoxelCount);
        end = time.time()
        print "DSM Build Time: " + str(end - dsmtime);



        ###############################################
        #Push our volumes and lights
        ###############################################
        scene.pushFloatVolume(gridded);
        scene.pushDSM(rDSM);

        ###############################################
        #Create our annotations
        ###############################################
        scene.renderlog.addVector(scene.getAnnotation());
        scene.renderlog.addLine("Number of Unions");
        scene.renderlog.addLine(0);
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
        scene.clearDSMs();
        scene.clearFloatVolumes();
        scene.renderlog.clearAnnotations();
    print "Rendering Done"
    os.system("it " + outFile + ".exr &"); 


if __name__ == '__main__':
    main(sys.argv);
