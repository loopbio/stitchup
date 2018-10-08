#include "simplestitcher.hpp"
#include <stdexcept>

namespace lb {

SimpleStitcher::SimpleStitcher(bool use_gpu,
                               std::string estimator_type, std::string matcher_type, std::string warp_type) :
    use_gpu_(false),
    pano_width_(-1),
    pano_height_(-1),
    stitcher_(cv::Stitcher2::create(estimator_type == "homography" ?
                                        cv::Stitcher2::PANORAMA :
                                        cv::Stitcher2::SCANS, use_gpu)) {

    float match_conf = 0.3;

//#ifdef HAVE_OPENCV_CUDALEGACY
#if 0
    if (use_gpu_)
    {
        stitcher_->setFeaturesFinder(cv::makePtr<cv::detail::SurfFeaturesFinderGpu>());
    }
    else
#endif
    {
        stitcher_->setFeaturesFinder(cv::makePtr<cv::detail::SurfFeaturesFinder>());
    }

    cv::Ptr<cv::detail::Estimator> estimator;
    cv::Ptr<cv::detail::FeaturesMatcher> matcher;
    cv::Ptr<cv::detail::BundleAdjusterBase> adjuster;

    if (matcher_type == "affine") {
        matcher = cv::makePtr<cv::detail::AffineBestOf2NearestMatcher>(false, use_gpu_, match_conf);
    } else if (matcher_type == "homography") {
        matcher = cv::makePtr<cv::detail::BestOf2NearestMatcher>(use_gpu_, match_conf);
    } else {
        throw std::invalid_argument("invalid matcher");
    }

    if (estimator_type == "affine") {
        estimator = cv::makePtr<cv::detail::AffineBasedEstimator>();
        adjuster = cv::makePtr<cv::detail::BundleAdjusterAffinePartial>();
    } else if (estimator_type == "homography") {
        estimator = cv::makePtr<cv::detail::HomographyBasedEstimator>();
        adjuster = cv::makePtr<cv::detail::BundleAdjusterReproj>();
    } else {
        throw std::invalid_argument("invalid estimator");
    }

    stitcher_->setEstimator(estimator);
    stitcher_->setFeaturesMatcher(matcher);
    stitcher_->setBundleAdjuster(adjuster);

    cv::Ptr<cv::WarperCreator> warper_creator;
//#ifdef HAVE_OPENCV_CUDAWARPING
#if 0
    if (use_gpu_ && cv::cuda::getCudaEnabledDeviceCount() > 0)
    {
        if (warp_type == "plane")
            warper_creator = cv::makePtr<cv::PlaneWarperGpu>();
        else if (warp_type == "cylindrical")
            warper_creator = cv::makePtr<cv::CylindricalWarperGpu>();
        else if (warp_type == "spherical")
            warper_creator = cv::makePtr<cv::SphericalWarperGpu>();
    }
    else
#endif
    {
        if (warp_type == "plane")
            warper_creator = cv::makePtr<cv::PlaneWarper>();
        else if (warp_type == "affine")
            warper_creator = cv::makePtr<cv::AffineWarper>();
        else if (warp_type == "cylindrical")
            warper_creator = cv::makePtr<cv::CylindricalWarper>();
        else if (warp_type == "spherical")
            warper_creator = cv::makePtr<cv::SphericalWarper>();
        else if (warp_type == "fisheye")
            warper_creator = cv::makePtr<cv::FisheyeWarper>();
        else if (warp_type == "stereographic")
            warper_creator = cv::makePtr<cv::StereographicWarper>();
        else if (warp_type == "compressedPlaneA2B1")
            warper_creator = cv::makePtr<cv::CompressedRectilinearWarper>(2.0f, 1.0f);
        else if (warp_type == "compressedPlaneA1.5B1")
            warper_creator = cv::makePtr<cv::CompressedRectilinearWarper>(1.5f, 1.0f);
        else if (warp_type == "compressedPlanePortraitA2B1")
            warper_creator = cv::makePtr<cv::CompressedRectilinearPortraitWarper>(2.0f, 1.0f);
        else if (warp_type == "compressedPlanePortraitA1.5B1")
            warper_creator = cv::makePtr<cv::CompressedRectilinearPortraitWarper>(1.5f, 1.0f);
        else if (warp_type == "paniniA2B1")
            warper_creator = cv::makePtr<cv::PaniniWarper>(2.0f, 1.0f);
        else if (warp_type == "paniniA1.5B1")
            warper_creator = cv::makePtr<cv::PaniniWarper>(1.5f, 1.0f);
        else if (warp_type == "paniniPortraitA2B1")
            warper_creator = cv::makePtr<cv::PaniniPortraitWarper>(2.0f, 1.0f);
        else if (warp_type == "paniniPortraitA1.5B1")
            warper_creator = cv::makePtr<cv::PaniniPortraitWarper>(1.5f, 1.0f);
        else if (warp_type == "mercator")
            warper_creator = cv::makePtr<cv::MercatorWarper>();
        else if (warp_type == "transverseMercator")
            warper_creator = cv::makePtr<cv::TransverseMercatorWarper>();
    }

    stitcher_->setWarper(warper_creator);
}


bool SimpleStitcher::estimate_transform(std::vector<cv::Mat> images) {
    cv::Stitcher2::Status stat = stitcher_->estimateTransform(images);
    if (stat == cv::Stitcher2::OK) {
        cv::Mat pano;
        stitcher_->composePanorama(pano);
        pano_width_ = pano.cols;
        pano_height_ = pano.rows;
        return true;
    }
    return false;
}


bool SimpleStitcher::stitch_images(std::vector<cv::Mat> images, cv::Mat &pano) {
    cv::Stitcher2::Status stat = stitcher_->composePanorama(images, pano);
    return stat == cv::Stitcher2::OK;
}

void SimpleStitcher::disable_exposure_compensation(void) {
    stitcher_->setExposureCompensator(cv::makePtr<cv::detail::NoExposureCompensator>());
}


void SimpleStitcher::disable_blending(void) {
    stitcher_->setBlender(cv::detail::Blender::createDefault(cv::detail::Blender::NO, false));
}


void SimpleStitcher::disable_seam_finding(void) {
    stitcher_->setSeamFinder(cv::makePtr<cv::detail::NoSeamFinder>());
}


void SimpleStitcher::enable_seam_finding(std::string seam_find_type, float resol) {
    cv::Ptr<cv::detail::SeamFinder> seam_finder;
    if (seam_find_type == "none")
        seam_finder = cv::makePtr<cv::detail::NoSeamFinder>();
    else if (seam_find_type == "voronoi")
        seam_finder = cv::makePtr<cv::detail::VoronoiSeamFinder>();
    else if (seam_find_type == "gc_color")
    {
//#ifdef HAVE_OPENCV_CUDALEGACY
#if 0
        if (use_gpu_ && cv::cuda::getCudaEnabledDeviceCount() > 0)
            seam_finder = cv::makePtr<cv::detail::GraphCutSeamFinderGpu>(cv::detail::GraphCutSeamFinderBase::COST_COLOR);
        else
#endif
            seam_finder = cv::makePtr<cv::detail::GraphCutSeamFinder>(cv::detail::GraphCutSeamFinderBase::COST_COLOR);
    }
    else if (seam_find_type == "gc_colorgrad")
    {
//#ifdef HAVE_OPENCV_CUDALEGACY
#if 0
        if (use_gpu_ && cv::cuda::getCudaEnabledDeviceCount() > 0)
            seam_finder = cv::makePtr<cv::detail::GraphCutSeamFinderGpu>(cv::detail::GraphCutSeamFinderBase::COST_COLOR_GRAD);
        else
#endif
            seam_finder = cv::makePtr<cv::detail::GraphCutSeamFinder>(cv::detail::GraphCutSeamFinderBase::COST_COLOR_GRAD);
    }
    else if (seam_find_type == "dp_color")
        seam_finder = cv::makePtr<cv::detail::DpSeamFinder>(cv::detail::DpSeamFinder::COLOR);
    else if (seam_find_type == "dp_colorgrad")
        seam_finder = cv::makePtr<cv::detail::DpSeamFinder>(cv::detail::DpSeamFinder::COLOR_GRAD);

    if (!seam_finder)
        throw std::invalid_argument("invalid seam finding mode");

    stitcher_->setSeamFinder(seam_finder);
    if (resol > 0)
        stitcher_->setSeamEstimationResol(resol);
}


void SimpleStitcher::enable_exposure_compensation(std::string mode) {

    int expos_comp_type = 0;
    if (mode == "none")
        expos_comp_type = cv::detail::ExposureCompensator::NO;
    else if (mode == "gain")
        expos_comp_type = cv::detail::ExposureCompensator::GAIN;
    else if (mode == "gain_blocks")
        expos_comp_type = cv::detail::ExposureCompensator::GAIN_BLOCKS;
    else
        throw std::invalid_argument("invalid exposure compensation mode");

    cv::Ptr<cv::detail::ExposureCompensator> compensator = cv::detail::ExposureCompensator::createDefault(expos_comp_type);
    stitcher_->setExposureCompensator(compensator);
}



void SimpleStitcher::enable_blending(std::string mode, float blend_strength) {

    int blend_type = 0;
    if (mode == "none") {
        blend_type = cv::detail::Blender::NO;
        LOGLN("No blending");
    }
    else if (mode == "feather")
        blend_type = cv::detail::Blender::FEATHER;
    else if (mode == "multiband")
        blend_type = cv::detail::Blender::MULTI_BAND;
    else
        throw std::invalid_argument("invalid blend mode");

    if (pano_height_ <= 0)
        throw std::runtime_error("calibration/blending config must be loaded first");

    cv::Ptr<cv::detail::Blender> blender = cv::detail::Blender::createDefault(blend_type, use_gpu_);
    float blend_width = sqrt((pano_height_ * pano_width_)) * blend_strength / 100.f;

    if (blend_width < 1.f) {
        blender = cv::detail::Blender::createDefault(cv::detail::Blender::NO, use_gpu_);
        LOGLN("No blending, insufficient blend width: " << blend_width);
    }
    else if (blend_type == cv::detail::Blender::MULTI_BAND)
    {
        cv::detail::MultiBandBlender* mb = dynamic_cast<cv::detail::MultiBandBlender*>(blender.get());
        mb->setNumBands(static_cast<int>(ceil(log(blend_width)/log(2.)) - 1.));
        LOGLN("Multi-band blender, number of bands: " << mb->numBands());
    }
    else if (blend_type == cv::detail::Blender::FEATHER)
    {
        cv::detail::FeatherBlender* fb = dynamic_cast<cv::detail::FeatherBlender*>(blender.get());
        fb->setSharpness(1.f/blend_width);
        LOGLN("Feather blender, sharpness: " << fb->sharpness());
    }
    stitcher_->setBlender(blender);
}

}
