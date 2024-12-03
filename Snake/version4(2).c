/**
 * @file <version3.c>
 *
 * @brief <programme jeu snake>
 *
 * @author <Rodrigues Moreira Edgar>
 *
 * @version <version 4>
 *
 * @date <22/11/2024>
 *
 * <Le serpent se déplace dans un plateau créé via le caractère #, il peut se déplacer de haut en bas et de droite vers la gauche.
 * Des pavés apparaissent aléatoirement sur le plateau, et lors d'une collision avec l'un de ces pavés ou un bord, le jeu s'arrête.
 * De plus, des pommes apparaissent sous la forme de '6'. Si l'une d'entre elles est mangée, le serpent grandit, gagne en vitesse et une autre pomme apparaît.
 * Des issues sont apparues au milieu de chaque bordure. Si l'une d'entre elles est prise, le serpent réapparaît dans l'issue opposée.
 * Si le serpent atteint une taille de 20, alors le jeu s'arrête. >
 */

// Bibliothèques
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

// Déclaration des constantes
#define TAILLE_SERPENT 10          // Taille initiale du serpent
#define TEMPS 100000               // Temporisation (vitesse du jeu)
#define POSITION_INITIALE_X 40    // Position initiale du serpent en X
#define POSITION_INITIALE_Y 20    // Position initiale du serpent en Y
#define LARGEUR_PLATEAU 80        // Largeur du plateau de jeu
#define HAUTEUR_PLATEAU 40        // Hauteur du plateau de jeu
#define NB_PAVES 4                // Nombre de pavés dans le plateau
#define TPAVE 5                   // Taille des pavés
#define TETE_SERPENT 'O'          // Caractère représentant la tête du serpent
#define CORPS_SERPENT 'X'         // Caractère représentant le corps du serpent
#define ARRET 'a'                 // Touche pour arrêter le jeu
#define HAUT 'z'                  // Déplacement vers le haut
#define BAS 's'                   // Déplacement vers le bas
#define GAUCHE 'q'                // Déplacement vers la gauche
#define DROITE 'd'                // Déplacement vers la droite
#define PAVE '#'                  // Représente les pavés
#define CONTENU_PLATEAU ' '      // Représente l'intérieur du plateau
#define POMME '6'                 // Représente la pomme
#define TAIILE_MAX 20            // Taille maximale du serpent (gagne à cette taille)
#define ISSUE ' '                 // Représente une issue (zone de réapparition)

// Déclaration des procédures
void gotoXY(int x, int y); // Positionner le curseur à un endroit précis
void afficher(int x, int y, char c); // Afficher le caractère c à la position (x, y)
void effacer(int x, int y); // Effacer l'affichage à la position (x, y)
void dessinerSerpent(int lesX[], int lesY[]); // Dessiner le serpent
void progresser(int lesX[], int lesY[], char direction, int plateau[LARGEUR_PLATEAU][HAUTEUR_PLATEAU], int *collision, bool *pommeMangee); // Calculer et afficher la prochaine position du serpent
void initPlateau(int plateau[LARGEUR_PLATEAU][HAUTEUR_PLATEAU]); // Initialiser le plateau de jeu
void dessinerPlateau(int plateau[LARGEUR_PLATEAU][HAUTEUR_PLATEAU]); // Afficher le plateau de jeu
void ajouterPomme(int plateau[LARGEUR_PLATEAU][HAUTEUR_PLATEAU]); // Ajouter une pomme sur le plateau
void disableEcho(); // Désactiver l'affichage des touches pressées
void enableEcho(); // Réactiver l'affichage des touches pressées
int kbhit(); // Vérifier si une touche a été pressée

