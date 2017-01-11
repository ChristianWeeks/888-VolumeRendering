
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

    gridLength = 10.0;

    gridVoxelCount = 500;
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

        colorNoise = voy.SimplexNoiseObject(4, 0.5, 0.4, 2.2, 0.0, 1.0, 0);
        colorNoiseVolume = voy.Noisec(colorNoise, 0.0, 0.0, 0.0);
        scene.pushColorVolume(colorNoiseVolume);

        noise = voy.SimplexNoiseObject(5, 0.5, 0.6, 2.2, -0.01, 0.04, fogOffset);
        noiseVolume = voy.Noisef(noise);

        pyroNoise = voy.SimplexNoiseObject(5, 0.5, 1.0, 2.2, -1.4, 1.4, offset);
        advectNoise = voy.SimplexNoiseObject(5, 0.5, 2.5, 2.2, 0.2, 1.0, 0);
        advectField = voy.Noisev(advectNoise, 0, 0, 0);

        sRad = 1.5;
        pyroBound = 3.0;
        pyroExp = 0.5;
        #sphere = voy.PyroSphere(sRad, pyroBound, pyroExp, pyroNoise);
        #advectedSphere = voy.AdvectMMCf(sphere, advectField, 0.5);
        #unionVol = voy.Unionf(advectedSphere, noiseVolume);
        grid = voy.Gridf(noiseVolume, voy.Vector(0.0, 0.0, 0.0), gridLength, gridVoxelCount);
        griddedNoWisp = voy.GriddedVolf(grid);

        dsmtime = time.time();
        rDSM = voy.DSM(rLight, lightMarchStep, griddedNoWisp, zeroVector, gridLength, lightVoxelCount);
        end = time.time()
        print "DSM Build Time: " + str(end - dsmtime);

        #Stamp our wisp AFTER our DSM is built
        #density = grid.getRef();
        #wispRad = 0.4;
        #wispNoise1 = voy.SimplexNoiseObject(4, 1.0, 3.0, 2.2, -0.5, 0.5, 0.0);
        #wispNoise2 = voy.SimplexNoiseObject(4, 0.5, 4.0, 2.2, -0.2, 0.2, 0.3);
        #for k in range(20):
        #    random.seed(2*k);
        #    r1 = random.uniform(-5, 5)
        #    r2 = random.uniform(-5, 5)
        #    r3 = random.uniform(-5, 5)
        #    density.StampWisp(wispDensity, voy.Vector(r1,r2,r3), wispNoise1, wispNoise2, 1.0, wispRad, 100000, 2.34123*k);

        #griddedWithWisp = voy.GriddedVolf(grid);
        end = time.time()
        print "Grid Build Time: " + str(end - gridtime);



        ###############################################
        #Push our volumes and lights
        ###############################################
        scene.pushFloatVolume(griddedWithWisp);
        scene.pushDSM(rDSM);

        ###############################################
        #Create our annotations
        ###############################################
        scene.renderlog.addVector(scene.getAnnotation());
        scene.renderlog.addLine("Wisp Radius");
        scene.renderlog.addLine(str(wispRad));
        scene.renderlog.addVector(wispNoise1.getAnnotation());
        scene.renderlog.addVector(wispNoise2.getAnnotation());
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
