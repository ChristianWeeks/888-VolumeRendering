
import os
import voyager as voy
import time
import sys
import random

def main(args):

    #################################################################################################
    #Set our write path properly
    #################################################################################################
    outputName = args[1];
    currPath = os.path.dirname(os.path.realpath(__file__));
    outDir = os.path.join(currPath, "output", outputName)
    outFile = os.path.join(outDir, outputName)

    if not os.path.exists(outDir):
        os.mkdir(outDir);

    #################################################################################################
    #Set our scene globals
    #################################################################################################
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

    #################################################################################################
    #Assert our march step is less than our voxel length
    #################################################################################################
    assert (gridLength / gridVoxelCount) < marchStep, "Warning: March Step greater than Voxel Length; V: " + str(gridLength / gridVoxelCount) + "; March: " + str(marchStep);
    assert (gridLength / lightVoxelCount) < lightMarchStep, "Warning: Light March Step greater than Voxel Length; V: " + str(gridLength / lightVoxelCount) + "; March: " + str(lightMarchStep);

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
    scene.setFrameRange(startFrame, endFrame);
    scene.WRITE_RENDER_ANNOTATION = 1;
    scene.camera.setEyeViewUp(voy.Vector(0.0, 0.0, 10.0), voy.Vector(0.0, 0.0, -1.0), voy.Vector(0, 1.0, 0.0));

    #################################################################################################
    #Setup our lights
    #################################################################################################
    rLight = voy.light(voy.Color(0.95, 0.8, 0.8, 1.0), voy.Vector(0.0, -1.0, 0.0), voy.Vector(3.0, 3.0, 0.0), 1.0);
    rLight.cSlider.USE_SLIDER = 1;

    #################################################################################################
    #FIERY YELLOW RED SMOKE
    #################################################################################################
    rLight.cSlider.addColor(0.0, voy.Color(0.5, 0.0, 0.0, 1.0));
    rLight.cSlider.addColor(0.5, voy.Color(1.0, 0.0, 0.0, 1.0));
    rLight.cSlider.addColor(0.9, voy.Color(1.0, 1.0, 0.0, 1.0));
    rLight.cSlider.addColor(1.0, voy.Color(1.0, 1.0, 1.0, 1.0));

    print "Building Grid"
    offset = 0;
    for i in range(startFrame, endFrame):

        gridtime = time.time()
        #################################################################################################
        #Setup our Volumes
        #################################################################################################

        end = time.time()
        print "Grid Build Time: " + str(end - gridtime);


        dsmtime = time.time();
        #################################################################################################
        #Setup our Deep Shadow Maps
        #################################################################################################

        end = time.time()
        print "DSM Build Time: " + str(end - dsmtime);

        #################################################################################################
        #Push our volumes and lights
        #################################################################################################

        #################################################################################################
        #Create our annotations
        #################################################################################################
        #scene.renderlog.addVector(scene.getAnnotation());
        #scene.renderlog.addLine("Number of Unions");
        #scene.renderlog.addLine(0);

        print "---------------------Starting Render----------------------"
        t = time.time();
        scene.renderImage(i);

        #################################################################################################
        #Clear for the next iteration
        #################################################################################################
        scene.clearDSMs();
        scene.clearFloatVolumes();
        scene.renderlog.clearAnnotations();

    print "Rendering Done"
    os.system("it " + outFile + ".exr &"); 


if __name__ == '__main__':
    main(sys.argv);