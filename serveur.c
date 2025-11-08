#define _POSIX_C_SOURCE 200809L

#include "serv_cli_fifo.h"
#include "Handlers_Serv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>

/* Variable externe définie dans Handlers_Serv.c */
extern volatile sig_atomic_t serveur_actif;

/**
 * Traite une question reçue d'un client
 * Construit une réponse et l'envoie au client via son tube nommé
 */
void traiter_question(Question *q) {
    Reponse r;
    char fifo_client[256];
    int fd_client;
    
    printf("[SERVEUR] Question reçue de client PID=%d, numero=%d: %s\n", 
           q->pid_client, q->numero, q->message);
    
    /* Préparation de la réponse */
    r.pid_client = q->pid_client;
    r.numero = q->numero;
    snprintf(r.message, NMAX, "Réponse du serveur pour question #%d", q->numero);
    
    /* Construction du nom du tube client */
    snprintf(fifo_client, sizeof(fifo_client), FIFO_CLIENT_FORMAT, q->pid_client);
    
    /* Ouverture du tube client pour écriture */
    fd_client = open(fifo_client, O_WRONLY);
    if (fd_client == -1) {
        perror("[SERVEUR] Erreur open tube client");
        return;
    }
    
    /* Envoi de la réponse */
    if (write(fd_client, &r, sizeof(Reponse)) != sizeof(Reponse)) {
        perror("[SERVEUR] Erreur write réponse");
    } else {
        printf("[SERVEUR] Réponse envoyée au client PID=%d\n", q->pid_client);
    }
    
    close(fd_client);
    
    /* Envoi du signal SIGUSR1 au client pour le réveiller */
    if (kill(q->pid_client, SIGUSR1) == -1) {
        perror("[SERVEUR] Erreur kill SIGUSR1");
    }
}

int main(void) {
    int fd_serveur;
    Question q;
    ssize_t nb_lus;
    
    printf("=== SERVEUR DE COMMUNICATION FIFO ===\n");
    printf("PID du serveur: %d\n\n", getpid());
    
    /* Installation des handlers de signaux */
    installer_handlers_serveur();
    
    /* Suppression du tube s'il existe déjà */
    unlink(FIFO_SERVEUR);
    
    /* Création du tube nommé du serveur */
    if (mkfifo(FIFO_SERVEUR, 0666) == -1) {
        perror("[SERVEUR] Erreur mkfifo");
        exit(EXIT_FAILURE);
    }
    
    printf("[SERVEUR] Tube nommé créé: %s\n", FIFO_SERVEUR);
    printf("[SERVEUR] En attente de clients...\n\n");
    
    /* Ouverture du tube en lecture (bloquant jusqu'à ce qu'un client ouvre en écriture) */
    fd_serveur = open(FIFO_SERVEUR, O_RDONLY);
    if (fd_serveur == -1) {
        perror("[SERVEUR] Erreur open");
        unlink(FIFO_SERVEUR);
        exit(EXIT_FAILURE);
    }
    
    /* Boucle principale de traitement des questions */
    while (serveur_actif) {
        /* Lecture d'une question */
        nb_lus = read(fd_serveur, &q, sizeof(Question));
        
        if (nb_lus == sizeof(Question)) {
            /* Question complète reçue */
            traiter_question(&q);
        } else if (nb_lus == 0) {
            /* Fin de fichier : tous les clients ont fermé leur côté écriture */
            /* On rouvre le tube pour accepter de nouveaux clients */
            close(fd_serveur);
            fd_serveur = open(FIFO_SERVEUR, O_RDONLY);
            if (fd_serveur == -1) {
                perror("[SERVEUR] Erreur réouverture");
                break;
            }
        } else if (nb_lus == -1) {
            if (errno == EINTR) {
                /* Interruption par signal, on continue */
                continue;
            }
            perror("[SERVEUR] Erreur read");
            break;
        }
    }
    
    /* Nettoyage */
    close(fd_serveur);
    unlink(FIFO_SERVEUR);
    
    printf("\n[SERVEUR] Arrêt du serveur.\n");
    return EXIT_SUCCESS;
}
