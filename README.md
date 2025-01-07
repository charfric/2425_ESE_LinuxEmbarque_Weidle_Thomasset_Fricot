# 2425_ESE_LinuxEmbarqué_Weidle_Thomasset_Fricot

## TP1

### Prise en main

#### Préparation de la carte SD

![tp_linux1](https://github.com/user-attachments/assets/94b4a1b8-6e84-4c01-b2e8-799141c868a2)

#### Chenillard

Dans cette partie nous avons fait un chenillard. utilisation d'un driver
On écrit directement dans un fichier la valeur 1 pour allumer la LED. 
![tp_linux1](https://github.com/charfric/2425_ESE_LinuxEmbarque_Weidle_Thomasset_Fricot/blob/main/capture/IMG_9965.gif)

Dans le deuxième on écrit directement dans le registre de la LED pour l'allumer. 
![tp_linux1](capture/Chenillard_2.gif)

## TP2

### 2.1 Accès aux registres
On parle de la mémoire virtuelle, séance 3 de linux, on sait qu'il y a des processus de virtualisation de mémoire, cela permet d'avoir des endroits plus sécurisés car nous n'avons pas directement accès à l'adresse. Il y a cependant des limitations techniques.

La mémoire virtuelle permet :
  - D’utiliser de la mémoire de masse comme extension de la mémoire vive ;
  - Limiter la fragmentation ;
  - De mettre en place des mécanismes de protection de la mémoire ;
  - De partager la mémoire entre processus

#### Chenillard
Pour le chenillard, nous dévons écrire dans une adresse physique et pour cela nous accédons à un périphérique mappé en mémoire avec un appel à la primitive mmap().

## 2.2 Compilation de module noyau sur la VM

En utilisant les fichiers Makefile et hello.c, nous allons créer un module noyau sous amd pour linux, nous utilisons ensuite la commande make pour build le module en fonction du hello.c. Après, nous utilisons sudo insmod hello.ko, pour insérer le module hello créé parmi les modules dispo sur linux.

Pour tester votre module, nous avons utilisé modinfo, lsmod, insmod et rmmod (à utiliser avec sudo) :

![tp_linux14](https://github.com/user-attachments/assets/b9798ba9-0aac-4699-93d0-3c0179cff91a)

insmod : insérer le module sans vérification des dépendances
rmmod : décharger le module
lsmod : lister les modules chargés
modinfo : afficher des infos sur le module
dmesg : afficher les messages émis par les modules

Pour accéder à du matériel, nous devons créer un module, c'est-à-dire écrire un ensemble de fonctions. L'utilisateur pourra ainsi accéder au driver, en écrivant ou en lisant un fichier (cela lui permet d'utiliser les fonctions du module). 

![image](https://github.com/user-attachments/assets/2f23f470-79db-4212-820a-6c314add1228)

- création d’un entrée dans /proc
explication du code .c

- utilisation d’un timer

![image](https://github.com/user-attachments/assets/ad1dc1d8-4ba3-4368-9684-3a3005b3eb26)


## 2.3 CrossCompilation de modules noyau

À cause de la puissance limitée du processeur de la carte cible, la compilation, en particulier la compilation de modules noyau, est relativement longue. Nous allons donc, une fois encore, cross-compiler les modules noyau pour la carte SoC, à l’aide de la VM. 

module = bout de noyau qu'on ajoute. Pour compiler un module pour un noyau, il faut qu'il soit sur la bonne version du noyau pour cela nous devons récupérer les sources du noyau actuellement en fonctionnement sur la carte VEEK.

La commande **git checkout** permet de récupérer la version 6b20a2929d54 du github et ce fixer à cette version.
La commande **git config** peremt de lire ou écrire des configuration dans le git. La commande **core.abbrev 7** permet d'uniformiser les hash en les habrégeant à 7 caractères.

### Récupération de la configuration actuelle du noyau

Depuis le dossier ~/linux-socfpga/, nous lançons les commandes suivantes :
  - export CROSS_COMPILE=<chemin_arm-linux-gnueabihf->
    → cette ligne définit la variable CROSS_COMPILE, utilisée par make pour indiquer quel outil de compilation croisée utiliser.
  - export ARCH=arm
    → cette ligne définit l'architecture cible comme étant arm. 
  
Le <chemin_arm-linux-gnueabihf> est le chemin noté plus haut sans le gcc final. Par exemple : /usr/bin/arm-linux gnueabihf-

Lorsque nous définissons CROSS_COMPILE=/usr/bin/arm-linux-gnueabihf-, les scripts de compilation peuvent automatiquement ajouter le suffixe correspondant (comme gcc, as, etc.) pour appeler le bon outil.

### Hello World
