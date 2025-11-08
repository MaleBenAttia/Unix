#ifndef SERV_CLI_FIFO_H
#define SERV_CLI_FIFO_H

#include <sys/types.h>

/* Taille maximale des messages */
#define NMAX 256

/* Nom du tube nommé du serveur */
#define FIFO_SERVEUR "/tmp/fifo_serveur"

/* Format pour les tubes nommés des clients : /tmp/fifo_<PID> */
#define FIFO_CLIENT_FORMAT "/tmp/fifo_%d"

/* Structure représentant une question envoyée par un client */
typedef struct {
    pid_t pid_client;        /* PID du client émetteur */
    int numero;              /* Numéro tiré au sort (entre 1 et NMAX) */
    char message[NMAX];      /* Message/question du client */
} Question;

/* Structure représentant une réponse envoyée par le serveur */
typedef struct {
    pid_t pid_client;        /* PID du client destinataire */
    int numero;              /* Numéro de la question */
    char message[NMAX];      /* Réponse du serveur */
} Reponse;

#endif
