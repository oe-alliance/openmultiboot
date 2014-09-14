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