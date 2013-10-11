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

#ifndef MBB_PREREQUISITES_H_
#define MBB_PREREQUISITES_H_

#include <stdint.h>

// Input/output available data types.
//typedef int8_t              mbb_int8;
typedef uint8_t             mbb_uint8;
//typedef int16_t             mbb_int16;
//typedef uint16_t            mbb_uint16;
//typedef int32_t             mbb_int32;
//typedef uint32_t            mbb_uint32;
//typedef float               mbb_float;

// Floating point type used internally.
typedef float               mbb_real;

// Maximum number of channels a color can have.
#define mbb_kMaxChannels    4

#endif // MBB_PREREQUISITES_H_
