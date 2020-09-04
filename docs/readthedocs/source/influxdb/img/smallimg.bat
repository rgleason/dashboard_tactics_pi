rem Mogrify the images
mkdir simg
del s_*.png
del simg/*.png
for %%a in (*.png) do mogrify -path simg -resize "480x600^>" %%a
cd simg
for %%a in (*.png) do copy /Y "%%a" ..\"s_%%a"
for %%a in (*.png) do del %%a
cd ..
rmdir simg