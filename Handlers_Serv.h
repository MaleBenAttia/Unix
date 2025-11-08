#ifndef HANDLERS_SERV_H
#define HANDLERS_SERV_H

#include <signal.h>

/* Handler pour gérer la fin du serveur (SIGINT, SIGTERM) */
void hand_reveil(int sig);

/* Fonction pour installer les handlers de signaux du serveur */
void installer_handlers_serveur(void);

#endif
