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

#include "images_pyramid.h"

#include <stdlib.h>

ImagesPyramid *images_pyramid_ctor(ImagesPyramid *imgs_pyramid, int levels) {
  imgs_pyramid->levels = levels;
  imgs_pyramid->data = (Image*)malloc(imgs_pyramid->levels * sizeof(Image));
  return imgs_pyramid;
}

void images_pyramid_dtor(ImagesPyramid *imgs_pyramid) {
  int i;
  for (i = 0; i < imgs_pyramid->levels; ++i)
    image_dtor(&imgs_pyramid->data[i]);

  free(imgs_pyramid->data);
}
