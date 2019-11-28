# A PowerShell Script to resize all *.PNG images in the directory, copy to ..
 # mogrify-command is used, from ImageMagick-package

mogrify -path .. -resize '800x600>' *.png
