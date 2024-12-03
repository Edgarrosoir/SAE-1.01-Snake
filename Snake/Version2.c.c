#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

// Définition de la taille du serpent
#define TAILLE_SERPENT 10
#define PAUSE 500000
#define LARGEUR_ECRAN 40
#define HAUTEUR_ECRAN 20
#define TETE 'O'
#define CORPS 'X'
#define GAUCHE 'q'
#define DROITE 'd'
#define HAUT 'z'
#define BAS 's'
#define POSITION_INITIALE_X 21
#define POSITION_INITIALE_Y 20

// Fonction pour déplacer le curseur à une position donnée (x, y)
void gotoXY(int x, int y) { 
    printf("\033[%d;%df", y, x);
}

// Fonction pour afficher un caractère à une position (x, y)
void afficher(int x, int y, char c) {
    gotoXY(x, y);
    printf("%c", c);
}

// Fonction pour effacer un caractère en affichant un espace à la position (x, y)
void effacer(int x, int y) {
    gotoXY(x, y);
    printf(" ");
}

// Fonction qui dessine le serpent à partir des tableaux de coordonnées
void dessinerSerpent(int lesX[], int lesY[]) {
    for (int  = 1; boucle < LONGUEUR_SERPENT; boucle = boucle + 1) {
        afficher(lesX[boucle], lesY[boucle], CORPS);
    }
    afficher(lesX[0], lesY[0], TETE); // La tête du serpent
}

// Fonction pour faire progresser le serpent (
void progresser(int lesX[], int lesY[]) {
    for (int boucle = 0; boucle < TAILLE; boucle = boucle + 1) {
        lesX[boucle] = lesX[boucle] + 1;
    }
    switch (direction) {
        case GAUCHE:
            lesX[0]--;
            break;
        case DROITE:
            lesX[0]++;
            break;
        case HAUT:
            lesY[0]--;
            break;
        case BAS:
            lesY[0]++;
            break;
    }
}

// Fonction pour vérifier si une touche a été pressée
int kbhit() {
    // La fonction retourne :
    // 1 si un caractère est présent
    // 0 si pas de caractère présent
    
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
    return unCaractere;
}

int main() {
     int lesX[TAILLE_SERPENT], lesY[TAILLE_SERPENT];
    char direction = DROITE; // Direction initiale vers la droite
    
    int lesX[10], lesY[10];
    printf("X et Y ?\n");
    printf("les x: ");
    scanf("%d", &lesX[0]);
    if (lesX[0] >LARGEUR_ECRAN || lesX[0] < HAUTEUR_ECRAN) {
        printf(" chiffre entre 1 et 39, les x: ");
        scanf("%d", &lesX[0]);
    }
    printf("les y: ");
    scanf("%d", &lesY[0]);
    if (lesY[0] >LARGEUR_ECRAN || lesY[0] <HAUTEUR_ECRAN) {
        printf("chiffre entre 1 et 39, les y: ");
        scanf("%d", &lesY[0]);
    }
    for (int boucle = 1; boucle < TAILLE; boucle = boucle + 1) {
        lesX[boucle] = lesX[boucle - 1] - 1;
        lesY[boucle] = lesY[boucle - 1];
    }
    
    disableEcho();
    
    while (1) { // Boucle infinie
        if (kbhit()) {
            char ch = getchar();
            if (ch == ARRET) {
                break; // Sortir de la boucle si la touche 'a' est pressée
            }
        }

        if (kbhit()) {
            char nouvelleDirection = getchar();
            // Vérifier si la nouvelle direction est valide et ne provoque pas un demi-tour instantané
            if (nouvelleDirection == GAUCHE && direction != DROITE ||
                nouvelleDirection == DROITE && direction != GAUCHE ||
                nouvelleDirection == HAUT && direction != BAS ||
                nouvelleDirection == BAS && direction != HAUT) {
                direction = nouvelleDirection;
            }
        }

        dessinerSerpent(lesX, lesY);
        progresser(lesX, lesY);
        usleep(PAUSE); // Pause de 5 secondes
        system("clear"); // Effacer l'écran (utiliser "clear" sur Unix)
    }
    enableEcho();
}