#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* les tailles maximales qu'on s'autorise */
#define MAX_NOM 64
#define MAX_CONTENU 1024
#define MAX_ENFANTS 32
#define ESPACE_TOTAL 100000

int espace_utilise = 0;


typedef struct Fichier {
    char nom[MAX_NOM];
    char contenu[MAX_CONTENU];
    int taille;
} Fichier;

typedef struct Repertoire {
    char nom[MAX_NOM];

    Fichier *fichiers[MAX_ENFANTS];
    int nb_fichiers;

    struct Repertoire *enfants[MAX_ENFANTS];
    int nb_enfants;

    struct Repertoire *parent;   /* NULL si on est à la racine */
} Repertoire;



Repertoire *creer_repertoire(char *nom, Repertoire *parent) {
    Repertoire *rep = malloc(sizeof(Repertoire));

    strcpy(rep->nom, nom);
    rep->nb_fichiers = 0;
    rep->nb_enfants = 0;
    rep->parent = parent;

    espace_utilise += sizeof(Repertoire);

    return rep;
}


void creer_fichier(Repertoire *rep, char *nom, char *contenu) {

    if (rep->nb_fichiers >= MAX_ENFANTS) {
        printf("Erreur : répertoire plein.\n");
        return;
    }

    /* vérifier si le fichier existe déjà */
    for (int i = 0; i < rep->nb_fichiers; i++) {
        if (strcmp(rep->fichiers[i]->nom, nom) == 0) {
            printf("Erreur : le fichier '%s' existe deja.\n", nom);
            return;
        }
    }

    Fichier *f = malloc(sizeof(Fichier));

    strcpy(f->nom, nom);
    strcpy(f->contenu, contenu);
    f->taille = strlen(contenu);

    rep->fichiers[rep->nb_fichiers] = f;
    rep->nb_fichiers++;

    espace_utilise += sizeof(Fichier);

    printf("Fichier '%s' créé.\n", nom);
}


/* chercher un fichier dans un répertoire, retourne NULL si pas trouvé */
Fichier *trouver_fichier(Repertoire *rep, char *nom) {
    for (int i = 0; i < rep->nb_fichiers; i++) {
        if (strcmp(rep->fichiers[i]->nom, nom) == 0)
            return rep->fichiers[i];
    }
    return NULL;
}


/* afficher le contenu d'un fichier */
void lire_fichier(Repertoire *rep, char *nom) {
    Fichier *f = trouver_fichier(rep, nom);

    if (f == NULL) {
        printf("Erreur : fichier '%s' introuvable.\n", nom);
        return;
    }

    printf("=== %s ===\n", f->nom);
    printf("%s\n", f->contenu);
    printf("==========\n");
}


/* modifier le contenu d'un fichier existant */
void ecrire_fichier(Repertoire *rep, char *nom, char *nouveau_contenu) {
    Fichier *f = trouver_fichier(rep, nom);

    if (f == NULL) {
        printf("Erreur : fichier '%s' introuvable.\n", nom);
        return;
    }

    int ancienne_taille = f->taille;

    strcpy(f->contenu, nouveau_contenu);
    f->taille = strlen(nouveau_contenu);

    /* on met à jour l'espace utilisé */
    espace_utilise += f->taille - ancienne_taille;

    printf("Fichier '%s' modifié.\n", nom);
}


void supprimer_fichier(Repertoire *rep, char *nom) {
    int index = -1;

    for (int i = 0; i < rep->nb_fichiers; i++) {
        if (strcmp(rep->fichiers[i]->nom, nom) == 0) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("Erreur : fichier '%s' introuvable.\n", nom);
        return;
    }

    espace_utilise -= sizeof(Fichier);
    free(rep->fichiers[index]);

    /* décaler les fichiers suivants d'une case vers la gauche */
    for (int i = index; i < rep->nb_fichiers - 1; i++) {
        rep->fichiers[i] = rep->fichiers[i + 1];
    }
    rep->nb_fichiers--;

    printf("Fichier '%s' supprimé.\n", nom);
}


/* afficher tous les fichiers et sous-répertoires du répertoire courant */
void lister_contenu(Repertoire *rep) {
    printf("Contenu de [%s] :\n", rep->nom);

    if (rep->nb_fichiers == 0 && rep->nb_enfants == 0) {
        printf("  (vide)\n");
        return;
    }

    for (int i = 0; i < rep->nb_enfants; i++) {
        printf("  [REP]  %s/\n", rep->enfants[i]->nom);
    }
    for (int i = 0; i < rep->nb_fichiers; i++) {
        printf("  [FILE] %s  (%d octets)\n", rep->fichiers[i]->nom, rep->fichiers[i]->taille);
    }
}



Repertoire *changer_repertoire(Repertoire *courant, char *nom) {

    if (strcmp(nom, "..") == 0) {
        if (courant->parent == NULL) {
            printf("Déjà à la racine.\n");
            return courant;
        }
        return courant->parent;
    }

    for (int i = 0; i < courant->nb_enfants; i++) {
        if (strcmp(courant->enfants[i]->nom, nom) == 0)
            return courant->enfants[i];
    }

    printf("Erreur : répertoire '%s' introuvable.\n", nom);
    return courant;
}


/* afficher l'espace mémoire utilisé et disponible */
void afficher_espace() {
    printf("Espace utilisé : %d octets\n", espace_utilise);
    printf("Espace libre   : %d octets\n", ESPACE_TOTAL - espace_utilise);
}



