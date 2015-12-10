# Multi-Band Blending (MBB) #

## Introduction ##
This is a plugin for GIMP version 2.8.x which implements the blending technique described in ‘A Multiresolution Spline With Application to Image Mosaics’, by Peter J Burt and Edward H. Adelson. This technique blends two arbitrary images according to a third one (the mask). Conceptually speaking, this technique blends images using a different weighting function for each frequency band. It has the benefit of producing more natural transitions between images boundaries than the usual alpha blending technique.

If you desire, fell free to use the source code on your own project. Since the core MBB functionalities were implemented detached from the plugin GUI, it would be easy to anyone to get it running inside other projects. Source code can be downloaded from https://code.google.com/p/gimp-mbb/. Binaries can be downloaded from http://registry.gimp.org/node/31957.

## Features/Restrictions ##
  * Blends two images according to mask.
  * The images and the mask must be in separated layers with equal size.
  * A new layer is created with the final result.
  * By default, blending is performed for the whole image.
  * If part of the image is selected, blending is restricted to this part.

## Changes ##
Current version:
0.1 (2014-04-20)
  * Initial release.

## Screens/Results ##
MBB plugin dialog:
![http://wiki.gimp-mbb.googlecode.com/git/imgs/dialog.png](http://wiki.gimp-mbb.googlecode.com/git/imgs/dialog.png)

Result using the MBB plugin:
![http://wiki.gimp-mbb.googlecode.com/git/imgs/mbb_result.png](http://wiki.gimp-mbb.googlecode.com/git/imgs/mbb_result.png)

Result using the usual alpha blending technique (observe that the same mask is used):
![http://wiki.gimp-mbb.googlecode.com/git/imgs/alpla_blending_result.png](http://wiki.gimp-mbb.googlecode.com/git/imgs/alpla_blending_result.png)