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

#include <stdio.h>
#include <stdarg.h>

#include "omb_log.h"
#include "omb_common.h"

void omb_log(int level, const char* format, ...)
{
	va_list arglist;
	
	switch(level)
	{
		case LOG_ERROR:
			printf("%s - error: ", OMB_APP_NAME);
			break;
		case LOG_WARNING:
			printf("%s - warning: ", OMB_APP_NAME);
			break;
		case LOG_DEBUG:
			printf("%s - debug: ", OMB_APP_NAME);
			break;
	}
	
	va_start(arglist, format);
	vprintf(format, arglist);
	va_end(arglist);
	printf("\n");
}
