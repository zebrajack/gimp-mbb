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

#include "image.h"

#include <stdlib.h>
#include <string.h>

Image *image_ctor(Image *img, int width, int height, int channels) {
  Image *result = img;
  result->width = width;
  result->height = height;
  result->channels = channels;
  result->data = (mbb_real*)malloc(result->width * result->height * result->channels * sizeof(mbb_real));
  memset(result->data, 0, result->width * result->height * result->channels * sizeof(mbb_real));
  return result;
}

void image_dtor(Image *img) {
  free(img->data);
}

mbb_real *sample(const Image *img, int w, int h) {
  mbb_real *result = &img->data[(h*img->width + w)*img->channels];
  return result;
}

mbb_real *color_copy(int channels, const mbb_real *a, mbb_real *out) {
  int i;
  for (i = 0; i < channels; ++i)
    out[i] = a[i];
  return out;
}

mbb_real *color_sub(int channels, const mbb_real *a, const mbb_real *b,
                         mbb_real *out) {
  int i;
  for (i = 0; i < channels; ++i)
    out[i] = a[i] - b[i];
  return out;
}

mbb_real *color_add(int channels, const mbb_real *a, const mbb_real *b,
                         mbb_real *out) {
  int i;
  for (i = 0; i < channels; ++i)
    out[i] = a[i] + b[i];
  return out;
}

mbb_real *color_mul(int channels, const mbb_real *a, const mbb_real *b, mbb_real *out) {
  int i;
  for (i = 0; i < channels; ++i)
    out[i] = a[i] * b[i];
  return out;
}

mbb_real *color_muls(int channels, const mbb_real *a, mbb_real b,
                         mbb_real *out) {
  int i;
  for (i = 0; i < channels; ++i)
    out[i] = a[i] * b;
  return out;
}

mbb_real *color_fill(int channels, mbb_real a, mbb_real *out) {
  int i;
  for (i = 0; i < channels; ++i)
    out[i] = a;
  return out;
}

Image *image_deepcopy(const Image *a, Image *out) {
  int wi, hi;

  image_ctor(out, a->width, a->height, a->channels);

  for (hi = 0; hi < a->height; ++hi)
    for (wi = 0; wi < a->width; ++wi)
      color_copy(a->channels, sample(a,wi,hi), sample(out,wi,hi));

  return out;
}

Image *image_sub(const Image *a, const Image *b, Image *out) {
  int wi, hi;
  for (hi = 0; hi < a->height; ++hi)
    for (wi = 0; wi < a->width; ++wi)
      color_sub(a->channels, sample(a,wi,hi), sample(b,wi,hi), sample(out,wi,hi));

  return out;
}

Image *image_add(const Image *a, const Image *b, Image *out) {
  int wi, hi;
  for (hi = 0; hi < a->height; ++hi)
    for (wi = 0; wi < a->width; ++wi)
      color_add(a->channels, sample(a,wi,hi), sample(b,wi,hi), sample(out,wi,hi));

  return out;
}
