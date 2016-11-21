
namespace lux{
class SceneManager{
    public:
        SceneManager();
        ~SceneManager();

        int setFrameRange(int start, int end);
        Color rayMarch(const Camera& cam, Vector n, float start, float end);
        double rayMarchLightScatter(Vector x, light l Volume<float> *vol);

        //These contain everything in our scene
        std::vector<light> lights;
        std::vector<DeepShadowMap> lightGrids;
        std::vector<std::shared_ptr<Volume<float> > > volumes;
        std::vector<std::shared_ptr<Volume<Color> > > colorVolumes;

    private:
        int startFrame;
        int endFrame;

};
}
