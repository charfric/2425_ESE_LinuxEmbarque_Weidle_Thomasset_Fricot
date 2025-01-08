#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/kernel.h>
#include <linux/rtc.h> // Pour obtenir l'heure système
#include <linux/delay.h>

#define UPDATE_INTERVAL_MS 1000 // Mise à jour toutes les 1000 ms

// GPIO des segments pour chaque afficheur HEX (0 à 5)
#define GPIO_SEGMENTS { \
    {17, 18, 19, 20, 21, 22, 23}, /* HEX0 */ \
    {24, 25, 26, 27, 28, 29, 30}, /* HEX1 */ \
    {31, 32, 33, 34, 35, 36, 37}, /* HEX2 */ \
    {38, 39, 40, 41, 42, 43, 44}, /* HEX3 */ \
    {45, 46, 47, 48, 49, 50, 51}, /* HEX4 */ \
    {52, 53, 54, 55, 56, 57, 58}  /* HEX5 */ \
}

// Table de correspondance pour les segments des chiffres de 0 à 9
static const uint8_t segment_map[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

static struct timer_list display_timer;
static int gpio_segments[6][7] = GPIO_SEGMENTS;

// Prototypes
static void update_display(unsigned long data);

// Fonction pour configurer les GPIO
static int setup_gpio(void)
{
    int i, j;
    int ret;

    for (i = 0; i < 6; i++) {
        for (j = 0; j < 7; j++) {
            ret = gpio_request(gpio_segments[i][j], "hex_segment");
            if (ret) {
                pr_err("Failed to request GPIO %d\n", gpio_segments[i][j]);
                goto error;
            }

            ret = gpio_direction_output(gpio_segments[i][j], 0);
            if (ret) {
                pr_err("Failed to set GPIO %d as output\n", gpio_segments[i][j]);
                goto error;
            }
        }
    }
    return 0;

error:
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 7; j++) {
            gpio_free(gpio_segments[i][j]);
        }
    }
    return ret;
}

// Fonction pour libérer les GPIO
static void release_gpio(void)
{
    int i, j;

    for (i = 0; i < 6; i++) {
        for (j = 0; j < 7; j++) {
            gpio_free(gpio_segments[i][j]);
        }
    }
}

// Fonction pour afficher un chiffre sur un afficheur donné
static void display_digit(int hex_index, int digit)
{
    int j;
    uint8_t segments = segment_map[digit];

    for (j = 0; j < 7; j++) {
        gpio_set_value(gpio_segments[hex_index][j], (segments >> j) & 0x01);
    }
}

// Fonction de mise à jour de l'afficheur
static void update_display(unsigned long data)
{
    struct rtc_time tm;
    struct timespec64 ts;

    // Récupérer l'heure système
    ktime_get_real_ts64(&ts);
    rtc_time64_to_tm(ts.tv_sec, &tm);

    // Afficher les chiffres sur les afficheurs HEX0 à HEX5
    display_digit(0, tm.tm_sec % 10); // HEX0 : secondes unités
    display_digit(1, tm.tm_sec / 10); // HEX1 : secondes dizaines
    display_digit(2, tm.tm_min % 10); // HEX2 : minutes unités
    display_digit(3, tm.tm_min / 10); // HEX3 : minutes dizaines
    display_digit(4, tm.tm_hour % 10); // HEX4 : heures unités
    display_digit(5, tm.tm_hour / 10); // HEX5 : heures dizaines

    // Redémarrer le timer
    mod_timer(&display_timer, jiffies + msecs_to_jiffies(UPDATE_INTERVAL_MS));
}

// Fonction d'initialisation du module
static int __init hex_display_init(void)
{
    int ret;

    pr_info("Initializing hex display driver\n");

    // Configurer les GPIO
    ret = setup_gpio();
    if (ret) {
        pr_err("Failed to initialize GPIOs\n");
        return ret;
    }

    // Initialiser le timer
    init_timer(&display_timer);
    display_timer.function = update_display;
    display_timer.data = 0;
    display_timer.expires = jiffies + msecs_to_jiffies(UPDATE_INTERVAL_MS);
    add_timer(&display_timer);

    pr_info("Hex display driver initialized successfully\n");
    return 0;
}

// Fonction de nettoyage du module
static void __exit hex_display_exit(void)
{
    del_timer_sync(&display_timer);
    release_gpio();
    pr_info("Hex display driver exited\n");
}

module_init(hex_display_init);
module_exit(hex_display_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Driver for 7-segment hex display using GPIO");
