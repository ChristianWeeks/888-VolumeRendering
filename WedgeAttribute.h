#include <vector>

class WedgeAttribute {
    public:
        WedgeAttribute() : keyFrames(0), keyValues(0){};
        ~WedgeAttribute(){};
    void addKeyFrame(int frame, float value){
        if(keyFrames.size() == 0){
            keyFrames.push_back(frame);
            keyValues.push_back(value);
            return;
        }
        std::vector<int>::iterator fit;
        std::vector<float>::iterator vit;
        fit = keyFrames.begin();
        vit = keyValues.begin();
        while (frame > *fit && fit != keyFrames.end()){
            fit++;
            vit++;
        }
        keyFrames.insert(fit, frame);
        keyValues.insert(vit, value);
        //for (int i = 0; i < keyFrames.size(); i++){
        //}
        return;
    }

    float getValueAtFrame(int frame){
        std::vector<int>::iterator fit;
        std::vector<float>::iterator vit;
        fit = keyFrames.begin();
        vit = keyValues.begin();

        // If the queried frame is before our first keyframe, return value at first keyframe
        if (frame < *fit) return *vit;

        while (frame > *fit){
            //Test if it points at the last element
            if((fit != keyFrames.end()) && (next(fit) == keyFrames.end())) return *vit;

            //If it doesn't, iterate
            fit++;
            vit++;
        }


        //Interpolate between our keyframes
        int endFrame = *fit;
        int startFrame = *(fit - 1);
        float endValue = *vit;
        float startValue =  *(vit - 1);

        return startValue + ((frame - startFrame) * ((endValue - startValue) / (endFrame - startFrame)));
    }

    private:
        std::vector<int> keyFrames;
        std::vector<float> keyValues;
};
