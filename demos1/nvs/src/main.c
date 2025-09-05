#include <zephyr/kernel.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/device.h>
#include <string.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/fs/nvs.h>

static struct nvs_fs fs;

//This is important for the flash partition
#define NVS_PARTITION		storage_partition
#define NVS_PARTITION_DEVICE	FIXED_PARTITION_DEVICE(NVS_PARTITION)
#define NVS_PARTITION_OFFSET	FIXED_PARTITION_OFFSET(NVS_PARTITION)

#define ADDRESS_ID 1
#define KEY_ID 2
#define RBT_CNT_ID 3
#define STRING_ID 4

int main(void)
{
        int rc = 0;
        int cnt = 0;
        int cnt_his = 0;
        char buffer[16];
        u_int8_t key[8];
        uint32_t reboot_counter = 0U, reboot_counter_his;
	struct flash_pages_info info;

        //Define the flash device and get its parameters
        fs.flash_device = NVS_PARTITION_DEVICE;
	if (!device_is_ready(fs.flash_device)) {
		printk("Flash device %s is not ready\n", fs.flash_device->name);
		return 0;
	}

        fs.offset = NVS_PARTITION_OFFSET;
	rc = flash_get_page_info_by_offs(fs.flash_device, fs.offset, &info);
	if (rc) {
		printk("Unable to get page info, rc=%d\n", rc);
		return 0;
	}
	
	fs.sector_size = info.size;
	fs.sector_count = 3U;

	rc = nvs_mount(&fs);
	if (rc) {
		printk("Flash Init failed, rc=%d\n", rc);
		return 0;
	}

        rc = nvs_read(&fs, ADDRESS_ID, &buffer, sizeof(buffer));
	if (rc > 0) { /* item was found, show it */
		printk("Id: %d, Address: %s\n", ADDRESS_ID, buffer);
	} else   {/* item was not found, add it */
		strcpy(buffer, "192.168.1.1");
		printk("No address found, adding %s at id %d\n", buffer, ADDRESS_ID);
		(void)nvs_write(&fs, ADDRESS_ID, &buffer, strlen(buffer)+1);
	}

	rc = nvs_read(&fs, KEY_ID, &key, sizeof(key));
	if (rc > 0) { /* item was found, show it */
		printk("Id: %d, Key: ", KEY_ID);
		for (int n = 0; n < 8; n++) {
			printk("%x ", key[n]);
		}
		printk("\n");
	} else   {/* item was not found, add it */
		printk("No key found, adding it at id %d\n", KEY_ID);
		key[0] = 0xFF;
		key[1] = 0xFE;
		key[2] = 0xFD;
		key[3] = 0xFC;
		key[4] = 0xFB;
		key[5] = 0xFA;
		key[6] = 0xF9;
		key[7] = 0xF8;
		(void)nvs_write(&fs, KEY_ID, &key, sizeof(key));
	}

        rc = nvs_read(&fs, RBT_CNT_ID, &reboot_counter, sizeof(reboot_counter));
	if (rc > 0) { /* item was found, show it */
		printk("Id: %d, Reboot_counter: %d\n", RBT_CNT_ID, reboot_counter);
	} else   {/* item was not found, add it */
		printk("No Reboot counter found, adding it at id %d\n", RBT_CNT_ID);
		(void)nvs_write(&fs, RBT_CNT_ID, &reboot_counter, sizeof(reboot_counter));
	}

        /* STRING_ID is used to store data that will be deleted,lets see
	 * if we can read it from flash, since we don't know the size read the
	 * maximum possible
	 */
        rc = nvs_read(&fs, STRING_ID, &buffer, sizeof(buffer));
	if (rc > 0) {
		/* item was found, show it */
		printk("Id: %d, Data: %s\n", STRING_ID, buffer);
		/* remove the item if reboot_counter = 10 */
		if (reboot_counter == 10U) {
			(void) nvs_delete(&fs, STRING_ID);
		}
	} else   {
		/* entry was not found, add it if reboot_counter = 0*/
		if (reboot_counter == 0U) {
			printk("Id: %d not found, adding it\n", STRING_ID);
			strcpy(buffer, "DATA");
			(void)nvs_write(&fs, STRING_ID, &buffer, strlen(buffer) + 1);
		}
	}

        cnt = CONFIG_NVS_SAMPLE_REBOOT_COUNTDOWN;

        while (1) {
		k_msleep(CONFIG_NVS_SAMPLE_SLEEP_TIME);
		if (reboot_counter < CONFIG_NVS_SAMPLE_MAX_REBOOT) {
			if (cnt == CONFIG_NVS_SAMPLE_REBOOT_COUNTDOWN) {
				printk("Reboot counter history: ");
				while (1) {
					rc = nvs_read_hist(
						&fs, RBT_CNT_ID,
						&reboot_counter_his,
						sizeof(reboot_counter_his),
						cnt_his);
					if (rc < 0) {
						break;
					}
					printk("...%d", reboot_counter_his);
					cnt_his++;
				}
				if (cnt_his == 0) {
					printk("\n Error, no Reboot counter");
				} else {
					printk("\nOldest reboot counter: %d",
					       reboot_counter_his);
				}
				printk("\nRebooting in ");
			}
			printk("...%d", cnt);
			cnt--;
			if (cnt == 0) {
				printk("\n");
				reboot_counter++;
				(void)nvs_write(
					&fs, RBT_CNT_ID, &reboot_counter,
					sizeof(reboot_counter));
				if (reboot_counter == CONFIG_NVS_SAMPLE_MAX_REBOOT) {
					printk("Doing last reboot...\n");
				}
				sys_reboot(0);
			}
		} else {
			printk("Reboot counter reached max value.\n");
			printk("Reset to 0 and exit test.\n");
			reboot_counter = 0U;
			(void)nvs_write(&fs, RBT_CNT_ID, &reboot_counter,
			  sizeof(reboot_counter));
			break;
		}
	}

        return 0;
}
