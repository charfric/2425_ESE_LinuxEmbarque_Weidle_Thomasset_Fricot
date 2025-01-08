
# 2425_ESE_LinuxEmbarqué_Weidle_Thomasset_Fricot

## TP1

### Prise en main

#### Connection au système

Pour démarrer nous avons installer Linux sur la carte SD de la carte pour pouvoir utiliser Linux sur notre carte.
Une fois connecter au SOC nous avons observé la séquence de démarrrage.
On peut remarqué en utilisant la commande **df -h** que la mémoire de la carte n'est pas complétement utilisé. Sur les 3Go de disponible seulement 1,3Go sont utilisé.

<p align="center"><img src="https://github.com/user-attachments/assets/94b4a1b8-6e84-4c01-b2e8-799141c868a2" width="600" /> </p>

Nous avons ensuite augmenté la taille du système de fichier à l'aide la commande **./resize2fs_once**.

<p align="center"><img src="https://github.com/user-attachments/assets/0e3be882-a472-4368-92e8-a1c7872bc029" width="600" /> </p> 

#### Configuration réseau

A l'aide de la commande **ifconfig** nous avons pu vérifier que la carte disposait d'une adresse IP. Sur la figure ci-dessous on peut voir que la carte n'est pas connecté et n'a donc pas d'adresse IP.

<p align="center"><img src="https://github.com/user-attachments/assets/9cdd4a21-49e1-4f19-bd09-fe32d342d9db" width="600" /> </p> 

Après avoir connecté la carte, on peut voir sur la figure ci-dessous que celle-ci dispose bien d'une adresse IP.

<p align="center"><img src="https://github.com/user-attachments/assets/dddea9c9-47fa-4eec-b019-db1901049c01" width="600" /> </p> 

Après c'être connecté au wifi **Techtinkerers** qui est le même réseau que le câble ethernet on ping la carte pour vérifier que l'on arrive bien à ce connecter. 

<p align="center"><img src="https://github.com/user-attachments/assets/eae37a7a-3cd2-4cca-9656-0b213e1fa6fa" width="600" /> </p> 

#### Découverte de la cible


![tp_linux6](https://github.com/user-attachments/assets/d4d4c89a-a2e9-4f01-8275-ad3dd2831694)

![tp_linux7](https://github.com/user-attachments/assets/a87015db-9469-463a-9561-8dca51e9edcd)
![tp_linux8](https://github.com/user-attachments/assets/62270597-4681-49c8-9d24-7af7bef9eb56)

![tp_linux9](https://github.com/user-attachments/assets/438b30ca-ceba-4526-9b64-73df2a4a1913)

![tp_linux10](https://github.com/user-attachments/assets/f806ead5-c49b-47bd-8ba6-b93947d4783b)

![tp_linux11](https://github.com/user-attachments/assets/ba6952c4-a31f-4734-90ec-8557c500d394)

![tp_linux12](https://github.com/user-attachments/assets/9e4cce7d-2e2e-4bc7-933d-247195cbf9f4)



#### Chenillard

Dans cette partie nous avons fait un chenillard. utilisation d'un driver
On écrit directement dans un fichier la valeur 1 pour allumer la LED. 

![tp_linux13](https://github.com/user-attachments/assets/6df73298-5efe-4a97-aa9f-94d02526ff82)

![tp_linux1](https://github.com/charfric/2425_ESE_LinuxEmbarque_Weidle_Thomasset_Fricot/blob/main/capture/IMG_9965.gif)

## TP2

### 2.1 Accès aux registres
Dans ce tp, nous allons parler de la mémoire virtuelle, ce qui correspond à la séance 3 de cours. On sait qu'il y a des processus de virtualisation de mémoire, cela permet d'avoir des endroits plus sécurisés car nous n'avons pas directement accès à l'adresse. Il y a cependant des limitations techniques.

La mémoire virtuelle permet plus précisement:
  - D’utiliser de la mémoire de masse comme extension de la mémoire vive ;
  - De limiter la fragmentation ;
  - De mettre en place des mécanismes de protection de la mémoire ;
  - De partager la mémoire entre processus

#### Chenillard
Pour  réaliser un chenillard, nous devons écrire dans une adresse physique et pour cela nous accédons à un périphérique mappé en mémoire avec un appel à la primitive mmap().
On écrit directement dans le registre de la LED pour l'allumer. 
![tp_linux1](capture/Chenillard_2.gif)


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

  - Création d’un entrée dans /proc :

![tp_linux15](https://github.com/user-attachments/assets/4cfe8b2e-2ed6-4265-9660-f736014b7c0e)

A l'aide de la commande modinf, on peut vérifier que le module ensea_proc se charge bien après l'utilisation de la commande insmod.
On peut également voir que l'on peut lire le fichier ensea_proc dans le fichier /proc. On peut aussi écrire dans le fichier "bonjour ensea", ce que l'on peut vérifier en lisant le fichier. 

Nous avons donc réussi à obtenir un module avec de la lecture et de l'écriture.

  - Utilisation d’un timer

![image](https://github.com/user-attachments/assets/ad1dc1d8-4ba3-4368-9684-3a3005b3eb26)

![tp_linux16](https://github.com/user-attachments/assets/405efc8c-891d-4580-8ad1-361fef4438e3)

Tout comme le module précedent, le module du timer se charge bien. Il expire à ... jiffies, c'est-à-dire qu'il se réinitialise quand il a terminé de compter.


## 2.3 CrossCompilation de modules noyau

À cause de la puissance limitée du processeur de la carte cible, la compilation, en particulier la compilation de modules noyau, est relativement longue. Nous allons donc, une fois encore, cross-compiler les modules noyau pour la carte SoC, à l’aide de la VM. 

remarque : on ajoute le noyau de la carte au noyau de la machine virtuelle.

La compilation croisée consiste à compiler du code sur une architecture pour qu'il fonctionne sur une autre architecture (dans notre cas ARM).

La version exacte du noyau de la carte est la suivante :
![tp_linux17](https://github.com/user-attachments/assets/3b0026fa-b300-41d8-a33d-c9399433a2a5)

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

La commande de compilation du Makefile $("CROSS_COMPILE")gcc... 

### Hello World

![tp_linux18](https://github.com/user-attachments/assets/bf08fa46-ee2e-419b-9532-4bf084716820)

Cela montre qu'on a réussi à compiler au format ARM, un module pour le noyau du SoC sur la machine virtuelle au format AMD tournant avec un autre noyau.

![tp_linux19](https://github.com/user-attachments/assets/2d6b35a5-aa34-4b79-b3cb-eae7818d2ac5)

Ce fichier est le Makefile dans lequel on a ajouté CFLAGS_MODULE=-fno-pic. dans celui-ci on le KERNEL_SOURCE qui est maintenant le noyau SoC.

### Chenillard

Dans cette partie, on a créé un chenillard dont on peut modifier :
  - Le pattern depuis le fichier : /proc/ensea/chenille
  - La vitesse au moment du chargement du module.

![tp_linux20](https://github.com/user-attachments/assets/838992e9-ff51-4d3e-a78c-b9a0a1434048)

Sur les figures suivantes, on vérifie que la modification du pattern et de la vitesse est fonctionnelle :

![tp_linuxChenillard1](capture/Chenillard_2.gif)

![tp_Chenillard2](capture/Chenillard2_2.3.gif)

