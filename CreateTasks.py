import sys
from datetime import datetime
import os

#########################
#arg1 - executable script
#arg2 - job name
#arg3 - start frame
#arg4 - end frame
#arg5 - frames per job
#########################
def main(args):

    #first create directory to hold scripts and rendered images
    jobName = args[1];
    currPath = os.path.dirname(os.path.realpath(__file__));
    executablePath = os.path.join(currPath, args[2]);
    outDir = os.path.join(currPath, "output", jobName)
    print outDir
    outFile = os.path.join(outDir, jobName)
    print outDir
    print outFile

    if not os.path.exists(outDir):
        os.mkdir(outDir);


    #now write .sh scripts
    startFrame = int(args[3]);
    endFrame = int(args[4]);
    frameStep = int(args[5]);
    for i in range(startFrame, endFrame, frameStep):
        scriptName = os.path.join(outDir, "caweeks" + datetime.now().strftime('%Y-%m-%d_%H-%M-%S') + jobName + str(i) + "_" + str(i + frameStep - 1));
        pipeJob = open(scriptName, "w+")

        #Add voyager module to dpa_queue path
        exportPath = "export PYTHONPATH=$PYTHONPATH:/DPA/wookie/dpa/projects/caweeks/rnd/VolumeRendering/swig\n "
        s = " ";
        execCommand = exportPath + s.join(["python", executablePath, outFile, str(i), str(i + frameStep)]);

        pipeJob.write(execCommand);
        pipeJob.close();
        os.system("chmod 770 " + scriptName);

        #now push this job into the queue
        os.system("cqsubmittask cheddar " + scriptName);

if __name__ == '__main__':
    main(sys.argv);
