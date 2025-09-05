#include <zephyr/kernel.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/settings/settings.h>
#include <zephyr/sys/printk.h>
#include <inttypes.h>

static uint8_t foo_val = 0;
static bool foo_val_loaded = false;

static int foo_settings_set(const char *name, size_t len, settings_read_cb read_cb, void *cb_arg)
{
        const char *next;
        int rc;

        if (settings_name_steq(name, "bar", &next) && !next)
        {
                if (len != sizeof(foo_val))
                {
                        return -EINVAL;
                }

                rc = read_cb(cb_arg, &foo_val, sizeof(foo_val));
                if (rc >= 0)
                {
                        foo_val_loaded = true;
                        return 0;
                }

                return rc;
        }

        return -ENOENT;
}

static void example_delete(void)
{
        int rc;
        printk("Delete a key-value pair\n");

        printk("delete <foo/bar>: ");
        rc = settings_delete("foo/bar");
        if (rc)
        {
                printk("fail (err %d)\n", rc);
        }
        else
        {
                printk("OK.\n\n");
        }
}

struct settings_handler my_conf = {
    .name = "foo",
    .h_set = foo_settings_set};

int main(void)
{
        printk("\nSettings demo rebooted\n");
        settings_subsys_init();
        settings_register(&my_conf);
        foo_val_loaded = false;  // Reset the flag before loading
        //settings_load();
        if (settings_load() != 0) {
                printk("Settings load failed\n");
        } else if (foo_val_loaded) {
                printk("Read from settings foo: %d\n", foo_val);
        } else {
                printk("No settings found, using default foo: %d\n", foo_val);
        }

        if (foo_val > 10)
        {
                printk("foo_val is %d so it will rewritten 0\n", foo_val);
                foo_val = 0;
                example_delete();
                k_sleep(K_SECONDS(1));
                
        }
        else
        {
                foo_val++;
                settings_save_one("foo/bar", &foo_val, sizeof(foo_val));
                printk("foo: %d\n\n", foo_val);
        }

        k_sleep(K_SECONDS(2));
        sys_reboot(SYS_REBOOT_COLD);
}