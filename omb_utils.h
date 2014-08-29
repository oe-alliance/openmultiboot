#ifndef _OMB_UTILS_H_
#define _OMB_UTILS_H_

typedef struct omb_device_item
{
	char *label;
	char *directory;
	char *identifier;
	struct omb_device_item *next;
} omb_device_item;

#define OMB_SETTINGS_SELECTED "selected"
#define OMB_SETTINGS_FORCE "force"

int omb_utils_find_and_mount();
omb_device_item *omb_utils_get_images();
void omb_utils_update_background(omb_device_item *item);
void omb_utils_free_items(omb_device_item *items);

void omb_utils_save(const char* key, const char* value);
char* omb_utils_read(const char *key);
void omb_utils_save_int(const char* key, int value);
int omb_utils_read_int(const char *key);

void omb_utils_init_system();
void omb_utils_load_modules(omb_device_item *item);

void omb_utils_backup_kernel(omb_device_item *item);
void omb_utils_restore_kernel(omb_device_item *item);

void omb_utils_remount_media(omb_device_item *item);

void omb_utils_reboot();
void omb_utils_sysvinit(omb_device_item *item, const char *args);

#endif // _OMB_UTILS_H_
