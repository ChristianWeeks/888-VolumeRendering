import voyager as voy
import time
import os

zeroVector = voy.Vector(0.0, 0.0, 0.0);
K = 3.0;
emissive = 0.00;
marchStep = 0.02;
lightMarchStep = 0.02;

startFrame = 1;
endFrame = 2; 

gridLength = 15.0;

gridVoxelCount = 700;
lightVoxelCount = 200;

w = 480;
h = 270;

outputName = "LightSliderTest"
#Setup our scene globals
scene = voy.SceneManager(outputName);
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

rLight = voy.light(voy.Color(0.95, 0.8, 0.8, 1.0), voy.Vector(0.0, -1.0, 0.0), voy.Vector(-1.0, 0.0, 5.0), 1.0);
rLight.cSlider = voy.ColorSlider();
rLight.cSlider.USE_SLIDER = 1;
rLight.cSlider.addColor(0.0, voy.Color(0.5, 0.3, 1.0, 1.0));
rLight.cSlider.addColor(1.0, voy.Color(1.0, 0.75, 0.9, 1.0));
mainBB = voy.BoundingBox(voy.Vector(0.0, 0.0, 0.0), gridLength);
scene.boundingboxes.push_back(mainBB);
sRad = 19.0;
pyroExp = 0.3;
pyroBound = 3.0;
pyroOffset = 0.0
print "Building Grid"
#scene.pushDSM(bDSM);
#scene.pushLight(bLight);
for i in range(startFrame, endFrame):

    pyroOffset += 0.01;
    pyroNoise = voy.SimplexNoiseObject(5, 0.5, 2.0, 2.2, -4.0, 4.0, pyroOffset);

    sphere = voy.PyroSphere(sRad, pyroBound, pyroExp, pyroNoise);
    tSphere = voy.Translatef(sphere, voy.Vector(5, -20, 0));
    tSphere2 = voy.Translatef(sphere, voy.Vector(-5, 20, 0));
    finalDensity = voy.Unionf(tSphere, tSphere2);

    grid = voy.Gridf(tSphere, voy.Vector(0.0, 0.0, 0.0), gridLength, gridVoxelCount);
    #griddedVol = voy.GriddedVolf(grid);

    #bLight = voy.light(voy.Color(0.7, 0.7, 0.95, 1.0), voy.Vector(0.0, -1.0, 0.0), voy.Vector(-5.0, -2.0, 5.0), 1.0);
    dsmtime = timeit.Timer();
    rDSM = voy.DSM(rLight, lightMarchStep, griddedVol, zeroVector, gridLength, lightVoxelCount);
    #bDSM = voy.DSM(bLight, lightMarchStep, griddedVol, zeroVector, gridLength, lightVoxelCount);
    end = timeit.Timer()
    print str(end - dsmtime);

    scene.pushDSM(rDSM);
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

    ###############################################
    #Push our volumes
    ###############################################
    print "Initializing Volumes: " + str(i)
    scene.pushFloatVolume(griddedVol);

    print "---------------------Starting Render----------------------"
    t = timeit.Timer();
    scene.renderImage(i);
    #scene.renderlog.addLine("Render Time");
    #scene.renderlog.addLine(str(t.timeit()));
    ###############################################
    #Clear for the next iteration
    ###############################################
    scene.clearFloatVolumes();
    scene.clearDSMs();

    scene.renderlog.clearAnnotations();
print "Rendering Done"
os.system("it output/" + outputName + ".exr &"); 
