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


int omb_branding_is_compatible(const char* base_dir)
{
	char fallback_arch_path[512];
	char box_type_inflash_cmd[512];
	char box_type_cmd[512];
	char box_type_inflash[255] = "\0";
	char box_type[255] = "\0";

	omb_log(LOG_DEBUG, "omb_branding_is_compatible: processing %s", base_dir);
	// we assume that flash image have boxbranding support
	sprintf(box_type_inflash_cmd, "%s %s /usr/lib/enigma2/python box_type", OMB_PYTHON_BIN, OMB_BRANDING_HELPER_BIN);
	FILE *fd = popen(box_type_inflash_cmd, "r");
	if (fd) {
		char buffer[255];
		char *line = fgets(buffer, sizeof(buffer), fd);
		if (line) {
			strtok(line, "\n");
			strncpy(box_type_inflash, line, sizeof(box_type_inflash));
		}
		pclose(fd);
	}
	
	sprintf(box_type_cmd, "%s %s %s/usr/lib/enigma2/python box_type 2>/dev/null", OMB_PYTHON_BIN, OMB_BRANDING_HELPER_BIN, base_dir);
	fd = popen(box_type_cmd, "r");
	if (fd) {
		char buffer[255];
		char *line = fgets(buffer, sizeof(buffer), fd);
		if (line) {
			strtok(line, "\n");
			strncpy(box_type, line, sizeof(box_type));
		}
		pclose(fd);
	}

	if (strlen(box_type) == 0) {
		sprintf(fallback_arch_path, "%s/etc/opkg/arch.conf", base_dir);
		omb_log(LOG_DEBUG, "omb_branding_is_compatible: fallback to %s parsing", fallback_arch_path);
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
				if (!strcmp(array[1],box_type_inflash)) {
					strcpy(box_type,array[1]);
					break;
				}
			}
			fclose(farch);
		}
		
	}
	if (!strcmp(box_type, box_type_inflash)) {
		omb_log(LOG_DEBUG, "omb_branding_is_compatible: box_type_inflash:%s == box_type:%s", box_type_inflash, box_type);
		return 1;
	}

	omb_log(LOG_DEBUG, "omb_branding_is_compatible: box_type_inflash:%s != box_type:%s", box_type_inflash, box_type);
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
		if (strcmp(identifier, "flash") == 0)
			sprintf(item->label, "%s %s (flash)", name, version);
		else
			sprintf(item->label, "%s %s", name, version);
	}
	else {
		item->label = malloc(strlen(settings_value) + 9);
		if (strcmp(identifier, "flash") == 0)
			sprintf(item->label, "%s (flash)", settings_value);
		else
			sprintf(item->label, "%s", settings_value);
		free(settings_value);
	}
		
	return item;
}