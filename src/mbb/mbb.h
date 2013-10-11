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

#ifndef MBB_MBB_H_
#define MBB_MBB_H_

#include "prerequisites.h"
#include "image.h"

/* Available input/output data types enums. */
typedef enum {
  kUint8 = 0//,
  // TODO(saulo): Add more data types here...
} mbb_DataTypes;


#ifdef __cplusplus
extern "C" {
#endif

/* Blends two images according to a mask. Mask and blended image have
same width, height, and channels as the input images. Blended image data
buffer must be allocated by the client. */
void *mbb_blend(int width, int height, int channels, mbb_DataTypes type,
                const void *img0_data, const void *img1_data,
                const void *mask_data, void *o_img_data);

#ifdef __cplusplus
}
#endif

#endif // MBB_MBB_H_
