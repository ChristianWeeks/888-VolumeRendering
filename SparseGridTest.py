
import os
import voyager as voy
import time
import sys

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
    emissive = 0.1;
    marchStep = 0.02;
    lightMarchStep = 0.02;

    gridLength = 3.0;

    gridVoxelCount = 400;
    lightVoxelCount = 300;
    if((gridLength / gridVoxelCount) < marchStep):
        print "Warning: March Step greater than Voxel Length; Voxel: " + str(gridLength / gridVoxelCount) + "; MarchStep: " + str(marchStep);

    #################################################################################################
    #Setup our scene globals
    #################################################################################################
    scene = voy.SceneManager(outFile);
    scene.setResolution(1080);
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
    #rLight.cSlider.addColor(0.0, voy.Color(0.4, 0.1, 1.0, 1.0));
    #rLight.cSlider.addColor(0.8, voy.Color(1.0, 0.3, 0.4, 1.0));
    #rLight.cSlider.addColor(0.9, voy.Color(1.0, 0.9, 0.1, 1.0));
    #rLight.cSlider.addColor(1.0, voy.Color(1.0, 1.0, 1.0, 1.0));

    ############################################################
    #NEON
    ############################################################
    rLight.cSlider.addColor(0.0, voy.Color(0.0, 0.1, 1.0, 1.0));
    rLight.cSlider.addColor(0.8, voy.Color(0.1, 1.0, 0.8, 1.0));
    rLight.cSlider.addColor(0.9, voy.Color(0.1, 1.0, 0.1, 1.0));
    rLight.cSlider.addColor(1.0, voy.Color(1.0, 1.0, 1.0, 1.0));


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
    #rLight.cSlider.addColor(0.0, voy.Color(0.5, 0.0, 0.0, 1.0));
    #rLight.cSlider.addColor(0.5, voy.Color(1.0, 0.0, 0.0, 1.0));
    #rLight.cSlider.addColor(0.9, voy.Color(1.0, 1.0, 0.0, 1.0));
    #rLight.cSlider.addColor(1.0, voy.Color(1.0, 1.0, 1.0, 1.0));
    sRad = 1.0;
    pyroExp = 1.2;
    pyroBound = 4.0;
    pyroOffset = 0.0
    xOffset = 0.0;
    yOffset= 0.0;
    zOffset = 0.0;
    advectTime = 0.5;
    print "Building Grid"
    #scene.pushDSM(bDSM);
    #scene.pushLight(bLight);
    for i in range(startFrame, endFrame):

        xOffset = xOffset + i * 0.01;
        yOffset = yOffset + i * 0.01;
        zOffset = zOffset + i * 0.01;
        pyroOffset = i * 0.01;
        pyroNoise = voy.SimplexNoiseObject(5, 0.5, 1.0, 2.2, -0.8, 0.8, pyroOffset);
        advectNoise = voy.SimplexNoiseObject(5, 0.5, 2.5, 2.2, 0.2, 2.0, 0);
        advectField = voy.Noisev(advectNoise, 0, 0, 0);

        gridtime = time.time()
        sphere = voy.Sphere(sRad);
        #advectedSphere = voy.AdvectMMCf(sphere, advectField, 0.5);
        griddedVol = voy.AutoGriddedf(sphere, voy.Vector(0.0, 0.0, 0.0), gridLength, gridVoxelCount,0);
        end = time.time()

        print "Grid Build Time: " + str(end - gridtime);

        #bLight = voy.light(voy.Color(0.7, 0.7, 0.95, 1.0), voy.Vector(0.0, -1.0, 0.0), voy.Vector(-5.0, -2.0, 5.0), 1.0);
        #dsmtime = time.time();
        #rDSM = voy.DSM(rLight, lightMarchStep, griddedVol, zeroVector, gridLength, lightVoxelCount);
        #rDSM.get().sourceLight.cSlider.addColor(1.0, voy.Color(1.0, 0.0, 0.0, 1.0));
        #end = time.time()
        #print "DSM Build Time: " + str(end - dsmtime);

        ###############################################
        #Push our volumes and lights
        ###############################################
        scene.pushFloatVolume(griddedVol);

        ###############################################
        #Create our annotations
        ###############################################

        print "---------------------Starting Render----------------------"
        t = time.time();

        scene.renderlog.addLine("Dense grid WITHOUT hacky reference in 'DensityGrid' constructor");
        scene.renderlog.addLine("");
        scene.renderlog.addLine("Grid voxels");
        scene.renderlog.addLine(str(gridVoxelCount));
        scene.renderlog.addLine("Grid Length");
        scene.renderlog.addLine(str(gridLength));
        scene.renderlog.addLine("Grid Build Time");
        scene.renderlog.addLine(str(end - gridtime));
        scene.renderImage(i);

        ###############################################
        #Clear for the next iteration
        ###############################################
        scene.clearFloatVolumes();
        scene.clearDSMs();
        scene.renderlog.clearAnnotations();
    print "Rendering Done"
    os.system("it " + outFile + ".exr &"); 


if __name__ == '__main__':
    main(sys.argv);