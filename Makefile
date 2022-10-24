# ------------------- # --- VARIÁVEIS DE AMBIENTE --- # -------------------- #

CC = gcc
RM = rm -f
CFLAGS = -Wall -Wextra -lm
SEQ_BIN = seq
PAR_BIN = par
ZIP = parallel.zip

# ------------------- # --- DIRETIVAS PRINCIPAIS --- # -------------------- #

# Global
all: $(SEQ_BIN) $(PAR_BIN)

# Produção do executável
$(SEQ_BIN): obj/studentsseq.o
	$(CC) obj/studentsseq.o -o $(SEQ_BIN) -lm
$(PAR_BIN): obj/studentspar_2.o
	$(CC) obj/studentspar_2.o -fopenmp -o $(PAR_BIN) -lm

# Execução convencional do programa
runseq: $(SEQ_BIN)
	./$(SEQ_BIN)
runpar: $(PAR_BIN)
	./$(PAR_BIN)

# Compressão dos arquivos
zip: clean
	zip -r $(ZIP) Makefile lib/* src/* obj/

# Limpeza de objetos e de executável
clean:
	$(RM) $(SEQ_BIN) $(PAR_BIN) $(ZIP) obj/*.o

# ----------------------- # --- OBJETIFICAÇÃO --- # ------------------------ #

obj/studentsseq.o:
	$(CC) -c ./studentsseq.c -o obj/studentsseq.o $(CFLAGS)

obj/studentspar_2.o:
	$(CC) -c ./studentspar_2.c -fopenmp -o obj/studentspar_2.o $(CFLAGS)
