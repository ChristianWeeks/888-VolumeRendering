node = hou.pwd()
geo = node.geometry()
import os
import voyager as voy
import time
import sys
import random
import traceback

print "PROCESS STARTING"

def main():
    #################################################################################################
    #Set our write path properly
    #################################################################################################
    outputName =    node.evalParm("outputName")
    currPath =      hou.getenv("HIP");
    outDir =        os.path.join(currPath, "output", outputName)
    outFile =       os.path.join(outDir, outputName)

    if not os.path.exists(outDir):
        os.mkdir(outDir);

    #################################################################################################
    #Setup our scene globals
    #################################################################################################
    scene = voy.SceneManager(outFile);
    scene.marchStep =       node.evalParm("rayMarchStep");
    scene.lightMarchStep =  node.evalParm("lightMarchStep");
    scene.emissive =        node.evalParm("emissive")
    scene.K =               node.evalParm("K")

    startFrame =        node.evalParm("startFrame")
    endFrame =          node.evalParm("endFrame")
    scene.setFrameRange(startFrame, endFrame)

    gridLength =        node.evalParm("gridLength")
    gridVoxelCount =   node.evalParm("gridVoxelCount")
    lightVoxelCount =  node.evalParm("lightVoxelCount")

    scene.setResolution(node.evalParm("resolution"));
    scene.ENABLE_DSM = 1;
    scene.WRITE_RENDER_ANNOTATION = 1;
    scene.camera.setEyeViewUp(voy.Vector(0.0, 0.0, 10.0), voy.Vector(0.0, 0.0, -1.0), voy.Vector(0, 1.0, 0.0));
    #################################################################################################
    #Assert our march step is less than our voxel length        #scene.renderlog.addLine("Number of Unions");
        #scene.renderlog.addLine(0);
    #################################################################################################
    assert (gridLength / gridVoxelCount) > scene.marchStep, "Voxel Length smaller than Ray March Step; V: " + \
        str(gridLength / gridVoxelCount) + "; March: " + str(scene.marchStep);
    assert (gridLength / lightVoxelCount) > scene.lightMarchStep, "Voxel Length smaller than Light March Step; V: " + \
        str(gridLength / lightVoxelCount) + "; March: " + str(scene.lightMarchStep);


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
    points = geo.points()

    print "Building Grid"
    offset = 0;
    for i in range(startFrame, endFrame):
        print "Frame: " + str(i);

        gridtime = time.time()
        #################################################################################################
        #Setup our Volumes
        #################################################################################################
        wispStampValue = 1.0;
        wispRad = 0.5;
        EmptyVol = voy.Constantf(0.0);
        WispGrid = voy.Gridf(EmptyVol);
        density = WispGrid.getRef();

        wispNoise1 = voy.SimplexNoiseObject(4, 1.0, 3.0, 2.2, 1.0, 1.4, 0.0);
        wispNoise2 = voy.SimplexNoiseObject(4, 0.5, 4.0, 2.2, -0.2, 0.2, 0.3);
        for p in points:
            density.StampWisp(wispStampvalue, voy.Vector(p[0], p[1], p[2]), wispNoise1, wispNoise2, 1.0, wispRad, 10000, 0);
        gridded = voy.GriddedVolf(WispGrid);
        end = time.time()
        print "Grid Build Time: " + str(end - gridtime);


        dsmtime = time.time();
        #################################################################################################
        #Setup our Deep Shadow Maps
        #################################################################################################
        #DSM = voy.DSM(rLight, scene.lightMarchStep, gridded, voy.Vector(0,0,0), gridLength, lightVoxelCount);
        end = time.time()
        print "DSM Build Time: " + str(end - dsmtime);

        #################################################################################################
        #Push our volumes and lights
        #################################################################################################
        scene.pushFloatVolume(gridded);
        #scene.pushDSM(DSM);


        #################################################################################################
        #Create our annotations
        #################################################################################################
        scene.renderlog.addVector(scene.getAnnotation());

        print "---------------------Starting Render----------------------"
        #scene.renderImage(i);

        #################################################################################################
        #Clear for the next iteration
        #################################################################################################
        scene.clearDSMs();
        scene.clearFloatVolumes();
        scene.renderlog.clearAnnotations();

    print "Rendering Done"
    #os.system("it " + outFile + ".exr &");

#The RENDER_SWITCH statement prevents this script from running every time it updates.
#It will only render when we manually switch the input on RENDER_SWITCH to 0.
switchValue = `ch("../RENDER_SWITCH/input")`
if switchValue == 0:
    try:
        hou.node("../RENDER_SWITCH").parm("input").set(1);
        main()
    except:
        print "Error Raised: "
        print(traceback.format_exc());
else:
    print "Bypassing render."
