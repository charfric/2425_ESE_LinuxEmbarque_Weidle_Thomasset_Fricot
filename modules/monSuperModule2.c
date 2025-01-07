#include <linux/module.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>

#define TIMER_INTERVAL 2000 // Intervalle en millisecondes

static struct timer_list my_timer;

// Fonction appelée lorsque le timer expire
static void timer_callback(struct timer_list *timer) {
    printk(KERN_INFO "Timer expired: %lu jiffies\n", jiffies);

    // Redémarrage du timer
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(TIMER_INTERVAL));
}

// Fonction appelée lors du chargement du module
static int __init timer_module_init(void) {
    printk(KERN_INFO "Timer module loaded\n");

    // Initialisation du timer
    timer_setup(&my_timer, timer_callback, 0);

    // Démarrage du timer
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(TIMER_INTERVAL));

    return 0;
}

// Fonction appelée lors du déchargement du module
static void __exit timer_module_exit(void) {
    del_timer(&my_timer);
    printk(KERN_INFO "Timer module unloaded\n");
}

module_init(timer_module_init);
module_exit(timer_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Toi");
MODULE_DESCRIPTION("Module Linux simple avec un timer");