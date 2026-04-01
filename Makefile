# Dossiers
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include

CC      = gcc
# -std=c99 obligatoire (for/int, //, stdbool.h)
# fifo.c et sjf.c sont EXCLUS : leurs fonctions sont déjà dans scheduler.c
CFLAGS  = -Wall -std=c99 -I$(INC_DIR)
EXEC    = simulateur

all: $(OBJ_DIR) $(EXEC)

# Tous les fichiers .c automatiquement
#SRC = $(SRC_DIR)/process.c \
#      $(SRC_DIR)/queue.c \
#      $(SRC_DIR)/csv.c \
#      $(SRC_DIR)/scheduler.c \
#      $(SRC_DIR)/sjrf.c \
#      $(SRC_DIR)/rr.c \
#      $(SRC_DIR)/main.c
SRC = $(wildcard $(SRC_DIR)/*.c)

OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Création du dossier obj si nécessaire
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC)
	@echo ">>> Binaire produit : $(EXEC)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(EXEC)
	@echo ">>> Nettoyage effectué"

doc:
	doxygen Doxyfile
	@echo ">>> Documentation generee dans doc/html"

install:
	@if [ -w /usr/local/bin ]; then \
        cp simulateur /usr/local/bin; \
        echo ">>> Installé dans /usr/local/bin"; \
    else \
        mkdir -p $(HOME)/bin; \
        cp simulateur $(HOME)/bin; \
        echo ">>> Installé dans $(HOME)/bin"; \
	fi

uninstall:
	rm -f /usr/local/bin/$(EXEC) $(HOME)/bin/$(EXEC)
	@echo ">>> Desinstallation bien reussi!!";