// Fonction principale du jeu
int main() {
    int i, collision = 0;
    int tabX[TAILLE_SERPENT * 2], tabY[TAILLE_SERPENT * 2]; // Permet de gérer une taille de serpent plus grande
    int plateau[LARGEUR_PLATEAU][HAUTEUR_PLATEAU]; // Plateau de jeu
    char presse = 0;
    char direction = DROITE; // Direction initiale du serpent
    int tailleSerpent = TAILLE_SERPENT; // Longueur initiale du serpent
    int vitesse = TEMPS; // Vitesse du jeu
    bool pommeMangee = false; // Indicateur si la pomme a été mangée

    // Effacer l'écran au début du jeu
    system("clear");

    // Initialisation de la graine pour la génération de nombres aléatoires
    srand(time(NULL));

    // Initialisation du plateau
    initPlateau(plateau);

    // Ajoute une pomme sur le plateau
    ajouterPomme(plateau);

    // Position initiale du serpent (tête en (40,20), corps à gauche)
    for (i = 0; i < tailleSerpent; i++) {
        tabX[i] = POSITION_INITIALE_X - i;
        tabY[i] = POSITION_INITIALE_Y;
    }

    // Désactiver l'écho pour ne pas afficher les touches de direction
    disableEcho();

    // Boucle principale du jeu
    while (presse != ARRET && !collision) {

        // Afficher le plateau avec les éléments
        dessinerPlateau(plateau);

        // Dessiner le serpent
        dessinerSerpent(tabX, tabY);

        // Effacer l'ancienne position de la queue
        effacer(tabX[tailleSerpent - 1], tabY[tailleSerpent - 1]);

        // Faire avancer le serpent et vérifier s'il mange une pomme
        progresser(tabX, tabY, direction, plateau, &collision, &pommeMangee);

        // Si une pomme est mangée, augmenter la taille du serpent et accélérer le jeu
        if (pommeMangee) {
            tailleSerpent++; // Augmenter la taille du serpent

            // Ajouter un nouveau segment à la fin du serpent (dans la même direction que la tête)
            tabX[tailleSerpent - 1] = tabX[tailleSerpent - 2]; // Le nouvel élément suit la direction
            tabY[tailleSerpent - 1] = tabY[tailleSerpent - 2];

            // Ajuster la vitesse du jeu (accélère)
            vitesse -= 5000;
        }

        // Vérifier si la taille du serpent a atteint 20 (victoire)
        if (tailleSerpent >= TAIILE_MAX) {
            printf("\nVictoire ! Vous avez atteint la taille maximale de 20. Bravo !\n");
            break; // Sortir de la boucle et terminer le jeu
        }

        // Temps en microsecondes (contrôle de la vitesse du jeu)
        usleep(vitesse);

        // Vérifier si l'utilisateur a appuyé sur une touche
        if (kbhit() == 1) {
            presse = getchar(); // Lire la touche appuyée
            // Vérifier si la direction choisie est opposée à la direction actuelle
            if ((presse == HAUT && direction != BAS) || 
                (presse == BAS && direction != HAUT) || 
                (presse == GAUCHE && direction != DROITE) || 
                (presse == DROITE && direction != GAUCHE)) {
                direction = presse; // Mettre à jour la direction du serpent
            }
        }
    }

    // Réactiver l'écho après la fin du jeu
    enableEcho();

    // Effacer l'écran après la fin du jeu
    system("clear");

    return EXIT_SUCCESS;
}

// Positionner le curseur à un endroit précis
void gotoXY(int x, int y) {
    printf("\033[%d;%df", y, x); // Utilisation de séquence ANSI pour positionner le curseur
}

// Afficher le caractère c à la position (x, y)
void afficher(int x, int y, char c) {
    if ((x >= 0) && (x < LARGEUR_PLATEAU) && (y >= 0) && (y < HAUTEUR_PLATEAU)) {
        gotoXY(x + 1, y + 1); // Positionner le curseur avant d'afficher
        printf("%c", c); // Afficher le caractère
    }
}

// Effacer le caractère à la position (x, y)
void effacer(int x, int y) {
    afficher(x, y, CONTENU_PLATEAU); // Afficher un espace pour effacer l'ancien caractère
}

// Dessiner le serpent à partir des tableaux de coordonnées
void dessinerSerpent(int lesX[], int lesY[]) {
    int i;
    for (i = 0; i < TAILLE_SERPENT; i++) {
        if (i == 0) {
            afficher(lesX[i], lesY[i], TETE_SERPENT); // Afficher la tête du serpent
        } else {
            afficher(lesX[i], lesY[i], CORPS_SERPENT); // Afficher le corps du serpent
        }
    }
    fflush(stdout); // Forcer l'affichage immédiatement
}

