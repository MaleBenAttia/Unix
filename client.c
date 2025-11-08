#include "serv_cli_fifo.h"
#include "Handlers_Cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>

/* Variable externe définie dans Handlers_Cli.c */
extern volatile sig_atomic_t reponse_recue;

/**
 * Envoie une question au serveur
 */
int envoyer_question(int fd_serveur, const char *message) {
    Question q;
    
    /* Préparation de la question */
    q.pid_client = getpid();
    q.numero = (rand() % NMAX) + 1;  /* Numéro aléatoire entre 1 et NMAX */
    strncpy(q.message, message, NMAX - 1);
    q.message[NMAX - 1] = '\0';
    
    printf("[CLIENT %d] Envoi question #%d: %s\n", getpid(), q.numero, q.message);
    
    /* Envoi de la question */
    if (write(fd_serveur, &q, sizeof(Question)) != sizeof(Question)) {
        perror("[CLIENT] Erreur write question");
        return -1;
    }
    
    return 0;
}

/**
 * Attend et lit la réponse du serveur
 */
int lire_reponse(int fd_client) {
    Reponse r;
    ssize_t nb_lus;
    
    printf("[CLIENT %d] En attente de la réponse...\n", getpid());
    
    /* Attente du signal SIGUSR1 du serveur */
    while (!reponse_recue) {
        pause();  /* Suspend le processus jusqu'à réception d'un signal */
    }
    
    /* Réinitialisation du flag */
    reponse_recue = 0;
    
    /* Lecture de la réponse */
    nb_lus = read(fd_client, &r, sizeof(Reponse));
    
    if (nb_lus == sizeof(Reponse)) {
        printf("[CLIENT %d] Réponse reçue pour question #%d: %s\n", 
               getpid(), r.numero, r.message);
        return 0;
    } else {
        perror("[CLIENT] Erreur lecture réponse");
        return -1;
    }
}

int main(void) {
    int fd_serveur, fd_client;
    char fifo_client[256];
    char message[NMAX];
    pid_t mon_pid = getpid();
    
    printf("\n=== CLIENT FIFO ===\n");
    printf("PID du client: %d\n\n", mon_pid);
    
    /* Initialisation du générateur aléatoire */
    srand(getpid());
    
    /* Installation des handlers de signaux */
    installer_handlers_client();
    
    /* Construction du nom du tube client */
    snprintf(fifo_client, sizeof(fifo_client), FIFO_CLIENT_FORMAT, mon_pid);
    
    /* Suppression du tube s'il existe déjà */
    unlink(fifo_client);
    
    /* Création du tube nommé du client */
    if (mkfifo(fifo_client, 0666) == -1) {
        perror("[CLIENT] Erreur mkfifo");
        exit(EXIT_FAILURE);
    }
    
    printf("[CLIENT %d] Tube nommé créé: %s\n", mon_pid, fifo_client);
    
    /* Ouverture du tube serveur en écriture */
    fd_serveur = open(FIFO_SERVEUR, O_WRONLY);
    if (fd_serveur == -1) {
        perror("[CLIENT] Erreur open tube serveur (serveur lancé?)");
        unlink(fifo_client);
        exit(EXIT_FAILURE);
    }
    
    /* Ouverture du tube client en lecture non-bloquante */
    fd_client = open(fifo_client, O_RDONLY | O_NONBLOCK);
    if (fd_client == -1) {
        perror("[CLIENT] Erreur open tube client");
        close(fd_serveur);
        unlink(fifo_client);
        exit(EXIT_FAILURE);
    }
    
    /* Repasse en mode bloquant */
    int flags = fcntl(fd_client, F_GETFL);
    fcntl(fd_client, F_SETFL, flags & ~O_NONBLOCK);
    
    /* Boucle de communication */
    printf("\n[CLIENT %d] Entrez vos questions (Ctrl+D pour quitter):\n", mon_pid);
    
    while (1) {
        printf("\n> ");
        fflush(stdout);
        
        /* Lecture de la question depuis stdin */
        if (fgets(message, NMAX, stdin) == NULL) {
            break;  /* EOF (Ctrl+D) */
        }
        
        /* Suppression du '\n' final */
        size_t len = strlen(message);
        if (len > 0 && message[len - 1] == '\n') {
            message[len - 1] = '\0';
        }
        
        /* Ignore les lignes vides */
        if (strlen(message) == 0) {
            continue;
        }
        
        /* Envoi de la question */
        if (envoyer_question(fd_serveur, message) == -1) {
            break;
        }
        
        /* Attente et lecture de la réponse */
        if (lire_reponse(fd_client) == -1) {
            break;
        }
    }
    
    /* Nettoyage */
    printf("\n[CLIENT %d] Fermeture...\n", mon_pid);
    close(fd_serveur);
    close(fd_client);
    unlink(fifo_client);
    
    return EXIT_SUCCESS;
}
