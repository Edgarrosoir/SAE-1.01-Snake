/**
 * @file <version3.c>
 *
 * @brief <programme jeu snake>
 *
 * @author <Rodrigues Moreira Edgar>
 *
 * @version <version 3>
 *
 * @date <11/11/2024>
 *
 * <Le serpent se deplace dans un plateu creer via le caractèrre #, il peut se déplacer de haut en bas et de droite vers la gauche 
 .Des pavés apparaissent aléatoirement sur le plateau et lors d'une collision entre 1 de ces pavés ou un rebord ,le jeu s'arrête. >
 */

//Bibliothèques
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <time.h>

//Déclaration des constantes
#define TAILLE_SERPENT 10 //Taille du serpent 
#define TEMPS 100000 //Temporisation
#define POSITION_INITIALE_X 40 //Position iniital du serpent en X
#define POSITION_INITIALE_Y 20// Position initial du serpent en Y
#define LARGEUR_PLATEAU 80 //Largeur du plateau
#define HAUTEUR_PLATEAU 40 // Hauteur du plateau
#define NB_PAVES 4 //Nombre de pavés présent sur le plateau
#define TPAVE 5 //Taille des pavés
#define TETE_SERPENT 'O' //Représente la tête du serpent
#define CORPS_SERPENT 'X' // Représnete le corps du serpent
#define ARRET 'a'// Touch qui arrête le jeu
#define HAUT 'z'// Déplacement vers le haut
#define BAS 's' //Déplacement vers le bas
#define GAUCHE 'q' //Déplacement vers la gauche
#define DROITE 'd' //Déplacement vers la droite
#define PAVE '#' // Ce qui définnie ce qu'est un pavé
#define CONTENU_PLATEAU ' '// intérieur du plateau

// Déclaration des procédures
void gotoXY(int x, int y); // Positionner le curseur à un endroit précis
void afficher(int x, int y, char c); //Afficher le caractère c à la position (x, y)
void effacer(int x, int y); //Afficher un espace à la position (x, y)
void dessinerSerpent(int lesX[], int lesY[]); //Afficher à l’écran un à un les éléments du serpent dont les coordonnées sont fournies dans le tableau en paramètre
void progresser(int lesX[], int lesY[], char direction, int plateau[LARGEUR_PLATEAU][HAUTEUR_PLATEAU], int *collision); //Calcule et affiche la prochaine position du serpent
void initPlateau(int plateau[LARGEUR_PLATEAU][HAUTEUR_PLATEAU]); // Initialiser le plateau de jeu
void dessinerPlateau(int plateau[LARGEUR_PLATEAU][HAUTEUR_PLATEAU]); // Afficher le plateau de jeu sur l'écran préalablement effacé
void disableEcho(); //Éviter que les caractères utilisés pour diriger le serpent s’affichent à l’écran
void enableEcho(); //Réactiver l'écho
int kbhit(); //Vérifier s’il y a eu frappe d’un caractère au clavier

// Fonction principale
int main() {

    int i, collision = 0;
    int tabX[TAILLE_SERPENT], tabY[TAILLE_SERPENT];
    int plateau[LARGEUR_PLATEAU][HAUTEUR_PLATEAU];
    char presse = 0;
    char direction = DROITE; // Initialisation de la direction à droite

    // Effacer l'écran
    system("clear");

    // Initialisation de l'aléatoire
    srand(time(NULL));

    // Initialisation du plateau
    initPlateau(plateau);

    // Position initiale du serpent (tête en (40,20), corps à gauche)
    for (i = 0; i < TAILLE_SERPENT; i++) {
        tabX[i] = POSITION_INITIALE_X - i;
        tabY[i] = POSITION_INITIALE_Y;
    }

    // Désactiver l'écho pour ne pas afficher les touches de direction
    disableEcho();

    // Boucle de mouvement du serpent
    while (presse != ARRET && !collision) {

        // Afficher le plateau
        dessinerPlateau(plateau);

        // Dessiner le serpent
        dessinerSerpent(tabX, tabY);

        //Afficher un espace à la position (x, y)
        effacer(tabX[TAILLE_SERPENT - 1], tabY[TAILLE_SERPENT - 1]);

        // Faire avancer le serpent
        progresser(tabX, tabY, direction, plateau, &collision);

        // Temps en microsecondes
        usleep(TEMPS);

        // Vérifier si l'utilisateur a appuyé sur une touche
        if (kbhit() == 1) {

            presse = getchar();

            if ((presse == HAUT && direction != BAS) || (presse == BAS && direction != HAUT) || (presse == GAUCHE && direction != DROITE) || (presse == DROITE && direction != GAUCHE)) {
                direction = presse;
            }
        }
    }

    // Réactiver l'écho après la fin du jeu
    enableEcho();

    // Effacer l'écran
    system("clear");

    return EXIT_SUCCESS;
}

// Positionne le curseur aux coordonnées (x, y)
void gotoXY(int x, int y) {

    printf("\033[%d;%df", y, x);

}


