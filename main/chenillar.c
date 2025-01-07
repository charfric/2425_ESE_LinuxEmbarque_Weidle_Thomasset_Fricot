#include <stdio.h>
#include <unistd.h> // Pour la fonction usleep

#define LED_COUNT 9
#define DELAY 200000 // Délai en microsecondes (200ms)

void set_led_brightness(int led, int brightness) {
    char path[50];
    FILE *file;

    // Construire le chemin du fichier
    snprintf(path, sizeof(path), "/sys/class/leds/fpga_led%d/brightness", led);

    // Ouvrir le fichier en mode écriture
    file = fopen(path, "w");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    // Écrire la valeur de luminosité
    fprintf(file, "%d", brightness);

    // Fermer le fichier
    fclose(file);
}

int main() {
    while (1) {
        for (int i = 1; i <= LED_COUNT; i++) {
            // Allumer la LED
            set_led_brightness(i, 1);

            // Pause pour laisser la LED allumée
            usleep(DELAY);

            // Éteindre la LED
            set_led_brightness(i, 0);
        }
    }

    return 0;
}
