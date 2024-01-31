#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Ville {
    char nom[50];
    int passage;
    int depart;
    struct Ville *suivante;
} Ville;

typedef struct NoeudAVL {
    Ville *ville;
    struct NoeudAVL *gauche;
    struct NoeudAVL *droite;
    int equilibre;
} NoeudAVL;

// Structure mise à jour
struct Info {
    char nom[100];
    int passage;
    char depart[100];
};

// Fonction de comparaison mise à jour
int compare(const void *a, const void *b) {
    return ((struct Info *)b)->passage - ((struct Info *)a)->passage;
}

// Fonction de comparaison pour le tri alphabétique
int compareAlpha(const void *a, const void *b) {
    return strcmp(((struct Info *)a)->nom, ((struct Info *)b)->nom);
}

NoeudAVL *creerNoeudAVL(Ville *ville) {
    NoeudAVL *noeud = (NoeudAVL *)malloc(sizeof(NoeudAVL));
    if (noeud == NULL) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }

    noeud->ville = ville;
    noeud->gauche = NULL;
    noeud->droite = NULL;
    noeud->equilibre = 0;

    return noeud;
}

NoeudAVL *insererAVL(NoeudAVL *racine, Ville *ville, int *h) {
    if (racine == NULL) {
        *h = 1;
        return creerNoeudAVL(ville);
    }

    if (ville->passage < racine->ville->passage) {
        racine->gauche = insererAVL(racine->gauche, ville, h);
        *h = -*h;
    } else if (ville->passage > racine->ville->passage) {
        racine->droite = insererAVL(racine->droite, ville, h);
    } else {
        *h = 0;
        return racine;
    }

    if (*h != 0) {
        racine->equilibre = racine->equilibre + *h;

        if (racine->equilibre == 0) {
            *h = 0;
        } else {
            *h = 1;
        }
    }

    return racine;
}

NoeudAVL *insererVilleAVL(NoeudAVL *racineAVL, Ville *ville) {
    int h = 0;
    return insererAVL(racineAVL, ville, &h);
}

void parcoursInfixeAVL(NoeudAVL *a, int *compteur) {
    if (a != NULL && *compteur < 10) {
        parcoursInfixeAVL(a->droite, compteur);
        if (*compteur < 10) {
            printf("%s %d %d\n", a->ville->nom, a->ville->passage, a->ville->depart);
            (*compteur)++;
        }
        parcoursInfixeAVL(a->gauche, compteur);
    }
}

