import voyager as voy
import os

K = 1.0;
emissive = 0.05;
marchStep = 0.02;
lightMarchStep = 0.1

startFrame = 0;
endFrame = 1;

bbSize = 3.5;
gridSize = 2 * bbSize;

DSMVoxelCount = 200;
gridVoxelCount = 800;

w = 480;
h = 270;
outputName = "SwigTestRender"
#Setup our scene globals
scene = voy.SceneManager(outputName);
#scene.setResolution(1080);
scene.K = K;
scene.emissive = emissive;
scene.marchStep = marchStep;
scene.lightMarchStep = lightMarchStep;
scene.bbSize = bbSize;
scene.gridSize = gridSize;
scene.DSMVoxelCount = DSMVoxelCount;
scene.gridVoxelCount = gridVoxelCount;
scene.setFrameRange(startFrame, endFrame);
scene.WRITE_RENDER_ANNOTATION = 1;
scene.camera.setEyeViewUp(voy.Vector(0.0, 0.0, 10.0), voy.Vector(0.0, 0.0, -1.0), voy.Vector(0, 1.0, 0.0));

sRad = 2.0;

sphere = voy.Sphere_v(sRad);
light = voy.light(voy.Color(1.0, 0.0, 1.0, 1.0), voy.Vector(0.0, -1.0, 0.0), voy.Vector(0.0, 5.0, 0.0), 1.0);
scene.pushLight(light);
for i in range(startFrame, endFrame):
    scene.renderlog.addLine("GLOBALS");
    scene.renderlog.addLine("");
    scene.renderlog.addLine("Frame");
    scene.renderlog.addLine(str(i));
    scene.renderlog.addLine("Radius");
    scene.renderlog.addLine(str(sRad));
    print "Initializing Volumes: " + str(i)
    scene.pushFloatVolume(sphere);

    print scene.volumes.size()
    print scene.volumes[0];
    for k in dir(scene.volumes[0]):
        print k;

    print "---------------------Starting Render----------------------"
    scene.renderImage(i);
    scene.clearFloatVolumes();

    scene.renderlog.clearAnnotations();
print "Rendering Done"
os.system("it output/" + outputName + ".exr"); 