// Affiche le caractère c à la position (x, y)
void afficher(int x, int y, char c) {

    if ((x >= 0) && (x < LARGEUR_PLATEAU) && (y >= 0) && (y < HAUTEUR_PLATEAU)) {
        gotoXY(x + 1, y + 1);
        printf("%c", c);
    }

}

// Efface le caractère a la position (x, y)
void effacer(int x, int y) {

    afficher(x, y, CONTENU_PLATEAU);

}

// Dessine le serpent à partir des tableaux de coordonnées
void dessinerSerpent(int lesX[], int lesY[]) {

    int i;

    for (i = 0; i < TAILLE_SERPENT; i++) {

        if (i == 0) {
            afficher(lesX[i], lesY[i], TETE_SERPENT); // Affiche la tête du serpent
        }

        else {
            afficher(lesX[i], lesY[i], CORPS_SERPENT); // Affiche le corps du serpent
        }
        
    }

    fflush(stdout);

}

// Fait avancer le serpent dans la direction donnée
void progresser(int lesX[], int lesY[], char direction, int plateau[LARGEUR_PLATEAU][HAUTEUR_PLATEAU], int *collision) {
    
    int i;

    // Vérifie si la tête va toucher le corps
    for (i = 1; i < TAILLE_SERPENT; i++) {

        if (lesX[0] == lesX[i] && lesY[0] == lesY[i]) {
            *collision = 1;
            return;
        }

    }

    // Déplace chaque partie du serpent
    for (i = TAILLE_SERPENT - 1; i > 0; i--) {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }

    // Déplace la tête selon la direction
    switch (direction) {

        case HAUT: lesY[0]--; // Haut
        break;
        case BAS: lesY[0]++; // Bas
        break;
        case GAUCHE: lesX[0]--; // Gauche
        break;
        case DROITE: lesX[0]++; // Droite
        break;

    }

    // Vérifie les collisions avec le plateau ou un pavé
    if (plateau[lesX[0]][lesY[0]] == PAVE) {

        *collision = 1; // Si une collision est détectée, le jeu se termine

    }
}

// Initialiser le plateau de jeu
void initPlateau(int plateau[LARGEUR_PLATEAU][HAUTEUR_PLATEAU]) {

    int i, j, x, y, k, l;

    // Initialiser tout le tableau avec des espaces
    for (i = 0; i < HAUTEUR_PLATEAU; i++) {

        for (j = 0; j < LARGEUR_PLATEAU; j++) {
            plateau[j][i] = CONTENU_PLATEAU;
        }

    }

    // Placer les bordures
    for (i = 0; i < LARGEUR_PLATEAU; i++) {

        plateau[i][0] = PAVE;
        plateau[i][HAUTEUR_PLATEAU - 1] = PAVE;

    }

    for (i = 0; i < HAUTEUR_PLATEAU; i++) {

        plateau[0][i] = PAVE;
        plateau[LARGEUR_PLATEAU - 1][i] = PAVE;

    }

    // Placer des pavés aléatoires (4 pavés dans le plateau)
    for (int p = 0; p < NB_PAVES; p++) {

        do { // Générer des positions aléatoires pour les pavés
            x = rand() % (LARGEUR_PLATEAU - TAILLE_SERPENT) + TPAVE;
            y = rand() % (HAUTEUR_PLATEAU - TAILLE_SERPENT) + TPAVE;
        } while ((x >= POSITION_INITIALE_X - TAILLE_SERPENT && x < POSITION_INITIALE_X + TAILLE_SERPENT && y >= POSITION_INITIALE_Y - TAILLE_SERPENT && y < POSITION_INITIALE_Y + TAILLE_SERPENT)); // Empêcher les pavés d'apparaître sur la position initiale du serpent

        // Une fois que la position est valide, placer le pavé
        for (k = 0; k < TPAVE; k++) {

            for (l = 0; l < TPAVE; l++) {
                plateau[x + k][y + l] = PAVE;
            }
        }
    }
}

// Afficher le plateau de jeu sur l'écran préalablement effacé
void dessinerPlateau(int plateau[LARGEUR_PLATEAU][HAUTEUR_PLATEAU]) {

    int i, j;

    for (i = 0; i < HAUTEUR_PLATEAU; i++) {

        for (j = 0; j < LARGEUR_PLATEAU; j++) {
            gotoXY(j + 1, i + 1);
            putchar(plateau[j][i]);
        }
    }
}

void disableEcho() {
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Desactiver le flag ECHO
    tty.c_lflag &= ~ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

void enableEcho() {
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Reactiver le flag ECHO
    tty.c_lflag |= ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

// Vérifie si une touche a été appuyée
int kbhit() {

    // La fonction retourne :
	// 1 si un caractere est present
	// 0 si pas de caractere present

    int unCaractere = 0;
    struct termios oldt, newt;
    int ch;
    int oldf;

    // Mettre le terminal en mode non bloquant
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    // Restaurer le mode du terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        unCaractere = 1;
    }

    // Retour à la ligne à la fin du programme
    printf("\n");

    return unCaractere;
}