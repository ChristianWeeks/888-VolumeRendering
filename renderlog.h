#include <string.h>
#include <map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <Magick++.h>


class RenderLog {
    public:
        RenderLog(){};
        ~RenderLog(){};
        void addLine(const std::string& s){ annotations.push_back(s);};
        void addVector(const std::vector<std::string>& annoVec){
            // [] operator automatically inserts the element if it is not found
            annotations.insert(annotations.end(), annoVec.begin(), annoVec.end());
        }

        void writeToImage(std::string filename){
            //Format our map
            std::ostringstream ss;
            for( auto it = annotations.begin(); it != annotations.end(); it+=2){
                if( *(it + 1) == ""){
                    ss << setfill(' ') << setw(10) << "\n" << *it << "\n";
                }
                else{
                    ss << setfill(' ') << setw(10) << *it << setfill('.')  << setw(18) << *(it+1) << "\n";
                }
            }

            //Open and write to our image
            Magick::Geometry textBox(200, 200);
            Magick::GravityType gravity(Magick::NorthWestGravity);
            Magick::Image image;
            image.backgroundColor("#000000");
            image.boxColor("#000000");
            image.fillColor("#999999");
            image.read(filename);
            image.fontPointsize(12);
            image.font("courier");
            image.annotate(ss.str(), gravity);
            image.write(filename);
        };

        //Write our log to file
        void writeToFile(std::string filename){
            //Format our map
            std::ostringstream ss;
            for( auto it = annotations.begin(); it != annotations.end(); it+=2){
                if( *(it + 1) == ""){
                    ss << setfill(' ') << setw(10) << "\n" << *it << "\n";
                }
                else{
                    ss << setfill(' ') << setw(10) << *it << setfill('.')  << setw(18) << *(it+1) << "\n";
                }
            }

            //Open and write to our file
            std::ofstream logFile;
            logFile.open(filename + ".log");
            logFile << ss.str();
            logFile.close();
        };
        const std::string filepath;


    std::vector<std::string> annotations;

};
