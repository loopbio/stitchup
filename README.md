# Stitchup

Stitchup is a python library for stitching multiple videos together to form a larger panorama / mosaic. It is designed for use with Loopbio [motif video recording systems](http://loopbio.com/recording/) and [imgstore](https://github.com/loopbio/imgstore) videos, but can be used with any video or image source.

![stitched panorama image](intro.png?raw=true "Example Result")

It is based on a modified version of the OpenCV [stitcher](https://docs.opencv.org/3.4.3/d2/d8d/classcv_1_1Stitcher.html) machinery, but with several enhancements

 * Python binding
 * Can use different transforms for matching and homography estimation
 * Logging by default
 * Calibrations can be saved and re-loaded
 * Defaults more suited to [motif](http://loopbio.com/recording/) camera array installations (mostly planar cameras)
   * seam carving/estimation disabled
   * multi-band blending disabled (introduces blurring)
   * minimal/no exposure compensation

##### Installation

 * `conda env create -f environment.yaml`
 * `conda activate stitchup`
 * `python demo.py`

##### Notes

* Images/videos should be undistorted before stitching.
* Alignment should be done using images with overlapping features (see `demo.py`)
* When developing (working on c++ at least)
  * `pip uninstall stitchup`
  * ... edit code ...
  * `make cython-inplace`
  * ... test ...

