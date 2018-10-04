#include "opencv2/core.hpp"

#include "stitcher2.hpp"

#include <string>

namespace lb {

class SimpleStitcher
{
public:
    SimpleStitcher(bool use_gpu=false,
                   std::string estimator_type="homography",
                   std::string matcher_type="homography",
                   std::string warp_type="spherical");
    bool estimate_transform(std::vector<cv::Mat> images);
    bool stitch_images(std::vector<cv::Mat> images, cv::Mat &pano);
    int get_panorama_width() { return pano_width_; }
    int get_panorama_height() { return pano_height_; }
    void disable_wave_correction() { stitcher_->setWaveCorrection(false); }
    void disable_exposure_compensation();
    void enable_exposure_compensation(std::string mode);
    void disable_seam_finding();
    void enable_seam_finding(std::string seam_find_type, float resol=-1.);
    void disable_blending();
    void enable_blending(std::string mode, float blend_strength);
private:
    bool use_gpu_;
    int pano_width_;
    int pano_height_;
    cv::Ptr<cv::Stitcher2> stitcher_;
};

}
