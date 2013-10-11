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

#ifndef MBB_MATH_H_
#define MBB_MATH_H_

#include "prerequisites.h"

/*  */
int smaller(int a, int b) {
  return (a < b ? a : b);
}

/*  */
int ceil(mbb_real a) {
  return (int)a;
}

/*  */
mbb_uint8 real_to_uint8(mbb_real v) {
  if (v > 255)
    return 255;
  else if (v < 0)
    return 0;
  else
    return (mbb_uint8)v;
}

#endif // MBB_MATH_H_
