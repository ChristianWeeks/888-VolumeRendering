
import os
os.system("export PYTHONPATH=$PYTHONPATH:/DPA/wookie/dpa/projects/caweeks/rnd/VolumeRendering/swig");
import voyager as voy
import time
import sys

def main(args):

    outputName = args[1];
    startFrame = int(args[2]);
    endFrame = int(args[3]); 

    zeroVector = voy.Vector(0.0, 0.0, 0.0);
    K = 3.0;
    emissive = 0.00;
    marchStep = 0.5;
    lightMarchStep = 0.02;


    gridLength = 6.0;

    gridVoxelCount = 50;
    lightVoxelCount = 50;
    if((gridLength / gridVoxelCount) < marchStep):
        print "Warning: March Step greater than Voxel Length; Voxel: " + str(gridLength / gridVoxelCount) + "; MarchStep: " + str(marchStep);

    #################################################################################################
    #Setup our scene globals
    #################################################################################################
    scene = voy.SceneManager(outputName);
    #scene.setResolution(1080);
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

    #rLight.cSlider.addColor(0.0, voy.Color(0.2, 0.05, 1.0, 1.0));
    #rLight.cSlider.addColor(0.5, voy.Color(0.3, 0.3, 1.0, 1.0));
    #rLight.cSlider.addColor(0.85, voy.Color(1.0, 0.3, 0.3, 1.0));
    #rLight.cSlider.addColor(1.0, voy.Color(1.0, 1.0, 1.0, 1.0));
    rLight.cSlider.addColor(0.0, voy.Color(0.5, 0.0, 0.0, 1.0));
    rLight.cSlider.addColor(0.5, voy.Color(1.0, 0.0, 0.0, 1.0));
    rLight.cSlider.addColor(0.9, voy.Color(1.0, 1.0, 0.0, 1.0));
    rLight.cSlider.addColor(1.0, voy.Color(1.0, 0.0, 1.0, 1.0));
    sphere1Position = voy.Vector(-3, -0.7, 6.5);
    grid1Position = voy.Vector(-2.3, 0, 7);
    grid1Length = 5.0;

    sphere2Position = voy.Vector(4, 1, -2);
    grid2Position = voy.Vector(4, 1, -2);
    grid2Length = 5.0;

    sphere3Position = voy.Vector(1.5,-1, -5);
    grid3Position = voy.Vector(1.5, -1, -5);
    grid3Length = 2.3;


    sRad = 2.0;
    rad2 = 1.3;
    rad3 = 0.5;
    pyroExp = 0.2;
    pyroBound = 3.0;
    pyroOffset = 0.0
    print "Building Grid"
    #scene.pushDSM(bDSM);
    #scene.pushLight(bLight);
    for i in range(startFrame, endFrame):

        pyroOffset = i * 0.0005;
        pyroOffset2 = i * 0.0005 + 10;
        pyroOffset3 = i * 0.0005 + 20;
        pyroNoise = voy.SimplexNoiseObject(7, 0.5, 1.0, 2.0, -0.8, 0.8, pyroOffset);
        pyroNoise2 = voy.SimplexNoiseObject(5, 0.5, 1.0, 2.2, -0.8, 0.8, pyroOffset2);
        pyroNoise3 = voy.SimplexNoiseObject(4, 0.5, 0.4, 2.2, -0.15, 0.15, pyroOffset3);
        #advectNoise = voy.SimplexNoiseObject(5, 0.5, 2.5, 2.2, 0.5, 1.0, 0);
        #advectField = voy.Noisev(advectNoise, -0.2, 0.0, 0.2);

        gridtime = time.time()
        sphere = voy.PyroSphere(sRad, pyroBound, pyroExp, pyroNoise);
        sphere2 = voy.PyroSphere(rad2, pyroBound, pyroExp, pyroNoise2);
        sphere3 = voy.PyroSphere(rad3, pyroBound, pyroExp, pyroNoise3);
        #advectedSphere = voy.AdvectMMCf(sphere, advectField, 0.2);
        translateSphere = voy.Translatef(sphere, sphere1Position);
        translateSphere2 = voy.Translatef(sphere2, sphere2Position);
        translateSphere3 = voy.Translatef(sphere3, sphere3Position);

        griddedVol = voy.AutoGriddedf(translateSphere, grid1Position, grid1Length, gridVoxelCount);
        #griddedVol2 = voy.AutoGriddedf(translateSphere2, grid2Position, grid2Length, gridVoxelCount);
        #griddedVol3 = voy.AutoGriddedf(translateSphere3, grid3Position, grid3Length, gridVoxelCount);

        #unionGrids = voy.Unionf(translateSphere,translateSphere2);
        #unionGrids2 = voy.Unionf(unionGrids,translateSphere3);
        end = time.time();
        print "Grid Build Time: " + str(end - gridtime);

        #bLight = voy.light(voy.Color(0.7, 0.7, 0.95, 1.0), voy.Vector(0.0, -1.0, 0.0), voy.Vector(-5.0, -2.0, 5.0), 1.0);
        dsmtime = time.time();
        #rDSM = voy.DSM(rLight, lightMarchStep, unionGrids2, voy.Vector(0,0,2), 17, 150);
        #rDSM.get().sourceLight.cSlider.addColor(0.0, voy.Color(0.0, 0.0, 1.0, 1.0));
        #rDSM.get().sourceLight.cSlider.addColor(1.0, voy.Color(1.0, 0.0, 0.0, 1.0));
        #bDSM = voy.DSM(bLight, lightMarchStep, griddedVol, zeroVector, gridLength, lightVoxelCount);
        end = time.time()
        print "DSM Build Time: " + str(end - dsmtime);

        ###############################################
        #Push our volumes and lights
        ###############################################
        #scene.pushDSM(rDSM);
        scene.pushFloatVolume(griddedVol);
        #scene.pushFloatVolume(griddedVol2);
        #scene.pushFloatVolume(griddedVol3);

        ###############################################
        #Create our annotations
        ###############################################
        scene.renderlog.addVector(scene.getAnnotation());
        #scene.renderlog.addLine("PYRONOISE");
        #scene.renderlog.addLine("");
        #scene.renderlog.addLine("Exp");
        #scene.renderlog.addLine(str(pyroExp));
        #scene.renderlog.addLine("dBound");
        #scene.renderlog.addLine(str(pyroBound));
        scene.renderlog.addVector(pyroNoise.getAnnotation());
        #scene.renderlog.addLine("GLOBALS");
        #scene.renderlog.addLine("");
        #scene.renderlog.addLine("Frame");
        #scene.renderlog.addLine(str(i));
        #scene.renderlog.addLine("Grid Length");
        #scene.renderlog.addLine(str(gridLength));
        #scene.renderlog.addLine("Radius");
        #scene.renderlog.addLine(str(sRad));
        scene.renderlog.addLine("Light March Step");
        scene.renderlog.addLine(str(lightMarchStep));
        scene.renderlog.addLine("Light Grid Voxels");
        scene.renderlog.addLine(str(lightVoxelCount));


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
    os.system("it " + outputName + ".exr &"); 


if __name__ == '__main__':
    main(sys.argv);
