#define _POSIX_C_SOURCE 200809L

#include "Handlers_Cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

/* Variable globale pour signaler la réception d'une réponse */
volatile sig_atomic_t reponse_recue = 0;

/**
 * Handler appelé lors de la réception du signal SIGUSR1
 * Le serveur envoie ce signal au client pour lui indiquer
 * qu'une réponse est disponible dans son tube nommé
 */
void hand_reveil(int sig) {
    (void)sig;
    /* On marque simplement qu'une réponse est prête */
    reponse_recue = 1;
}

/**
 * Installe le gestionnaire de signal SIGUSR1 pour le client
 */
void installer_handlers_client(void) {
    struct sigaction sa;
    
    /* Configuration du handler */
    sa.sa_handler = hand_reveil;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    /* Installation pour SIGUSR1 */
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction SIGUSR1");
        exit(EXIT_FAILURE);
    }
}
