#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#define LONGUEUR_SERPENT 10        // Longueur du serpent
#define LARGEUR_ECRAN 40           // Largeur de l'écran (colonne max pour le serpent)
#define HAUTEUR_ECRAN 20           // Hauteur de l'écran (ligne max pour le serpent)
#define DELAI_DEPLACEMENT 100000   // Délai en microsecondes pour le déplacement



// Fonction pour afficher un caractère à une position (x, y)
void afficher(int colonne, int ligne, char caractere) {
    gotoXY(colonne, ligne);  // Positionne le curseur à la position voulue
    printf("%c", caractere); // Affiche le caractère
    fflush(stdout);           // Vide le buffer de sortie
}

// Fonction pour effacer un caractère en affichant un espace à la position (x, y)
void effacer(int colonne, int ligne) {
    afficher(colonne, ligne, ' ');  // Affiche un espace pour effacer
}

// Fonction qui dessine le serpent à partir des tableaux de coordonnées
void dessinerSerpent(int positionsX[], int positionsY[]) {
    for (int i = 0; i < LONGUEUR_SERPENT - 1; i++) {
        afficher(positionsX[i], positionsY[i], 'X');  // Affiche le corps du serpent
    }
    afficher(positionsX[LONGUEUR_SERPENT - 1], positionsY[LONGUEUR_SERPENT - 1], 'O');  // Affiche la tête du serpent
}

// Fonction pour faire avancer le serpent d'une position vers la droite
void progresser(int positionsX[], int positionsY[]) {
    effacer(positionsX[0], positionsY[0]);  // Efface la queue du serpent

    // Décalage des positions des segments du corps du serpent
    for (int i = 0; i < LONGUEUR_SERPENT - 1; i++) {
        positionsX[i] = positionsX[i + 1];
        positionsY[i] = positionsY[i + 1];
    }

    // Avance la tête d'une position vers la droite
    positionsX[LONGUEUR_SERPENT - 1]++;
    if (positionsX[LONGUEUR_SERPENT - 1] > LARGEUR_ECRAN) {  // Retourne au début si la tête dépasse l'écran
        positionsX[LONGUEUR_SERPENT - 1] = 1;
    }
}

// Fonction pour initialiser le serpent à la position initiale
void initialiserSerpent(int positionsX[], int positionsY[], int positionInitialeX, int positionInitialeY) {
    for (int i = 0; i < LONGUEUR_SERPENT; i++) {
        positionsX[LONGUEUR_SERPENT - 1 - i] = positionInitialeX - i;
        positionsY[LONGUEUR_SERPENT - 1 - i] = positionInitialeY;
    }
}

// Fonction pour déplacer le curseur à une position donnée (x, y)
void gotoXY(int colonne, int ligne) {
    printf("\033[%d;%dH", ligne, colonne);
}

// Fonction pour vérifier si une touche a été pressée sans bloquer l'exécution
int kbhit(void) {
    struct termios term, term_old;
    int caractere_presse;

    tcgetattr(STDIN_FILENO, &term);
    term_old = term;
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);

    caractere_presse = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &term_old);
    return caractere_presse;
}

// Fonction principale du programme
int main() {
    int positionsX[LONGUEUR_SERPENT];
    int positionsY[LONGUEUR_SERPENT];
    int positionInitialeX, positionInitialeY;

    // Demande de la position initiale de la tête du serpent
    printf("Entrez la position initiale de la tête du serpent (colonne, ligne) entre 1 et 40 :\n");
    printf("Colonne : ");
    scanf("%d", &positionInitialeX);
    printf("Ligne : ");
    scanf("%d", &positionInitialeY);

    // Efface l'écran
    system("clear");

    // Initialisation du serpent
    initialiserSerpent(positionsX, positionsY, positionInitialeX, positionInitialeY);

    // Boucle principale pour déplacer le serpent
    while (1) {
        dessinerSerpent(positionsX, positionsY);  // Dessine le serpent
        usleep(DELAI_DEPLACEMENT);                // Temporisation pour ralentir le mouvement
        progresser(positionsX, positionsY);       // Fait avancer le serpent

        // Vérification de l'appui sur la touche 'a' pour arrêter le jeu
        if (kbhit()) {
            char touche = getchar();
            if (touche == 'a') break;
        }
    }

    return 0;
}
