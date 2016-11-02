#include "Color.h"
#include <vector>

namespace lux{
class ColorSlider {
    public:
        ColorSlider() : anchors(0), colors(0){};
        ~ColorSlider(){};

        void addColor(const float& position, const Color& color){
            double cArray [] = {color[0], color[1], color[2], color[3]};
            if (anchors.size() == 0){
                anchors.push_back(position);
                colors.push_back(color[0]);
                colors.push_back(color[1]);
                colors.push_back(color[2]);
                colors.push_back(color[3]);
                return;
            }

            std::vector<float>::iterator fit;
            std::vector<double>::iterator cit;
            fit = anchors.begin();
            cit = colors.begin();
            //Iterate until the until the new position is greater than an anchor
            while (position > *fit && fit != anchors.end()){
                    fit++;
                    cit += 4;
            }
            //Insert there
            anchors.insert(fit, position);
            colors.insert(cit, cArray, cArray + 4);

        };

        Color getColor(float position){
            std::vector<float>::iterator fit;
            std::vector<double>::iterator cit;
            fit = anchors.begin();
            cit = colors.begin();

            if(position < *fit){
                Color firstColor(*cit, *(cit + 1), *(cit + 2), *(cit + 3));
                return firstColor;
            }

            while (position > *fit){

                //If we are at the last element in the array, return that color
                if (fit != anchors.end() && next(fit) == anchors.end()){
                    Color lastColor(*cit, *(cit + 1), *(cit + 2), *(cit + 3));
                    return lastColor;
                }

                fit++;
                cit += 4;
            }
            float endAnchor = *fit;
            float startAnchor = *(fit - 1);
            Color endColor(*cit, *(cit + 1), *(cit + 2), *(cit + 3));
            Color startColor(*(cit - 4), *(cit - 3), *(cit - 2), *(cit - 1));
            return interpolate(startAnchor, endAnchor, position, startColor, endColor);
        };

    private:
        std::vector<float> anchors;
        std::vector<double> colors;

        const Color interpolate(float start, float end, float pos, Color c1, Color c2) {
            float len = end - start;
            pos = (pos - start) / len;
            return c1*(1-pos) + c2*pos;
        }
};
}
