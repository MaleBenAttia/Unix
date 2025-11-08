#define _POSIX_C_SOURCE 200809L

#include "Handlers_Serv.h"
#include "serv_cli_fifo.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

/* Variable globale pour signaler l'arrêt du serveur */
volatile sig_atomic_t serveur_actif = 1;

/**
 * Handler pour gérer la fin propre du serveur
 * Appelé lors de la réception de SIGINT (Ctrl+C) ou SIGTERM
 */
void hand_reveil(int sig) {
    (void)sig; /* Évite le warning unused parameter */
    printf("\n[SERVEUR] Signal reçu, arrêt en cours...\n");
    serveur_actif = 0;
    
    /* Suppression du tube nommé du serveur */
    unlink(FIFO_SERVEUR);
}

/**
 * Installe les gestionnaires de signaux pour le serveur
 */
void installer_handlers_serveur(void) {
    struct sigaction sa;
    
    /* Configuration du handler */
    sa.sa_handler = hand_reveil;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    /* Installation pour SIGINT (Ctrl+C) */
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction SIGINT");
        exit(EXIT_FAILURE);
    }
    
    /* Installation pour SIGTERM (kill) */
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction SIGTERM");
        exit(EXIT_FAILURE);
    }
}
