import os.path as op
import re
import glob
import itertools
import functools
import subprocess

import cv2
import numpy as np
import pandas as pd

from imgstore import new_for_filename, new_for_format
from imgstore.util import ensure_color

from stitchup import Stitcher, StoreAligner, new_window


s = Stitcher(use_gpu=True,
             estimator_type="homography",
             matcher_type="affine",
             warp_type="plane")

#s.enable_exposure_compensation('gain')
#s.enable_seam_finding('gc_color')

TL = 'imgs/TL.jpg'
TR = 'imgs/TR.jpg'
BL = 'imgs/BL.jpg'
BR = 'imgs/BR.jpg'
sorted_imgs = [cv2.imread(TL), cv2.imread(TR), cv2.imread(BL), cv2.imread(BR)]

ok = s.load_calibration(*sorted_imgs)
assert ok

s.enable_blending('feather', 1)

new_window('panorama', shape=s.panorama_shape)

ok, img = s.stitch_images(*sorted_imgs)
cv2.imshow('panorama', img)
cv2.waitKey(0)

