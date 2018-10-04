from libcpp cimport bool
from libcpp.vector cimport vector
from libcpp.string cimport string

from cvt cimport *

cdef extern from "src/simplestitcher.hpp" namespace "lb":
    cdef cppclass SimpleStitcher:
        SimpleStitcher(bool, string, string, string) except +
        bool estimate_transform(vector[Mat])
        bool stitch_images(vector[Mat], Mat)
        int get_panorama_width()
        int get_panorama_height()
        void disable_wave_correction()
        void disable_exposure_compensation()
        void disable_blending()
        void disable_seam_finding()
        void enable_blending(string mode, float blend_strength) except +
        void enable_exposure_compensation(string mode) except +
        void enable_seam_finding(string seam_find_type, float resol) except +


cdef class Stitcher:
    cdef SimpleStitcher *me

    def __cinit__(self, bool use_gpu=False,
                  string estimator_type="homography",
                  string matcher_type="homography",
                  string warp_type="spherical"):
        self.me = new SimpleStitcher(use_gpu, estimator_type, matcher_type, warp_type)
        self.me.disable_exposure_compensation()
        self.me.disable_wave_correction()
        self.me.disable_blending()
        self.me.disable_seam_finding()

    def __dealloc__(self):
        del self.me

    def enable_exposure_compensation(self, string mode):
        self.me.enable_exposure_compensation(mode)

    def enable_seam_finding(self, string seam_find_type, float resol=-1):
        self.me.enable_seam_finding(seam_find_type, resol)

    def enable_blending(self, string mode, float blend_strength):
        self.me.enable_blending(mode, blend_strength)

    def load_calibration(self, *images):
        cdef vector[Mat] imgs

        for i in images:
            if not ((i.shape[-1] == 3) & (i.ndim == 3)):
                raise ValueError('images must be color')
            imgs.push_back(nparray2cvmat(i))

        return self.me.estimate_transform(imgs)

    def stitch_images(self, *images):
        cdef bool ok
        cdef Mat out
        cdef vector[Mat] imgs

        for i in images:
            if not ((i.shape[-1] == 3) & (i.ndim == 3)):
                raise ValueError('images must be color')
            imgs.push_back(nparray2cvmat(i))

        ok = self.me.stitch_images(imgs, out)
        if ok:
            ret = cvmat2nparray(out)
        else:
            ret = None

        return ok, ret

    @property
    def panorama_shape(self):
        return self.me.get_panorama_height(), self.me.get_panorama_width(), 3

