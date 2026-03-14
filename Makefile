# Dossiers
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include

CC= gcc 		# Compilateur C
CFLAGS= -Wall -ansi -I$(INC_DIR)	# Les options de compilations
LDFLAGS= -Wall -ansi		# Les options de linkage
EXEC= main					# Les noms des executables à générer

# Tous les fichiers .c automatiquement
SRC = $(wildcard $(SRC_DIR)/*.c)

# Fichiers objets (.o)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Création du dossier obj s'il n'existe pas
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

all: $(OBJ_DIR) $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC)

# Compilation des .c en .o vers obj/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -rf $(OBJ_DIR) $(EXEC)