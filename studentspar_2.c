#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
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
void print_grades(int ***grades, int R, int C, int A) {
    int i, j, k;
    for(i=0; i<R; ++i){
        printf("region %d:\n", i);
        for(j=0; j<C; ++j){
            printf("city %d: ", j);
            for(k=0; k<A-1; ++k){
                printf("%d ", grades[i][j][k]);
            }
            printf("%d\n", grades[i][j][k]);
        }
        printf("\n");
    }
}


// Retorna a nota mínima do país
int get_country_min_grade(int ***grades, int R, int C, int A){
    int i, j, k, result = INT_MAX;
    #pragma omp parallel
    {
        int local_result = INT_MAX;
        #pragma omp for
        for(i=0; i<R; ++i){
            for(j=0; j<C; ++j){
                for(k=0; k<A; ++k){
                    if(grades[i][j][k] < local_result){
                        local_result = grades[i][j][k];
                        #pragma omp critical
                        {
                            if(local_result < result){
                                result = local_result;
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}


// Retorna a nota mínima da região
int get_region_min_grade(int **region, int C, int A){
    int i, j, result = INT_MAX;
    #pragma omp parallel
    {
        int local_result = INT_MAX;
        #pragma omp for
        for(i=0; i<C; ++i){
            for(j=0; j<A; ++j){
                if(region[i][j] < local_result){
                    local_result = region[i][j];
                    #pragma omp critical
                    {
                        if(local_result < result){
                            result = local_result;
                        }
                    }
                }
            }
        }
    }
    return result;
}


// Retorna a nota mínima da cidade
int get_city_min_grade(int *city, int A){
    int i, result = INT_MAX;
    #pragma omp parallel
    {
        int local_result = INT_MAX;
        #pragma omp for
        for(i=0; i<A; ++i){
            if(city[i] < local_result){
                local_result = city[i];
                #pragma omp critical
                {
                    if(local_result < result){
                        result = local_result;
                    }
                }
            }
        }
    }
    return result;
}


// Calcula o tempo de execução do mínimo global e o imprime
void print_country_min_grade(int ***grades, int R, int C, int A){
    double start = omp_get_wtime();
    int min = get_country_min_grade(grades, R, C, A);
    double end = omp_get_wtime();
    printf("country minimum grade: %d. Time elapsed: %lf.\n", min, end-start);
}


// Calcula o tempo de execução do mínimo da região e o imprime
void print_region_min_grade(int ***grades, int region, int C, int A){
    double start = omp_get_wtime();
    int min = get_region_min_grade(grades[region], C, A);
    double end = omp_get_wtime();
    printf("region %d minimum grade: %d. Time elapsed: %lf.\n", region, min, end-start);
}


// Calcula o tempo de execução do mínimo da cidade e o imprime
void print_city_min_grade(int ***grades, int region, int city, int A){
    double start = omp_get_wtime();
    int min = get_city_min_grade(grades[region][city], A);
    double end = omp_get_wtime();
    printf("region %d, city %d minimum grade: %d. Time elapsed: %lf.\n", region, city, min, end-start);
}

// Retorna a nota máxima do país
int get_country_max_grade(int ***grades, int R, int C, int A){
    int i, j, k, result = INT_MIN;
    #pragma omp parallel
    {
        int local_result = INT_MIN;
        #pragma omp for
        for(i=0; i<R; ++i){
            for(j=0; j<C; ++j){
                for(k=0; k<A; ++k){
                    if(grades[i][j][k] > local_result){
                        local_result = grades[i][j][k];
                        #pragma omp critical
                        {
                            if(local_result > result){
                                result = local_result;
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}


// Retorna a nota máxima da região
int get_region_max_grade(int **region, int C, int A){
    int i, j, result = INT_MIN;
    #pragma omp parallel
    {
        int local_result = INT_MIN;
        #pragma omp for
        for(i=0; i<C; ++i){
            for(j=0; j<A; ++j){
                if(region[i][j] > local_result){
                    local_result = region[i][j];
                    #pragma omp critical
                    {
                        if(local_result > result){
                            result = local_result;
                        }
                    }
                }
            }
        }
    }
    return result;
}


// Retorna a nota máxima da cidade
int get_city_max_grade(int *city, int A){
    int i, result = INT_MIN;
    #pragma omp parallel
    {
        int local_result = INT_MIN;
        #pragma omp for
        for(i=0; i<A; ++i){
            if(city[i] > local_result){
                local_result = city[i];
                #pragma omp critical
                {
                    if(local_result > result){
                        result = local_result;
                    }
                }
            }
        }
    }
    return result;
}


// Calcula o tempo de execução do máximo global e o imprime
void print_country_max_grade(int ***grades, int R, int C, int A){
    double start = omp_get_wtime();
    int max = get_country_max_grade(grades, R, C, A);
    double end = omp_get_wtime();
    printf("country maximum grade: %d. Time elapsed: %lf.\n", max, end-start);
}


// Calcula o tempo de execução do máximo da região e o imprime
void print_region_max_grade(int ***grades, int region, int C, int A){
    double start = omp_get_wtime();
    int max = get_region_max_grade(grades[region], C, A);
    double end = omp_get_wtime();
    printf("region %d maximum grade: %d. Time elapsed: %lf.\n", region, max, end-start);
}


// Calcula o tempo de execução do máximo da cidade e o imprime
void print_city_max_grade(int ***grades, int region, int city, int A){
    double start = omp_get_wtime();
    int max = get_city_max_grade(grades[region][city], A);
    double end = omp_get_wtime();
    printf("region %d, city %d maximum grade: %d. Time elapsed: %lf.\n", region, city, max, end-start);
}


// Obtém a nota média do país
int get_country_mean_grade(int ***grades, int R, int C, int A) {
    int sum = 0;
    int r, c, a;
    #pragma omp parallel for reduction(+:sum)
    for(r=0; r<R; ++r){
        for(c=0; c<C; ++c){
            for(a=0; a<A; ++a){
                sum += grades[r][c][a];
            }
        }
    }
    return sum / (R*C*A);
}


// Obtém a nota média da região
int get_region_mean_grade(int **region, int C, int A) {
    int sum = 0;
    int c, a;
    #pragma omp parallel for reduction(+:sum)
    for(c=0; c<C; ++c){
        for(a=0; a<A; ++a){
            sum += region[c][a];
        }
    }
    return sum / (C*A);
}


// Obtém a nota média da cidade
int get_city_mean_grade(int *city, int A) {
    int sum = 0;
    int a;
    #pragma omp parallel for reduction(+:sum)
    for(a=0; a<A; ++a){
        sum += city[a];
    }
    return sum / A;
}


// Calcula o tempo de execução da média do país e o imprime
void print_country_mean_grade(int ***grades, int R, int C, int A){
    double start = omp_get_wtime();
    int mean = get_country_mean_grade(grades, R, C, A);
    double end = omp_get_wtime();
    printf("country mean grade: %d. Time elapsed: %lf.\n", mean, end-start);
}


// Calcula o tempo de execução da média da região e o imprime
void print_region_mean_grade(int ***grades, int region, int C, int A){
    double start = omp_get_wtime();
    int mean = get_region_mean_grade(grades[region], C, A);
    double end = omp_get_wtime();
    printf("region %d mean grade: %d. Time elapsed: %lf.\n", region, mean, end-start);
}


// Calcula o tempo de execução da média da cidade e o imprime
void print_city_mean_grade(int ***grades, int region, int city, int A){
    double start = omp_get_wtime();
    int mean = get_city_mean_grade(grades[region][city], A);
    double end = omp_get_wtime();
    printf("region %d, city %d mean grade: %d. Time elapsed: %lf.\n", region, city, mean, end-start);
}


int main(void) {

    // Definição do número de threads
    omp_set_num_threads(omp_get_max_threads());

    // Variáveis locais
    int R, C, A, seed;

    // Leitura dos parâmetros e ativação de semente
    scanf("%d%d%d%d", &R, &C, &A, &seed);
    srand(seed);

    // Geração das notas
    int ***grades = generate_grades(R, C, A, MAX_GRADE);
    print_grades(grades, R, C, A);

    // Notas mínimas
    print_country_min_grade(grades, R, C, A);
    print_region_min_grade(grades, 0, C, A);
    print_city_min_grade(grades, 0, 0, A);
    printf("\n");

    // Notas máximas
    print_country_max_grade(grades, R, C, A);
    print_region_max_grade(grades, 0, C, A);
    print_city_max_grade(grades, 0, 0, A);
    printf("\n");

    // Médias
    print_country_mean_grade(grades, R, C, A);
    print_region_mean_grade(grades, 0, C, A);
    print_city_mean_grade(grades, 0, 0, A);
    printf("\n");

    // Finalização
    free_grades(grades, R, C);
    return EXIT_SUCCESS;
}