#include "Color.h"
#include <vector>

namespace lux{
class ColorSlider {
    public:
        ColorSlider(){};
        ~ColorSlider(){};

        void addColor(float position, Color color){
            if (anchors.size() == 0){
                anchors.push_back(position);
                return;
            }
            std::array<float, 5> 
            std::vector<float, 5>::iterator it;
            it = anchors.begin();
            //Iterate until the until the new position is greater than an anchor
            while (position < *it){
                    it++;
                    itc++;
            }
            //Insert there
            anchors.insert(it, position);
            colors.insert(itc, color);
            
        };

        Color getColor(float position){
            std::vector<float>::iterator it;
            std::vector<float>::iterator itc;
            it = anchors.begin();
            itc = colors.begin();
            while (position < *it){
                    it++;
                    itc++;
            }
            //If we are at the last element in the array, return that color
            if (it != anchors.end() && next(it) == anchors.end()){
                return *itc;
            }
            return interpolate(*it, *(next(it)), position, *itc, *(next(itc)));
        };

    private:
        std::vector<float> anchors;
        std::vector<Color> colors;

        const Color interpolate(float start, float end, float pos, Color c1, Color c2) {
            float len = end - start;
            pos = (pos - start) / len;
            return c1*(1-pos) + c2*pos;
        }
};
}
