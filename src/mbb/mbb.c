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

#include "mbb.h"

#include "image.h"
#include "images_pyramid.h"
#include "math.h"

/* 1D weight function (it is equivalent to a gaussian function). */
static mbb_real weight1D(int v) {
  const mbb_real a = 0.4f;
  const mbb_real b = 1.0f/4.0f;
  const mbb_real c = 1.0f/4.0f - a/2.0f;

  if (v == 0)
    return a;
  else if (v == -1 || v == 1)
    return b;
  else if (v == -2 || v == 2)
    return c;

  return 0;
}

/* 2D weight function. */
static mbb_real weight2D(int m, int n) {
  mbb_real result = weight1D(m) * weight1D(n);
  return result;
}

/* Sample color from image. It is aware of image bounds and return proper
values when pixels outside it are requested. */
static mbb_real *sample_b(const Image *img, int w, int h, mbb_real *out) {
  mbb_real tmp_buf0[mbb_kMaxChannels];

  // Image corners
  if (h < 0 && w < 0) {
    color_muls(img->channels, sample(img, 0, 0), 2, tmp_buf0);
    return color_sub(img->channels, tmp_buf0, sample(img, -w, -h), out);
  }
  if (h < 0 && w >= img->width) {
    color_muls(img->channels, sample(img, img->width-1, 0), 2, tmp_buf0);
    return color_sub(img->channels, tmp_buf0, sample(img, 2*img->width-w-1, -h),out);
  }
  if (h >= img->height && w >= img->width) {
    color_muls(img->channels, sample(img, img->width-1, img->height-1), 2,
              tmp_buf0);
    return color_sub(img->channels, tmp_buf0,
              sample(img, 2*img->width-w-1, 2*img->height-h-1), out);
  }
  if (h >= img->height && w < 0) {
    color_muls(img->channels, sample(img, 0, img->height-1), 2, tmp_buf0);
    return color_sub(img->channels, tmp_buf0, sample(img,-w, 2*img->height-h-1),out);
  }

  // Image edges
  if (h < 0) {
    color_muls(img->channels, sample(img, w, 0), 2, tmp_buf0);
    return color_sub(img->channels, tmp_buf0, sample(img, w, -h), out);
  }
  if (w < 0) {
    color_muls(img->channels, sample(img, 0, h), 2, tmp_buf0);
    return color_sub(img->channels, tmp_buf0, sample(img, -w, h), out);
  }
  if (h >= img->height) {
    color_muls(img->channels, sample(img, w, img->height-1), 2, tmp_buf0);
    return color_sub(img->channels, tmp_buf0, sample(img, w, 2*img->height-h-1),out);
  }
  if (w >= img->width) {
    color_muls(img->channels, sample(img, img->width-1, h), 2, tmp_buf0);
    return color_sub(img->channels, tmp_buf0, sample(img, 2*img->width-w-1, h), out);
  }

  return color_copy(img->channels, sample(img, w, h), out);
}

/* Reduce image. */
static Image *reduce(const Image *img, Image *reduced_img) {
  int wi, hi;
  int mi, ni;

  image_ctor(reduced_img, (img->width+1)/2, (img->height+1)/2, img->channels);

  for (hi = 0; hi < reduced_img->height; ++hi) {
    for (wi = 0; wi < reduced_img->width; ++wi) {
      mbb_real *sampled = sample(reduced_img, wi, hi);

      for (mi = -2; mi <= 2; ++mi) {
        for (ni = -2; ni <= 2; ++ni) {
          mbb_real tmp_buf0[mbb_kMaxChannels];
          mbb_real tmp_buf1[mbb_kMaxChannels];

          color_muls(img->channels, sample_b(img, wi*2+ni, hi*2+mi, tmp_buf0),
                     weight2D(mi, ni), tmp_buf1);

          color_add(reduced_img->channels, sampled, tmp_buf1, sampled);
        }
      }
    }
  }

  return reduced_img;
}

