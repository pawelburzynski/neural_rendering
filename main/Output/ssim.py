from skimage.measure import compare_ssim
import argparse
import imutils
import cv2
from statistics import mean,stdev

ssim = []
for i in range(1,21):
  imageA = cv2.imread(str(i)+"out.png")
  imageB = cv2.imread(str(i)+"sample.png")
  grayA = cv2.cvtColor(imageA, cv2.COLOR_BGR2GRAY)
  grayB = cv2.cvtColor(imageB, cv2.COLOR_BGR2GRAY)
  (score, diff) = compare_ssim(grayA, grayB, full=True)
  diff = (diff * 255).astype("uint8")
  ssim.append(score)
  print(str(i)+"SSIM: {}".format(round(score,4)))
print("mean: "+str(round(mean(ssim),4))+" dev: "+str(round(stdev(ssim),4)))