void ajouterVille(Ville **liste, char nom[50], int passage, int depart) {
    Ville *nouvelleVille = malloc(sizeof(Ville));
    if (nouvelleVille == NULL) {
        printf("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }
    strcpy(nouvelleVille->nom, nom);
    nouvelleVille->passage = passage;
    nouvelleVille->depart = depart;
    nouvelleVille->suivante = *liste;
    *liste = nouvelleVille;
}

// Fonction pour créer le fichier de fusion
void creerFichierFusion() {
    FILE *fichier1, *fichier2, *fichierSortie;
    struct Info *infos = NULL;
    int nombreDeLignes = 0;

    // Ouverture du premier fichier en mode lecture
    fichier1 = fopen("../city_counts.txt", "r");
    if (fichier1 == NULL) {
        perror("Erreur lors de l'ouverture du premier fichier");
        return;
    }

    // Ouverture du deuxième fichier en mode lecture
    fichier2 = fopen("../start_counts.txt", "r");
    if (fichier2 == NULL) {
        perror("Erreur lors de l'ouverture du deuxième fichier");
        fclose(fichier1);
        return;
    }

    // Allocation dynamique de la mémoire pour stocker les informations
    infos = malloc(sizeof(struct Info));
    if (infos == NULL) {
        perror("Erreur lors de l'allocation de mémoire");
        fclose(fichier1);
        fclose(fichier2);
        return;
    }

    // Lecture et stockage des informations du premier fichier
    while (fscanf(fichier1, "%s %d", infos[nombreDeLignes].nom, &infos[nombreDeLignes].passage) == 2) {
        strcpy(infos[nombreDeLignes].depart, "");  // Initialisation à une chaîne vide pour le premier fichier
        nombreDeLignes++;

        // Réallouer dynamiquement la mémoire pour stocker les informations supplémentaires
        infos = realloc(infos, (nombreDeLignes + 1) * sizeof(struct Info));
        if (infos == NULL) {
            perror("Erreur lors de la réallocation de la mémoire");
            fclose(fichier1);
            fclose(fichier2);
            return;
        }
    }

    // Lecture et mise à jour des informations du deuxième fichier
    while (fscanf(fichier2, "%s %s", infos[nombreDeLignes].nom, infos[nombreDeLignes].depart) == 2) {
        // Recherche si le nom existe déjà dans les entrées lues du premier fichier
        int i;
        for (i = 0; i < nombreDeLignes; i++) {
            if (strcmp(infos[i].nom, infos[nombreDeLignes].nom) == 0) {
                break;
            }
        }

        // Si le nom existe déjà, mettre à jour les informations
        if (i < nombreDeLignes) {
            strcpy(infos[i].depart, infos[nombreDeLignes].depart);
        } else {
            // Sinon, il s'agit d'une nouvelle entrée, ajouter comme avant
            infos[nombreDeLignes].passage = 0;
            nombreDeLignes++;

            // Réallouer dynamiquement la mémoire pour stocker les informations supplémentaires
            infos = realloc(infos, (nombreDeLignes + 1) * sizeof(struct Info));
            if (infos == NULL) {
                perror("Erreur lors de la réallocation de la mémoire");
                fclose(fichier1);
                fclose(fichier2);
                return;
            }
        }
    }

    // Tri du tableau par nom
    qsort(infos, nombreDeLignes, sizeof(struct Info), compare);

    // Ouverture du fichier de sortie en mode écriture
    fichierSortie = fopen("../fusion.txt", "w");
    if (fichierSortie == NULL) {
        perror("Erreur lors de l'ouverture du fichier de sortie");
        free(infos);
        fclose(fichier1);
        fclose(fichier2);
        return;
    }

    // Écriture des informations triées dans le fichier de sortie
    for (int i = 0; i < nombreDeLignes; i++) {
        fprintf(fichierSortie, "%s %d %s\n", infos[i].nom, infos[i].passage, infos[i].depart);
    }

    // Libération de la mémoire
    free(infos);

    // Fermeture des fichiers
    fclose(fichier1);
    fclose(fichier2);
    fclose(fichierSortie);
}

// Fonction pour créer le fichier alpha.txt
void creerFichierAlpha() {
    FILE *fichierFusion, *fichierAlpha;
    struct Info *infos = NULL;
    int nombreDeLignes = 0;

    // Ouverture du fichier "fusion.txt" en mode lecture
    fichierFusion = fopen("../fusion.txt", "r");
    if (fichierFusion == NULL) {
        perror("Erreur lors de l'ouverture du fichier fusion.txt");
        return;
    }

    // Allocation dynamique de la mémoire pour stocker les informations
    infos = malloc(sizeof(struct Info));
    if (infos == NULL) {
        perror("Erreur lors de l'allocation de mémoire");
        fclose(fichierFusion);
        return;
    }

    // Lecture et stockage des informations du fichier "fusion.txt"
    while (fscanf(fichierFusion, "%s %d %s", infos[nombreDeLignes].nom, &infos[nombreDeLignes].passage, infos[nombreDeLignes].depart) == 3) {
        nombreDeLignes++;

        // Réallouer dynamiquement la mémoire pour stocker les informations supplémentaires
        infos = realloc(infos, (nombreDeLignes + 1) * sizeof(struct Info));
        if (infos == NULL) {
            perror("Erreur lors de la réallocation de la mémoire");
            fclose(fichierFusion);
            return;
        }
    }

    // Sélection des 10 villes les plus traversées
    qsort(infos, nombreDeLignes, sizeof(struct Info), compare);
    if (nombreDeLignes > 10) {
        nombreDeLignes = 10;
    }

    // Ouverture du fichier "alpha.txt" en mode écriture
    fichierAlpha = fopen("../alpha.txt", "w");
    if (fichierAlpha == NULL) {
        perror("Erreur lors de l'ouverture du fichier alpha.txt");
        free(infos);
        fclose(fichierFusion);
        return;
    }

    // Écriture des informations triées dans le fichier "alpha.txt"
    for (int i = 0; i < nombreDeLignes; i++) {
        fprintf(fichierAlpha, "%s %d %s\n", infos[i].nom, infos[i].passage, infos[i].depart);
    }

    // Libération de la mémoire
    free(infos);

    // Fermeture des fichiers
    fclose(fichierFusion);
    fclose(fichierAlpha);
}

// Fonction pour créer le fichier traitement_t.txt
void creerFichierTraitementT() {
    FILE *fichierAlpha, *fichierTraitementT;
    struct Info *infos = NULL;
    int nombreDeLignes = 0;

    // Ouverture du fichier "alpha.txt" en mode lecture
    fichierAlpha = fopen("../alpha.txt", "r");
    if (fichierAlpha == NULL) {
        perror("Erreur lors de l'ouverture du fichier alpha.txt");
        return;
    }

    // Allocation dynamique de la mémoire pour stocker les informations
    infos = malloc(sizeof(struct Info));
    if (infos == NULL) {
        perror("Erreur lors de l'allocation de mémoire");
        fclose(fichierAlpha);
        return;
    }

    // Lecture et stockage des informations du fichier "alpha.txt"
    while (fscanf(fichierAlpha, "%s %d %s", infos[nombreDeLignes].nom, &infos[nombreDeLignes].passage, infos[nombreDeLignes].depart) == 3) {
        nombreDeLignes++;

        // Réallouer dynamiquement la mémoire pour stocker les informations supplémentaires
        infos = realloc(infos, (nombreDeLignes + 1) * sizeof(struct Info));
        if (infos == NULL) {
            perror("Erreur lors de la réallocation de la mémoire");
            fclose(fichierAlpha);
            return;
        }
    }

    // Tri du tableau par nom
    qsort(infos, nombreDeLignes, sizeof(struct Info), compareAlpha);

    // Ouverture du fichier "traitement_t.txt" en mode écriture
    fichierTraitementT = fopen("../traitement_t.txt", "w");
    if (fichierTraitementT == NULL) {
        perror("Erreur lors de l'ouverture du fichier traitement_t.txt");
        free(infos);
        fclose(fichierAlpha);
        return;
    }

    // Écriture des informations triées dans le fichier "traitement_t.txt"
    for (int i = 0; i < nombreDeLignes; i++) {
        fprintf(fichierTraitementT, "%s %d %s\n", infos[i].nom, infos[i].passage, infos[i].depart);
    }

    // Libération de la mémoire
    free(infos);

    // Fermeture des fichiers
    fclose(fichierAlpha);
    fclose(fichierTraitementT);
}

int main() {
    creerFichierFusion();
    creerFichierAlpha();
    creerFichierTraitementT();

    return 0;
}