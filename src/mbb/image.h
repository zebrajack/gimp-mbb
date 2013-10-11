//  Copyright (C) 2013 Saulo A. Pessoa <saulopessoa@gmail.com>.
//
//  This file is part of MBB GIMP.
//
//  MBB GIMP is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  MBB GIMP is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with MBB GIMP. If not, see <http://www.gnu.org/licenses/>.

#ifndef MBB_IMAGE_H_
#define MBB_IMAGE_H_

#include "prerequisites.h"

/* Struct used to store images. */
typedef struct {
  int width;
  int height;
  int channels;
  mbb_real *data;
} Image;

/* Constructor. */
Image *image_ctor(Image *img, int width, int height, int channels);
/* Destructor. */
void image_dtor(Image *img);

/* Sample pixel. */
mbb_real *sample(const Image *img, int w, int h);

/* Copy color to output. */
mbb_real *color_copy(int channels, const mbb_real *a, mbb_real *out);
/* Subtract colors (channel-by-channel) and place result in output. */
mbb_real *color_sub(int channels, const mbb_real *a, const mbb_real *b, mbb_real *out);
/* Add colors (channel-by-channel) and place result in output. */
mbb_real *color_add(int channels, const mbb_real *a, const mbb_real *b, mbb_real *out);
/* Multiply colors (channel-by-channel) and place result in output. */
mbb_real *color_mul(int channels, const mbb_real *a, const mbb_real *b, mbb_real *out);
/* Multiply color by an scalar and place result in output. */
mbb_real *color_muls(int channels, const mbb_real *a, mbb_real b, mbb_real *out);
/* Fill every channel of a color with an scalar. */
mbb_real *color_fill(int channels, mbb_real a, mbb_real *out);

/* Perform a deep copy of an image to output (output constructor is
automatically called). */
Image *image_deepcopy(const Image *a, Image *out);
/* Subtract two images (pixel-by-pixel) and place result in output. */
Image *image_sub(const Image *a, const Image *b, Image *out);
/* Add two images (pixel-by-pixel) and place result in output. */
Image *image_add(const Image *a, const Image *b, Image *out);

#endif // MBB_IMAGE_H_
