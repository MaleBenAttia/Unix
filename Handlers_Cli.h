#ifndef HANDLERS_CLI_H
#define HANDLERS_CLI_H

#include <signal.h>

/* Handler pour gérer le réveil du client (réception de SIGUSR1) */
void hand_reveil(int sig);

/* Fonction pour installer les handlers de signaux du client */
void installer_handlers_client(void);

#endif