/* Expand image. */
static Image *expand(const Image *img, Image *expanded_img) {
  int wi, hi;
  int mi, ni;

  image_ctor(expanded_img, img->width*2 - 1, img->height*2 - 1, img->channels);

  for (hi = 0; hi < expanded_img->height; ++hi) {
    for (wi = 0; wi < expanded_img->width; ++wi) {
      mbb_real *sampled = sample(expanded_img, wi, hi);

      for (mi = -2; mi <= 2; ++mi) {
        for (ni = -2; ni <= 2; ++ni) {
          if ((hi+mi)%2 == 0 && (wi+ni)%2 == 0) {
            mbb_real tmp_buf0[mbb_kMaxChannels];
            mbb_real tmp_buf1[mbb_kMaxChannels];

            color_muls(img->channels, sample_b(img, (wi+ni)/2, (hi+mi)/2, tmp_buf0),
                       weight2D(mi, ni), tmp_buf1);

            color_add(expanded_img->channels, sampled, tmp_buf1, sampled);
          }
        }
      }
      color_muls(expanded_img->channels, sampled, 4, sampled);
    }
  }

  return expanded_img;
}

/* Evaluate the number of levels a pyramid will have. It is determined by the
largest image dimension. */
static int eval_max_level(const Image *img) {
  int smaller_dimension = smaller(img->width, img->height) - 1;
  int tmp_shift = smaller_dimension;
  int counter = 0;

  while (tmp_shift >= 1) {
    tmp_shift = tmp_shift >> 1;
    ++counter;
  }

  return counter;
}

/* Build gaussian pyramid from the first image (G0). */
static ImagesPyramid *build_G(const Image *G0, ImagesPyramid *G) {
  int max_level = eval_max_level(G0);
  int i;

  images_pyramid_ctor(G, max_level);
  G->data[0] = *G0;

  for (i = 0; i < G->levels-1; ++i) {
    Image Gip1;
    reduce(&G->data[i], &Gip1);
    G->data[i+1] = Gip1;
  }

  return G;
}

/* Build laplacian pyramid from the gaussian pyramid. */
static ImagesPyramid *build_L(const ImagesPyramid *G, ImagesPyramid *L) {
  int i;

  images_pyramid_ctor(L, G->levels);
  // A deep copy is necessary because this image will be after deleted twice
  // (once from the gaussian pyramid and once from the laplacian pyramid)
  image_deepcopy(&G->data[G->levels-1], &L->data[L->levels-1]);
//  L->data[L->levels-1] = G->data[G->levels-1];

  for (i = 0; i < L->levels-1; ++i) {
    Image Li;
    expand(&G->data[i+1], &Li);
    image_sub(&G->data[i], &Li, &Li);
    L->data[i] = Li;
  }

  return L;
}

/* Blend two images according to a mask. */
static Image *blend_imgs(const Image *img0, const Image *img1, const Image *mask, Image *out) {
  ImagesPyramid G_img0, L_img0,  // Img0 gaussian and laplacian pyramid
                G_img1, L_img1,  // Img1 gaussian and laplacian pyramid
                G_mask,          // Mask laplacian pyramid
                L_blended;       // Blended laplacian pyramid
  int i;
  Image blended_img;

  build_G(img0, &G_img0);
  build_L(&G_img0, &L_img0);

  build_G(img1, &G_img1);
  build_L(&G_img1, &L_img1);

  build_G(mask, &G_mask);

  images_pyramid_ctor(&L_blended, L_img0.levels);

  for (i = 0; i < L_blended.levels; ++i) {
    int wi, hi;
    const Image *Li_img0 = &L_img0.data[i];
    const Image *Li_img1 = &L_img1.data[i];
    const Image *Gi_mask = &G_mask.data[i];
    Image Li_blended;

    image_ctor(&Li_blended, Li_img0->width, Li_img0->height, Li_img0->channels);

    for (hi = 0; hi < Li_blended.height; ++hi) {
      for (wi = 0; wi < Li_blended.width; ++wi) {
        mbb_real tmp_buf0[mbb_kMaxChannels];
        mbb_real tmp_buf1[mbb_kMaxChannels];

        mbb_real *Li_blended_samp = sample(&Li_blended, wi, hi);
        const mbb_real *Li_img0_samp = sample(Li_img0, wi, hi);
        const mbb_real *Li_img1_samp = sample(Li_img1, wi, hi);
        const mbb_real *Gi_mask_samp = sample(Gi_mask, wi, hi);

        color_mul(Li_blended.channels, Li_img0_samp, Gi_mask_samp, tmp_buf0);

        color_fill(Li_blended.channels, 1, tmp_buf1);
        color_sub(Li_blended.channels, tmp_buf1, Gi_mask_samp, tmp_buf1);
        color_mul(Li_blended.channels, Li_img1_samp, tmp_buf1, tmp_buf1);

        color_add(Li_blended.channels, tmp_buf0, tmp_buf1, Li_blended_samp);
      }
    }
    L_blended.data[i] = Li_blended;
  }

  // Restore blended image from its laplacian pyramid
  image_deepcopy(&L_blended.data[L_blended.levels-1], &blended_img);
  for (i = L_blended.levels - 2; i >= 0; --i) {
    Image expanded_img;
    expand(&blended_img, &expanded_img);

    image_dtor(&blended_img);
    image_ctor(&blended_img, expanded_img.width, expanded_img.height, expanded_img.channels);

    image_add(&L_blended.data[i], &expanded_img, &blended_img);

    image_dtor(&expanded_img);
  }

  images_pyramid_dtor(&G_img0);
  images_pyramid_dtor(&L_img0);
  images_pyramid_dtor(&G_img1);
  images_pyramid_dtor(&L_img1);
  images_pyramid_dtor(&G_mask);
  images_pyramid_dtor(&L_blended);

  *out = blended_img;
  return out;
}

