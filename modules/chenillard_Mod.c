#include <linux/module.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#define PROC_DIR "ensea"
#define PROC_FILE "chenille"
#define NUM_LEDS 4

static struct timer_list chenillard_timer;
static int chenille_pattern = 0x01; // Pattern par défaut : LED 1 allumée
static int speed = 500;             // Vitesse par défaut : 500 ms
static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_file;

// Fonction de mise à jour du chenillard
static void chenillard_callback(unsigned long data) {
    static int shift = 0;
    int display = (chenille_pattern << shift) & ((1 << NUM_LEDS) - 1);

    printk(KERN_INFO "Chenillard pattern: 0x%X\n", display);

    // Déplacer le pattern vers la gauche
    shift = (shift + 1) % NUM_LEDS;

    // Redémarrer le timer
    mod_timer(&chenillard_timer, jiffies + msecs_to_jiffies(speed));
}

// Fonction de lecture dans /proc/ensea/chenille
static ssize_t proc_read(struct file *file, char __user *user_buffer, size_t count, loff_t *pos) {
    char buffer[64];
    int len = snprintf(buffer, sizeof(buffer), "Pattern: 0x%X, Speed: %d ms\n", chenille_pattern, speed);

    if (*pos > 0 || count < len)
        return 0;

    if (copy_to_user(user_buffer, buffer, len))
        return -EFAULT;

    *pos = len;
    return len;
}

// Fonction d'écriture dans /proc/ensea/chenille
static ssize_t proc_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *pos) {
    char buffer[64];

    if (count > sizeof(buffer) - 1)
        return -EINVAL;

    if (copy_from_user(buffer, user_buffer, count))
        return -EFAULT;

    buffer[count] = '\0';

    // Extraire le pattern et la vitesse depuis l'entrée utilisateur
    sscanf(buffer, "%x %d", &chenille_pattern, &speed);
    printk(KERN_INFO "Nouveau pattern: 0x%X, Nouvelle vitesse: %d ms\n", chenille_pattern, speed);

    return count;
}

// Structure des opérations sur le fichier /proc
static const struct file_operations proc_file_ops = {
    .read = proc_read,
    .write = proc_write,
};

// Fonction appelée lors du chargement du module
static int __init chenillard_module_init(void) {
    printk(KERN_INFO "Chenillard module loaded\n");

    // Créer le répertoire et le fichier /proc
    proc_dir = proc_mkdir(PROC_DIR, NULL);
    if (!proc_dir) {
        printk(KERN_ALERT "Failed to create /proc/%s\n", PROC_DIR);
        return -ENOMEM;
    }

    proc_file = proc_create(PROC_FILE, 0666, proc_dir, &proc_file_ops);
    if (!proc_file) {
        printk(KERN_ALERT "Failed to create /proc/%s/%s\n", PROC_DIR, PROC_FILE);
        remove_proc_entry(PROC_DIR, NULL);
        return -ENOMEM;
    }

    // Initialisation du timer
    init_timer(&chenillard_timer);
    chenillard_timer.function = chenillard_callback;
    chenillard_timer.data = 0;
    chenillard_timer.expires = jiffies + msecs_to_jiffies(speed);
    add_timer(&chenillard_timer);

    return 0;
}

// Fonction appelée lors du déchargement du module
static void __exit chenillard_module_exit(void) {
    del_timer(&chenillard_timer);
    remove_proc_entry(PROC_FILE, proc_dir);
    remove_proc_entry(PROC_DIR, NULL);
    printk(KERN_INFO "Chenillard module unloaded\n");
}

module_init(chenillard_module_init);
module_exit(chenillard_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rems Weidle");
MODULE_DESCRIPTION("Module Linux pour chenillard modifiable via /proc");