void liberer_repertoire(Repertoire *rep) {
    for (int i = 0; i < rep->nb_fichiers; i++)
        free(rep->fichiers[i]);

    for (int i = 0; i < rep->nb_enfants; i++)
        liberer_repertoire(rep->enfants[i]);

    free(rep);
}



void supprimer_repertoire(Repertoire *courant, char *nom) {
    int index = -1;

    for (int i = 0; i < courant->nb_enfants; i++) {
        if (strcmp(courant->enfants[i]->nom, nom) == 0) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("Erreur : répertoire '%s' introuvable.\n", nom);
        return;
    }

    Repertoire *cible = courant->enfants[index];

    /* on refuse de supprimer un répertoire non vide */
    if (cible->nb_fichiers > 0 || cible->nb_enfants > 0) {
        printf("Erreur : le répertoire '%s' n'est pas vide.\n", nom);
        return;
    }

    espace_utilise -= sizeof(Repertoire);
    free(cible);

    /* décaler le tableau enfants d'une case vers la gauche */
    for (int i = index; i < courant->nb_enfants - 1; i++)
        courant->enfants[i] = courant->enfants[i + 1];
    courant->nb_enfants--;

    printf("Répertoire '%s' supprimé.\n", nom);
}


void afficher_aide() {
    printf("\nCommandes :\n");
    printf("  ls                     afficher le contenu du répertoire courant\n");
    printf("  mkdir <nom>            créer un sous-répertoire\n");
    printf("  rmdir <nom>            supprimer un répertoire vide\n");
    printf("  cd <nom>  ou  cd ..    changer de répertoire\n");
    printf("  touch <nom>            créer un fichier vide\n");
    printf("  write <nom> <contenu>  écrire dans un fichier (le crée si besoin)\n");
    printf("  cat <nom>              lire un fichier\n");
    printf("  rm <nom>               supprimer un fichier\n");
    printf("  df                     voir l'espace mémoire\n");
    printf("  help                   afficher cette aide\n");
    printf("  exit                   quitter\n\n");
}


void traiter_commande(Repertoire **courant, char *ligne) {

    char *cmd = strtok(ligne, " \t\n");
    if (cmd == NULL) return;  /* ligne vide */

    if (strcmp(cmd, "ls") == 0) {
        lister_contenu(*courant);

    } else if (strcmp(cmd, "mkdir") == 0) {
        char *nom = strtok(NULL, " \t\n");
        if (nom == NULL) { printf("Usage : mkdir <nom>\n"); return; }

        if ((*courant)->nb_enfants >= MAX_ENFANTS) {
            printf("Erreur : trop de sous-répertoires.\n");
            return;
        }
        Repertoire *nouveau = creer_repertoire(nom, *courant);
        (*courant)->enfants[(*courant)->nb_enfants] = nouveau;
        (*courant)->nb_enfants++;
        printf("Répertoire '%s' créé.\n", nom);

    } else if (strcmp(cmd, "cd") == 0) {
        char *nom = strtok(NULL, " \t\n");
        if (nom == NULL) { printf("Usage : cd <nom>\n"); return; }
        *courant = changer_repertoire(*courant, nom);

    } else if (strcmp(cmd, "touch") == 0) {
        char *nom = strtok(NULL, " \t\n");
        if (nom == NULL) { printf("Usage : touch <nom>\n"); return; }
        creer_fichier(*courant, nom, "");

    } else if (strcmp(cmd, "write") == 0) {
        char *nom     = strtok(NULL, " \t\n");
        char *contenu = strtok(NULL, "\n");   /* tout ce qui reste sur la ligne */
        if (nom == NULL) { printf("Usage : write <nom> <contenu>\n"); return; }
        if (contenu == NULL) contenu = "";

        /* si le fichier existe déjà on le modifie, sinon on le crée */
        if (trouver_fichier(*courant, nom) != NULL)
            ecrire_fichier(*courant, nom, contenu);
        else
            creer_fichier(*courant, nom, contenu);

    } else if (strcmp(cmd, "cat") == 0) {
        char *nom = strtok(NULL, " \t\n");
        if (nom == NULL) { printf("Usage : cat <nom>\n"); return; }
        lire_fichier(*courant, nom);

    } else if (strcmp(cmd, "rmdir") == 0) {
        char *nom = strtok(NULL, " \t\n");
        if (nom == NULL) { printf("Usage : rmdir <nom>\n"); return; }
        supprimer_repertoire(*courant, nom);

    } else if (strcmp(cmd, "rm") == 0) {
        char *nom = strtok(NULL, " \t\n");
        if (nom == NULL) { printf("Usage : rm <nom>\n"); return; }
        supprimer_fichier(*courant, nom);

    } else if (strcmp(cmd, "df") == 0) {
        afficher_espace();

    } else if (strcmp(cmd, "help") == 0) {
        afficher_aide();

    } else {
        printf("Commande inconnue : '%s'  (tapez 'help')\n", cmd);
    }
}


int main() {

    Repertoire *racine  = creer_repertoire("root", NULL);
    Repertoire *courant = racine;

    printf("=== Système de Fichier Simplifié ===\n");
    printf("Tapez 'help' pour voir les commandes.\n\n");

    char ligne[512];

    while (1) {
        printf("[%s]> ", courant->nom);
        fflush(stdout);

        if (fgets(ligne, sizeof(ligne), stdin) == NULL)
            break;

        /* enlever le \n à la fin */
        ligne[strcspn(ligne, "\n")] = '\0';

        if (strcmp(ligne, "exit") == 0)
            break;

        traiter_commande(&courant, ligne);
    }

    /* libérer toute la mémoire avant de quitter */
    liberer_repertoire(racine);
    printf("Au revoir.\n");

    return 0;
}
