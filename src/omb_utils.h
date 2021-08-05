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

#ifndef _OMB_UTILS_H_
#define _OMB_UTILS_H_

typedef struct omb_device_item
{
	char *label;
	char *directory;
	char *identifier;
	char *background;
	struct omb_device_item *next;
} omb_device_item;

#define OMB_SETTINGS_SELECTED "selected"
#define OMB_SETTINGS_FORCE "force"
#define OMB_SETTINGS_NEXTBOOT "nextboot"
#define OMB_SETTINGS_TIMER "timer"
#define OMB_SETTINGS_RCTYPE "rctype"

int omb_utils_find_and_mount();
omb_device_item *omb_utils_get_images();
void omb_utils_update_background(omb_device_item *item);
void omb_utils_free_items(omb_device_item *items);

void omb_utils_save(const char* key, const char* value);
char* omb_utils_read(const char *key);
void omb_utils_save_int(const char* key, int value);
int omb_utils_read_int(const char *key);
int omb_utils_check_lock_menu();
void omb_utils_build_platform_wrapper(omb_device_item *item);

void omb_utils_remove_nextboot();
int omb_utils_gettimer();
void omb_utils_setrctype();

void omb_utils_init_system();
void omb_utils_prepare_destination(omb_device_item *item);
void omb_utils_load_modules(omb_device_item *item);
void omb_utils_load_modules_gl(omb_device_item *item);

void omb_utils_backup_kernel(omb_device_item *item);
void omb_utils_restore_kernel(omb_device_item *item);

void omb_utils_remount_media(omb_device_item *item);

void omb_utils_reboot();
void omb_utils_sysvinit(omb_device_item *item, const char *args);

#endif // _OMB_UTILS_H_
