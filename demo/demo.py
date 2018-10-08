import cv2

from stitchup import Stitcher, new_window


s = Stitcher(use_gpu=True,
             estimator_type="homography",
             matcher_type="affine",
             warp_type="plane")

# s.enable_exposure_compensation('gain')
# s.enable_seam_finding('gc_color')

TL = '../imgs/TL.jpg'
TR = '../imgs/TR.jpg'
BL = '../imgs/BL.jpg'
BR = '../imgs/BR.jpg'
sorted_imgs = [cv2.imread(TL), cv2.imread(TR), cv2.imread(BL), cv2.imread(BR)]

ok = s.load_calibration(*sorted_imgs)
assert ok

s.enable_blending('feather', 1)

new_window('panorama', shape=s.panorama_shape)

ok, img = s.stitch_images(*sorted_imgs)
cv2.imshow('panorama', img)
cv2.waitKey(0)

