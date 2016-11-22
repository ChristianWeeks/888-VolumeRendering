#include <string.h>
#include <map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <Magick++.h>


class RenderLog {
    public:
        RenderLog(std::string fp) : filepath(fp){};
        ~RenderLog(){};
        void addLine(const std::string& s){ body += s + "\n";};
        /*void setFilepath(std::string fp){
            filepath = fp;
        }*/
        void addMap(const std::map<std::string, float>& annoMap){
            // [] operator automatically inserts the element if it is not found
            for( auto it = annoMap.begin(); it != annoMap.end(); ++it)
                annotations[it->first] += it->second;
        }

        void writeToImage(){
            //Format our map
            std::ostringstream ss;
            for( auto it = annotations.begin(); it!= annotations.end(); ++it){
                ss << setfill(' ') << setw(10) << it->first << setfill('.')  << setw(12) << it->second << "\n";
            }

            //Open and write to our image
            Magick::Geometry textBox(200, 200);
            Magick::GravityType gravity(Magick::NorthWestGravity);
            Magick::Image image;
            image.backgroundColor("#000000");
            image.boxColor("#000000");
            image.fillColor("#999999");
            image.read(filepath + ".exr");
            image.fontPointsize(12);
            image.font("courier");
            image.annotate(ss.str(), gravity);
            image.write(filepath + ".exr");
        };

        //Write our log to file
        void writeToFile(){
            //Format our map
            std::ostringstream ss;
            for( auto it = annotations.begin(); it!= annotations.end(); ++it){
                ss << setfill(' ') << setw(10) << it->first << setfill('.')  << setw(12) << it->second << "\n";
            }

            //Open and write to our file
            std::ofstream logFile;
            logFile.open(filepath + ".log");
            logFile << ss.str();
            logFile.close();
        };

        std::string getFilepath(){ return filepath;};
        std::string getBody(){ return body;};

    private:
        const std::string filepath;
        std::map<std::string, float> annotations;
        std::string body;

};
