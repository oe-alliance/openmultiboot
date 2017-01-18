/*
 *
 * Copyright (C) 2014 Impex-Sat Gmbh & Co.KG
 * Written by Sandro Cavazzoni <sandro@skanetwork.com>
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef _OMB_BRANDING_H_
#define _OMB_BRANDING_H_

#include "omb_utils.h"	

int omb_branding_is_compatible(const char* base_dir, const char* flash_box_type, const char* box_type);

omb_device_item *omb_branding_read_info(const char* base_dir, const char *identifier);

char * omb_branding_get_brand_oem(const char* base_dir);
char * omb_branding_get_box_type(const char* base_dir);

#endif // _OMB_BRANDING_H_
