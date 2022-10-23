# ------------------- # --- VARIÁVEIS DE AMBIENTE --- # -------------------- #

CC = gcc
RM = rm -f
CFLAGS = -Wall -Wextra
SEQ_BIN = seq
PAR_BIN = par
ZIP = parallel.zip
OBJ = obj/studentsseq.o obj/studentspar_2.o

# ------------------- # --- DIRETIVAS PRINCIPAIS --- # -------------------- #

# Global
all: $(SEQ_BIN) $(PAR_BIN)

# Produção do executável
$(SEQ_BIN): $(OBJ)
	$(CC) $(OBJ) -o $(SEQ_BIN)
$(PAR_BIN): $(OBJ)
	$(CC) $(OBJ) -fopenmp -ofast $(PAR_BIN)

# Execução convencional do programa
run-seq:
	./$(SEQ_BIN)
run-par:
	./$(PAR_BIN)

# Execução do programa com Valgrind
valgrind-seq:
	valgrind -s --tool=memcheck --leak-check=full --track-origins=yes --show-leak-kinds=all ./$(SEQ_BIN)
valgrind-seq:
	valgrind -s --tool=memcheck --leak-check=full --track-origins=yes --show-leak-kinds=all ./$(PAR_BIN)

# Compressão dos arquivos
zip: clean
	zip -r $(ZIP) Makefile lib/* src/* obj/

# Limpeza de objetos e de executável
clean:
	$(RM) $(SEQ_BIN) $(PAR_BIN) $(ZIP) obj/*.o

# ----------------------- # --- OBJETIFICAÇÃO --- # ------------------------ #

obj/studentsseq.o: src/studentseq.c
	$(CC) -c src/studentsseq.c -o obj/studentsseq.o $(CFLAGS)	

obj/studentspar_2.o: src/studentspar_2.c
	$(CC) -c src/studentspar_2.c -fopenmp -ofast obj/studentspar_2.o $(CFLAGS)