/* Evaluate a suitable size for a given dimenstion of the input image.
It is necessary because multi-band operations can only be performed in
images with sizes such as: n^2+1, for n >= 0. In order to reduce the loss
of information, it will always be bigger than or equal the original dimension. */
static int eval_scaled_dimension(int v) {
  int tmp_shift = v-2;
  int counter = 0;
  int result = 0;

  while (tmp_shift > 1) {
    tmp_shift = tmp_shift >> 1;
    ++counter;
  }
  result = (0x1 << (counter+1)) + 1;

  return result;
}

/* Convert data block to an image of suitable size. */
static Image *data_to_image(int width, int height, int channels,
                            const void *data, Image *img, mbb_real scale) {
  int wi, hi;
  int scaled_width, scaled_height;

  scaled_width = eval_scaled_dimension(width);
  scaled_height = eval_scaled_dimension(height);

  image_ctor(img, scaled_width, scaled_height, channels);

  for (hi = 0; hi < img->height; ++hi) {
    float hin = hi/(float)img->height + 1.0f/(2*img->height);
    int his = ceil(height*hin);
    for (wi = 0; wi < img->width; ++wi) {
      float win = wi/(float)img->width + 1.0f/(2*img->width);
      int wis = ceil(width*win);

      mbb_real *sampled = sample(img, wi, hi);
      int ci;
      for (ci = 0; ci < channels; ++ci)
        sampled[ci] = scale *
                      ((mbb_uint8*)data)[(his*width + wis)*channels + ci];
    }
  }

  return img;
}

/* Convert image to a data block of a given size (it is usually the input
data block original size). */
static void *image_to_data(const Image *img, int width, int height,
                           int channels, void *o_img_data) {
  int wi, hi;

  for (hi = 0; hi < height; ++hi) {
    float hin = hi/(float)height + 1.0f/(2*height);
    int his = ceil(img->height*hin);
    for (wi = 0; wi < width; ++wi) {
      float win = wi/(float)width + 1.0f/(2*width);
      int wis = ceil(img->width*win);

      mbb_real *sampled = sample(img, wis, his);
      int ci;
      for (ci = 0; ci < channels; ++ci)
        ((mbb_uint8*)o_img_data)[(hi*width + wi)*channels + ci] =
          real_to_uint8(sampled[ci]);
    }
  }

  return o_img_data;
}

// 
void *mbb_blend(int width, int height, int channels, mbb_DataTypes type,
                const void *img0_data, const void *img1_data,
                const void *mask_data, void *o_img_data) {
  Image img0, img1, mask;
  Image blended;

  data_to_image(width, height, channels, img0_data, &img0, 1);
  data_to_image(width, height, channels, img1_data, &img1, 1);
  data_to_image(width, height, channels, mask_data, &mask, 1.0f/255);

  blend_imgs(&img0, &img1, &mask, &blended);

  image_to_data(&blended, width, height, channels, o_img_data);

  image_dtor(&blended);
  return o_img_data;
}
