# Introduction
This document is intended only for the developers who are using Jupyter Lab for documentation in this repository.
Please update it for your own memo and/or to transfer information to fellow developers.

## Installation
https://jupyter.org/install

Along the way, you will be need many other packages, like _pandas_, but this is usually well explained in the web, please report here if you have difficulties and how you did manage to fix those.

## Both HTML and PDF from Jupyter (with images)
This requires some discipline and method - and the method used needs to be same all over to have the similar looking pages...

Having PDF requires TeXworks - and it will need to be set to install more packages without asking questions when it needs them: there will be a lots of them!

Most difficult is to have images in the PDF like in the HTML or Jupyter versions. Along the way, I decided to use flat structure, since the images are attached in the temporary directory and not in the Jupyter directory.

1. I put all the original images in the `img` directory
2. In this directory I have a script which calls `mogrify`-command of ImageMagick to resize the images to the above, flat directory where the document is - this way, the refererences are always direct, with the file name.
3. I use MD-markup language's image insert for the reduced size image - with a link to zoom the original image if necessary
4. For HTML, no embedded images - file is copied to the flat structure directory
5. PDF creation starts with LaTeX export
6. Open with TeXworks
7. Find out where the generated `*.tex` file is, in Windows, ``~AppData\Local\Temp`
8. Copy all flat structure (small size) image files into this directory
9. In TeXworks launch the conversion
10. Import the generated PDF-file into the Jupyter-directory, next to `.ipynb`

For the last, conversion part, I took this [annotated screenshot](img/Annotation_2019-10-12_125711_Jupyter-LaTeX-TeXworks-PDF_with_images.png)


