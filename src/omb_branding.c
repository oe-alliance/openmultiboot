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
#include <stdlib.h>
#include <string.h>

#include "omb_common.h"
#include "omb_log.h"
#include "omb_branding.h"
#include "omb_utils.h"

extern char *brand_oem;

char * omb_branding_get_brand_oem(const char* base_dir) {
	char brand_oem_cmd[512];
	char *p_brand_oem;
	FILE *fd;

	omb_log(LOG_DEBUG, "%-33s: processing %s", __FUNCTION__, base_dir);
	sprintf(brand_oem_cmd, "%s %s /usr/lib/enigma2/python brand_oem", OMB_PYTHON_BIN, OMB_BRANDING_HELPER_BIN);
	fd = popen(brand_oem_cmd, "r");
	if (fd) {
		char buffer[255];
		char *line = fgets(buffer, sizeof(buffer), fd);
		if (line) {
			strtok(line, "\n");
			p_brand_oem = malloc(strlen(line) + 1);
			strcpy(p_brand_oem, line);
		}
		pclose(fd);
		omb_log(LOG_DEBUG, "%-33s: brand_oem = %s", __FUNCTION__, p_brand_oem);
	}
	return p_brand_oem;
}

char * omb_branding_get_box_type(const char* base_dir) {

	char box_type_cmd[512];
	char *p_box_type;
	p_box_type = malloc(1);
	p_box_type[0] = 0;

	FILE *fd;

	omb_log(LOG_DEBUG, "%-33s: processing %s (%s)", __FUNCTION__, base_dir, brand_oem);
	sprintf(box_type_cmd, "%s %s %s/usr/lib/enigma2/python box_type", OMB_PYTHON_BIN, OMB_BRANDING_HELPER_BIN, base_dir);
	fd = popen(box_type_cmd, "r");
	if (fd) {
		char buffer[255];
		char *line = fgets(buffer, sizeof(buffer), fd);
		if (line) {
			strtok(line, "\n");
			p_box_type = realloc(p_box_type,strlen(line) + 1);
			strcpy(p_box_type, line);
		}
		pclose(fd);
		omb_log(LOG_DEBUG, "%-33s: box_type = %s", __FUNCTION__, p_box_type);
	}

	//fix for buggy oe-branding support of some image
	if (strlen(p_box_type) != 0 && !strcmp(brand_oem,"vuplus") && strncmp(p_box_type,"vu",2)) {
		omb_log(LOG_DEBUG, "%-33s: buggy image... box_type:%s is invalid", __FUNCTION__, p_box_type);
		char buffer[255];
		strcpy(buffer,p_box_type);
		p_box_type = realloc(p_box_type,strlen(p_box_type) + 3);
		sprintf(p_box_type, "vu%s",buffer);
		omb_log(LOG_DEBUG, "%-33s: patched box_type:%s should be ok", __FUNCTION__, p_box_type);
	}
	return p_box_type;
}

int omb_branding_is_compatible(const char* base_dir, const char* flash_box_type, const char* box_type)
{
	// if box_type is empty, probably the image isn't a boxbranded image... so apply a fix
	omb_log(LOG_DEBUG, "%-33s: comparing %s with %s", __FUNCTION__, flash_box_type, box_type);

	char fallback_arch_path[512];

	if (!strcmp(box_type, flash_box_type)) {
		omb_log(LOG_DEBUG, "%-33s: flash_box_type:%s == box_type:%s", __FUNCTION__, flash_box_type, box_type);
		return 1;
	}

	if (strlen(box_type) == 0) {
		sprintf(fallback_arch_path, "%s/etc/opkg/arch.conf", base_dir);
		omb_log(LOG_DEBUG, "omb_branding_get_box_type: fallback to %s parsing", __FUNCTION__, fallback_arch_path);
		FILE *farch = fopen(fallback_arch_path, "r");
		if (farch) {
			char buffer[255];
			while (fgets(buffer, sizeof(buffer), farch)) {
				char *token = strtok(buffer, " \t");
				int i = 0;
				char *array[80];
				while (token) {
					array[i] = malloc(strlen(token) + 1);
					strcpy(array[i],token);
					//printf("%2d %s\n", i, token);
					token = strtok(NULL, " \t");
					i++;
				}
				if (!strcmp(array[1],flash_box_type)) {
					omb_log(LOG_DEBUG, "%-33s: flash_box_type:%s == box_type:%s (workaround)", __FUNCTION__, flash_box_type, array[1]);
					//strcpy(box_type,array[1]);
					//break;
					return 1;
				}
			}
			fclose(farch);
		}
	}

	omb_log(LOG_DEBUG, "%-33s: flash_box_type:%s != box_type:%s", __FUNCTION__, flash_box_type, box_type);
	return 0;
}

omb_device_item *omb_branding_read_info(const char* base_dir, const char *identifier)
{
	char version[255];
	char name[255];
	char distro_cmd[512];
	char version_cmd[512];
	char settings_key[255];
	char *settings_value;

	sprintf(settings_key, "label_%s", identifier);
	settings_value = omb_utils_read(settings_key);

	omb_device_item *item = malloc(sizeof(omb_device_item));
	item->directory = malloc(strlen(base_dir) + 1);
	item->identifier = malloc(strlen(identifier) + 1);
	item->next = NULL;
	strcpy(item->directory, base_dir);
	strcpy(item->identifier, identifier);

	item->box_type = omb_branding_get_box_type(base_dir); // we use the malloc of the function
	
	if (!settings_value) {
		version[0] = name[0] = '\0';

		sprintf(distro_cmd, "%s %s %s/usr/lib/enigma2/python image_distro", OMB_PYTHON_BIN, OMB_BRANDING_HELPER_BIN, base_dir);
		FILE *fd = popen(distro_cmd, "r");
		if (fd) {
			char buffer[255];
			char *line = fgets(buffer, sizeof(buffer), fd);
			if (line) {
				strtok(line, "\n");
				strncpy(name, line, sizeof(name));
			}
			pclose(fd);
		}
	
		sprintf(version_cmd, "%s %s %s/usr/lib/enigma2/python image_version", OMB_PYTHON_BIN, OMB_BRANDING_HELPER_BIN, base_dir);
		fd = popen(version_cmd, "r");
		if (fd) {
			char buffer[255];
			char *line = fgets(buffer, sizeof(buffer), fd);
			if (line) {
				strtok(line, "\n");
				strncpy(version, line, sizeof(version));
			}
			pclose(fd);
		}
	
		if (strlen(name) == 0)
			strcpy(name, identifier);
		
		item->label = malloc(strlen(name) + strlen(version) + 10);
		if (strcmp(identifier, "flash") == 0) {
			sprintf(item->label, "%s %s (flash)", name, version);
			item->is_inflash = 1;
		} else{
			sprintf(item->label, "%s %s", name, version);
			item->is_inflash = 0;
		}
	}
	else {
		item->label = malloc(strlen(settings_value) + 9);
		if (strcmp(identifier, "flash") == 0) {
			sprintf(item->label, "%s (flash)", settings_value);
			item->is_inflash = 1;
		} else {
			sprintf(item->label, "%s", settings_value);
			item->is_inflash = 0;
		}
		free(settings_value);
	}
		
	return item;
}