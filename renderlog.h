#include <string.h>
#include <fstream>


class RenderLog {
    public:
        RenderLog(std::string fp) : filepath(fp){};
        ~RenderLog(){};
        void addLine(const std::string& s){ body += s + "\n";};
        /*void setFilepath(std::string fp){
            filepath = fp;
        }*/

        //Write our log to file
        void writeToFile(){
            std::ofstream logFile;
            logFile.open(filepath + ".log");
            logFile << body;
            logFile.close();
        };

        std::string getFilepath(){ return filepath;};
        std::string getBody(){ return body;};

    private:
        const std::string filepath;
        std::string body;

};