// Progresser la position du serpent et vérifier les collisions
void progresser(int lesX[], int lesY[], char direction, int plateau[LARGEUR_PLATEAU][HAUTEUR_PLATEAU], int *collision, bool *pommeMangee) {
    int i;
    // Vérifier si la tête du serpent touche son propre corps
    for (i = 1; i < TAILLE_SERPENT; i++) {
        if (lesX[0] == lesX[i] && lesY[0] == lesY[i]) {
            *collision = 1; // Si collision avec le corps, fin du jeu
            return;
        }
    }

    // Déplacer chaque partie du serpent
    for (i = TAILLE_SERPENT - 1; i > 0; i--) {
        lesX[i] = lesX[i - 1]; 
        lesY[i] = lesY[i - 1]; 
    }

    // Déplacer la tête selon la direction
    switch (direction) {
        case HAUT: lesY[0]--; break;
        case BAS: lesY[0]++; break;
        case GAUCHE: lesX[0]--; break;
        case DROITE: lesX[0]++; break;
    }

    // Gestion des issues : réapparaître de l'autre côté
    if (lesX[0] == 0 && lesY[0] == HAUTEUR_PLATEAU / 2) {
        lesX[0] = LARGEUR_PLATEAU - 2;
    } else if (lesX[0] == LARGEUR_PLATEAU - 1 && lesY[0] == HAUTEUR_PLATEAU / 2) {
        lesX[0] = 1;
    } else if (lesY[0] == 0 && lesX[0] == LARGEUR_PLATEAU / 2) {
        lesY[0] = HAUTEUR_PLATEAU - 2;
    } else if (lesY[0] == HAUTEUR_PLATEAU - 1 && lesX[0] == LARGEUR_PLATEAU / 2) {
        lesY[0] = 1;
    }

    // Vérifier si le serpent mange une pomme
    if (plateau[lesX[0]][lesY[0]] == POMME) {
        *pommeMangee = true; // Indiquer que la pomme a été mangée
        plateau[lesX[0]][lesY[0]] = CONTENU_PLATEAU; // Supprimer la pomme
        ajouterPomme(plateau); // Ajouter une nouvelle pomme
    } else {
        *pommeMangee = false;
    }

    // Vérifier les collisions avec les pavés
    if (plateau[lesX[0]][lesY[0]] == PAVE) {
        *collision = 1; // Fin du jeu si collision avec un pavé
    }
}

// Initialiser le plateau de jeu avec les bordures, pavés et issues
void initPlateau(int plateau[LARGEUR_PLATEAU][HAUTEUR_PLATEAU]) {
    int i, j, x, y, k, l;

    // Initialiser tout le plateau avec des espaces
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

    // Ajouter des issues au centre de chaque côté
    plateau[LARGEUR_PLATEAU / 2][0] = ISSUE; // Issue en haut
    plateau[LARGEUR_PLATEAU / 2][HAUTEUR_PLATEAU - 1] = ISSUE; // Issue en bas
    plateau[0][HAUTEUR_PLATEAU / 2] = ISSUE; // Issue à gauche
    plateau[LARGEUR_PLATEAU - 1][HAUTEUR_PLATEAU / 2] = ISSUE; // Issue à droite

    // Placer des pavés aléatoires
    for (int p = 0; p < NB_PAVES; p++) {
        do {
            x = rand() % (LARGEUR_PLATEAU - TPAVE); // Générer position aléatoire pour les pavés
            y = rand() % (HAUTEUR_PLATEAU - TPAVE);
        } while (plateau[x][y] != CONTENU_PLATEAU); // Vérifier que la case est libre

        // Placer un pavé
        for (k = 0; k < TPAVE; k++) {
            for (l = 0; l < TPAVE; l++) {
                plateau[x + k][y + l] = PAVE;
            }
        }
    }
}

// Afficher le plateau de jeu sur l'écran
void dessinerPlateau(int plateau[LARGEUR_PLATEAU][HAUTEUR_PLATEAU]) {
    int i, j;
    for (i = 0; i < HAUTEUR_PLATEAU; i++) {
        for (j = 0; j < LARGEUR_PLATEAU; j++) {
            gotoXY(j + 1, i + 1); // Placer le curseur à la position (x, y)
            putchar(plateau[j][i]); // Afficher le caractère
        }
    }
}

// Ajouter une pomme sur une position libre du plateau
void ajouterPomme(int plateau[LARGEUR_PLATEAU][HAUTEUR_PLATEAU]) {
    int x, y;
    do {
        // Générer une position aléatoire dans les limites du plateau
        x = rand() % (LARGEUR_PLATEAU - 2) + 1; 
        y = rand() % (HAUTEUR_PLATEAU - 2) + 1; 
    } while (plateau[x][y] != CONTENU_PLATEAU); // Vérifier que la case est libre

    // Placer la pomme sur le plateau
    plateau[x][y] = POMME;
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