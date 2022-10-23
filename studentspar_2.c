#include <stdio.h>
#include <stdlib.h>
#include <omp.h> 

#define MAX_GRADE 100

/// @brief Gera um vetor de notas.
/// @param R Quantia de regiões.
/// @param C Quantia de cidades por região.
/// @param A Quantia de estudantes por cidade.
/// @param max_grade Máxima nota alcançável exclusivamente.
/// @return Vetor com notas pseudoaleatórias, de cardinalidade R*C*A
int ***generate_grades(int R, int C, int A, int max_grade) {
    int ***output = (int ***) malloc(R * sizeof(int **));
    int i, j, k;
    for(i=0; i<R; ++i){
        output[i] = (int **) malloc(C * sizeof(int *));
        for(j=0; j<C; ++j){
            output[i][j] = (int *) malloc(A * sizeof(int));
            for(k=0; k<A; ++k){
                output[i][j][k] = rand() % max_grade;
            }
        }
    }
    return output;
}


/// @brief Desaloca um vetor de notas.
/// @param grades Vetor de notas.
/// @param R Número de regiões.
/// @param C Número de cidades por região.
void free_grades(int ***grades, int R, int C){
    int i, j;
    for(i=0; i<R; ++i){
        for(j=0; j<C; ++j){
            free(grades[i][j]);
        }
        free(grades[i]);
    }
    free(grades);
}


/// @brief Imprime, na saída padrão, um vetor de notas.
/// @param grades Vetor de notas.
/// @param R Quantia de regiões.
/// @param C Quantia de cidades por região.
/// @param A Quantia de alunos por cidade.
void print_grades(const int ***grades, int R, int C, int A) {
    int i, j, k;
    for(i=0; i<R; ++i){
        for(j=0; j<C; ++j){
            for(k=0; k<A-1; ++k){
                printf("%d ", grades[i][j][k]);
            }
            printf("%d\n", grades[i][j][k]);
        }
    }
}


int main(void) {

    // Variáveis locais
    int R, C, A, seed;

    // Leitura dos parâmetros e ativação de semente
    scanf("%d%d%d%d", &R, &C, &A, &seed);
    srand(seed);

    // Geração das notas
    int ***grades = geradorDeNotas(R, C, A, MAX_GRADE);
    print_grades(grades, R, C, A);

    //omp_set_num_threads(omp_get_max_threads());

    // Finalização
    free_grades(grades);
    return EXIT_SUCCESS;
}