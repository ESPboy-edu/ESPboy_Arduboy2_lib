#!/bin/bash

montage ../images/prince/left/Backup/???_*.png -geometry 36x36+0+0 -background none -tile 15x15 ../images/prince/left/Prince_Left_36x36.png
montage ../images/prince/right/Backup/???_*.png -geometry 36x36+0+0 -background none -tile 15x15 ../images/prince/right/Prince_Right_36x36.png
montage ../images/mirror/left/Backup/???_*.png -geometry 36x36+0+0 -background none -tile 15x15 ../images/mirror/left/Mirror_Left_36x36.png
montage ../images/mirror/right/Backup/???_*.png -geometry 36x36+0+0 -background none -tile 15x15 ../images/mirror/right/Mirror_Right_36x36.png
montage ../images/enemy/left/Backup/???_*.png -geometry 36x36+0+0 -background none -tile 15x15 ../images/enemy/left/Enemy_Left_36x36.png
montage ../images/enemy/right/Backup/???_*.png -geometry 36x36+0+0 -background none -tile 15x15 ../images/enemy/right/Enemy_Right_36x36.png
montage ../images/skeleton/left/Backup/???_*.png -geometry 36x36+0+0 -background none -tile 15x15 ../images/skeleton/left/Skeleton_Left_36x36.png
montage ../images/skeleton/right/Backup/???_*.png -geometry 36x36+0+0 -background none -tile 15x15 ../images/skeleton/right/Skeleton_Right_36x36.png

Python3 scripts/createMovements.py ../src/utils/Constants.h movements.txt
Python3 scripts/createSEQData.py ../src/utils/Constants.h ../src/utils/movements_SEQData.h
Python3 scripts/createOFFSETSData.py ../src/utils/Constants.h ../src/utils/movements_OFFSETSData.h

python3 ./Arduboy-Python-Utilities-master/fxdata-build.py fxdata.txt

cp fxdata.bin ../../build/fxdata.bin



# montage ???_*.png -geometry 36x36+0+0 -background none -tile 15x15 Mirror_Right_36x36.png
# ./midiConverter -o2 -k12 -fa pop_ending

# montage ../images/enemy/left/Backup/???_*.png -geometry 36x36+0+0 -background none -tile 15x15 ../images/enemy/left/Enemy_Left_36x36.png