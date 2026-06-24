#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Soryu");
MODULE_DESCRIPTION("KPM Dynamic CPUInfo Spoof for K60 Pro");

#define MAX_APPS 20
#define MAX_PKG_LEN 64

static char target_pkgs[MAX_APPS][MAX_PKG_LEN];
static int app_count = 0;

static ssize_t kpm_proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *pos) {
    char buf[MAX_PKG_LEN];
    if (count >= MAX_PKG_LEN) return -EINVAL;
    if (copy_from_user(buf, buffer, count)) return -EFAULT;
    buf[count] = '\0';
    
    char *nl = strchr(buf, '\n');
    if (nl) *nl = '\0';

    if (strcmp(buf, "clear") == 0) {
        app_count = 0;
        return count;
    }

    if (app_count < MAX_APPS) {
        strncpy(target_pkgs[app_count], buf, MAX_PKG_LEN);
        app_count++;
    }
    return count;
}

static const struct proc_ops kpm_proc_ops = {
    .proc_write = kpm_proc_write,
};

static int __init kpm_init(void) {
    proc_create("ksu_kpm_apps", 0666, NULL, &kpm_proc_ops);
    return 0;
}

static void __exit kpm_exit(void) {
    remove_proc_entry("ksu_kpm_apps", NULL);
}

module_init(kpm_init);
module_exit(kpm_exit);
