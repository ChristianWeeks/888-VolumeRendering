import sys
from datetime import datetime
import os

#########################
#arg1 - job name
#arg2 - start frame
#arg3 - end frame
#arg4 - frames per job
#########################
def main(args):

    #first create directory to hold scripts and rendered images
    jobName = args[1];
    currPath = os.path.dirname(os.path.realpath(__file__));
    executablePath = os.path.join(currPath, "PyroTests.py");
    outDir = os.path.join(currPath, "output", jobName)
    outFile = os.path.join(outDir, jobName)

    if not os.path.exists(outDir):
        os.mkdir(outDir);

    #Add voyager module to dpa_queue path

    #now write .sh scripts
    startFrame = int(args[2]);
    endFrame = int(args[3]);
    frameStep = int(args[4]);
    for i in range(startFrame, endFrame, frameStep):
        scriptName = os.path.join(outDir, "caweeks" + datetime.now().strftime('%Y-%m-%d_%H-%M-%S') + jobName + str(i) + "_" + str(i + frameStep - 1));
        pipeJob = open(scriptName, "w+")

        s = " ";
        execCommand = s.join(["python", executablePath, outDir, str(i), str(i + frameStep)]);

        pipeJob.write(execCommand);
        pipeJob.close();
        os.system("chmod 770 " + scriptName);

        #now push this job into the queue
        os.system("cqsubmittask muenster " + scriptName); 





if __name__ == '__main__':
    main(sys.argv);
