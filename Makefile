# Makefile pour le projet Client-Serveur FIFO

# Compilateur et options
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LDFLAGS =

# Fichiers sources
SERVEUR_SRC = serveur.c Handlers_Serv.c
CLIENT_SRC = client.c Handlers_Cli.c

# Fichiers objets
SERVEUR_OBJ = $(SERVEUR_SRC:.c=.o)
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)

# Fichiers exécutables
SERVEUR_BIN = serveur
CLIENT_BIN = client

# Cible par défaut : compile tout
all: $(SERVEUR_BIN) $(CLIENT_BIN)

# Compilation du serveur
$(SERVEUR_BIN): $(SERVEUR_OBJ)
	@echo "Linkage du serveur..."
	$(CC) $(LDFLAGS) -o $@ $^
	@echo "Serveur compilé avec succès!"

# Compilation du client
$(CLIENT_BIN): $(CLIENT_OBJ)
	@echo "Linkage du client..."
	$(CC) $(LDFLAGS) -o $@ $^
	@echo "Client compilé avec succès!"

# Règle générique pour compiler les fichiers .c en .o
%.o: %.c
	@echo "Compilation de $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage des fichiers objets
clean:
	@echo "Nettoyage des fichiers objets..."
	rm -f $(SERVEUR_OBJ) $(CLIENT_OBJ)
	rm -f /tmp/fifo_serveur /tmp/fifo_*

# Nettoyage complet (objets + exécutables)
fclean: clean
	@echo "Nettoyage des exécutables..."
	rm -f $(SERVEUR_BIN) $(CLIENT_BIN)

# Recompilation complète
re: fclean all

# Cibles qui ne sont pas des fichiers
.PHONY: all clean fclean re
