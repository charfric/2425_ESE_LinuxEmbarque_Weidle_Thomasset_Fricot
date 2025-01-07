#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define PROC_NAME "ensea_proc"
#define BUF_SIZE 128

static char *kernel_buffer;

// Fonction de lecture dans /proc
static ssize_t proc_read(struct file *file, char __user *user_buffer, size_t count, loff_t *pos) {
    int len = strlen(kernel_buffer);

    if (*pos > 0 || count < len)
        return 0;

    if (copy_to_user(user_buffer, kernel_buffer, len))
        return -EFAULT;

    *pos = len;
    return len;
}

// Fonction d'écriture dans /proc
static ssize_t proc_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *pos) {
    if (count > BUF_SIZE)
        return -EINVAL;

    memset(kernel_buffer, 0, BUF_SIZE);

    if (copy_from_user(kernel_buffer, user_buffer, count))
        return -EFAULT;

    kernel_buffer[count] = '\0';
    return count;
}

// Structure des opérations sur le fichier /proc
static const struct file_operations proc_file_ops = {
    .read = proc_read,
    .write = proc_write,
};

// Fonction appelée lors du chargement du module
static int __init ensea_proc_init(void) {
    kernel_buffer = kmalloc(BUF_SIZE, GFP_KERNEL);
    if (!kernel_buffer)
        return -ENOMEM;

    proc_create(PROC_NAME, 0666, NULL, &proc_file_ops);
    printk(KERN_INFO "ensea_proc: module loaded.\n");
    return 0;
}

// Fonction appelée lors du déchargement du module
static void __exit ensea_proc_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    kfree(kernel_buffer);
    printk(KERN_INFO "ensea_proc: module unloaded.\n");
}

module_init(ensea_proc_init);
module_exit(ensea_proc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Toi");
MODULE_DESCRIPTION("Module Linux simple avec entree /proc");
