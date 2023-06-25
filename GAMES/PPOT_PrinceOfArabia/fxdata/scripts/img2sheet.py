#images to sheet

import sys
import os
from PIL import Image
from math import sqrt

if (len(sys.argv) != 2):
  sys.stderr.write("No images to process.\n")
  sys.exit(-1)

imagepath = sys.argv[1]
if os.path.isfile(imagepath):
  imagepath = os.path.dirname(os.path.abspath(imagepath))
imagepath += os.sep  

images = []
for f in os.listdir(imagepath):
  if os.path.isfile(imagepath + f):
    images.append(f)

imagecount = len(images)
cols = imagecount // int(sqrt(imagecount))
if  (imagecount % cols) > (imagecount % (cols+1)): cols += 1
rows = (imagecount + cols -1) //  cols
img = Image.open(imagepath + images[0]).convert("RGBA")
width = img.size[0]
height = img.size[1]
new = Image.new("RGBA", (cols * width, rows * height), (0,0,0,0))
c = 0
r = 0
for i in range(len(images)):
  img = Image.open(imagepath + images[i]).convert("RGBA")
  new.paste(img,(c * width,r * height))
  c += 1
  if c == cols:
    c = 0
    r += 1
new.save('{}{}_{}x{}.png'.format(imagepath,os.path.basename(imagepath[:-1]),width,height))
