#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <omp.h> 

// Nota máxima exclusiva
#define MAX_GRADE 100

// Número de vezes que executa um algoritmo para tomar a média de tempo
#define NUM_ATTEMPTS 2

// Para ativar as threads aninhadas do mergesort
#define OMP_NESTED TRUE

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


// Obtém a média de um arranjo de números de dupla precisão
double get_mean(double *array, int len){
    double sum = 0.0;
    int i;
    for(i=0; i<len; ++i)
        sum += array[i];
    return sum / len;
}


// Retorna um vetor inicializado com o valor fornecido, que é replicado
int *start_array(int len, int value) {
    int *array = (int *) malloc(len * sizeof(int));
    int i;
    for(i=0; i<len; ++i)
        array[i] = value;
    return array;
}


// Retorna o mínimo de um arranjo de inteiros
int get_min(int *array, int len) {
    int i, min = INT_MAX;
    for(i=0; i<len; ++i)
        if(array[i] < min)
            min = array[i];
    return min;
}


// Retorna o máximo de um arranjo de inteiros
int get_max(int *array, int len) {
    int i, max = INT_MAX;
    for(i=0; i<len; ++i)
        if(array[i] < max)
            max = array[i];
    return max;
}


// Retorna a nota mínima do país
int get_country_min_grade(int ***grades, int R, int C, int A){

    // Vetor de resultados temporários
    int num_threads = omp_get_max_threads();
    int *results = start_array(num_threads, INT_MAX);

    // Região de paralelização de busca
    #pragma omp parallel num_threads(num_threads)
    {
        int r, c, a, thread_num = omp_get_thread_num();
        #pragma omp for private(r,c,a)
        for(r=0; r<R; ++r){
            for(c=0; c<C; ++c){
                for(a=0; a<A; ++a){
                    if(grades[r][c][a] < results[thread_num]){
                        results[thread_num] = grades[r][c][a];
                    }
                }
            }
        }
    }

    // Minimização
    int result = get_min(results, num_threads);
    free(results);
    return result;
}


// Retorna a nota mínima da região
int get_region_min_grade(int **region, int C, int A){

    // Vetor de resultados temporários
    int num_threads = omp_get_max_threads();
    int *results = start_array(num_threads, INT_MAX);

    // Região de paralelização de busca
    #pragma omp parallel num_threads(num_threads)
    {
        int c, a, thread_num = omp_get_thread_num();
        #pragma omp for private(c,a)
        for(c=0; c<C; ++c){
            for(a=0; a<A; ++a){
                if(region[c][a] < results[thread_num]){
                    results[thread_num] = region[c][a];
                }
            }
        }
    }

    // Minimização
    int result = get_min(results, num_threads);
    free(results);
    return result;
}


// Retorna a nota mínima da cidade
int get_city_min_grade(int *city, int A){

    // Vetor de resultados temporários
    int num_threads = omp_get_max_threads();
    int *results = start_array(num_threads, INT_MAX);

    // Região de paralelização de busca
    #pragma omp parallel num_threads(num_threads)
    {
        int a, thread_num = omp_get_thread_num();
        #pragma omp for private(a)
        for(a=0; a<A; ++a){
            if(city[a] < results[thread_num]){
                results[thread_num] = city[a];
            }
        }
    }

    // Minimização
    int result = get_min(results, num_threads);
    free(results);
    return result;
}


// Calcula o tempo de execução do mínimo global e o imprime
void print_country_min_grade(int ***grades, int R, int C, int A){
    int i, min;
    double start, end;
    double times[NUM_ATTEMPTS];
    for(i=0; i<NUM_ATTEMPTS; ++i){
        start = omp_get_wtime();
        min = get_country_min_grade(grades, R, C, A);
        end = omp_get_wtime();
        times[i] = end-start;
    }
    printf("country minimum grade: %d. Mean time elapsed: %lf.\n", min, get_mean(times, NUM_ATTEMPTS));
}


// Calcula o tempo de execução do mínimo da região e o imprime
void print_region_min_grade(int ***grades, int region, int C, int A){
    int i, min;
    double start, end;
    double times[NUM_ATTEMPTS];
    for(i=0; i<NUM_ATTEMPTS; ++i){
        start = omp_get_wtime();
        min = get_region_min_grade(grades[region], C, A);
        end = omp_get_wtime();
        times[i] = end-start;
    }
    printf("region %d minimum grade: %d. Mean time elapsed: %lf.\n", region, min, get_mean(times, NUM_ATTEMPTS));
}


// Calcula o tempo de execução do mínimo da cidade e o imprime
void print_city_min_grade(int ***grades, int region, int city, int A){
    int i, min;
    double start, end;
    double times[NUM_ATTEMPTS];
    for(i=0; i<NUM_ATTEMPTS; ++i){
        start = omp_get_wtime();
        min = get_city_min_grade(grades[region][city], A);
        end = omp_get_wtime();
        times[i] = end-start;
    }
    printf("region %d, city %d minimum grade: %d. Mean time elapsed: %lf.\n", region, city, min, get_mean(times, NUM_ATTEMPTS));
}

// Retorna a nota máxima do país
int get_country_max_grade(int ***grades, int R, int C, int A){

    // Vetor de resultados temporários
    int num_threads = omp_get_max_threads();
    int *results = start_array(num_threads, INT_MIN);

    // Região de paralelização de busca
    #pragma omp parallel num_threads(num_threads)
    {
        int r, c, a, thread_num = omp_get_thread_num();
        #pragma omp for private(r,c,a)
        for(r=0; r<R; ++r){
            for(c=0; c<C; ++c){
                for(a=0; a<A; ++a){
                    if(grades[r][c][a] > results[thread_num]){
                        results[thread_num] = grades[r][c][a];
                    }
                }
            }
        }
    }

    // Maximização
    int result = get_max(results, num_threads);
    free(results);
    return result;
}


// Retorna a nota máxima da região
int get_region_max_grade(int **region, int C, int A){
    
    // Vetor de resultados temporários
    int num_threads = omp_get_max_threads();
    int *results = start_array(num_threads, INT_MIN);

    // Região de paralelização de busca
    #pragma omp parallel num_threads(num_threads)
    {
        int c, a, thread_num = omp_get_thread_num();
        #pragma omp for private(c,a)
        for(c=0; c<C; ++c){
            for(a=0; a<A; ++a){
                if(region[c][a] > results[thread_num]){
                    results[thread_num] = region[c][a];
                }
            }
        }
    }

    // Maximização
    int result = get_max(results, num_threads);
    free(results);
    return result;
}


// Retorna a nota máxima da cidade
int get_city_max_grade(int *city, int A){
    
    // Vetor de resultados temporários
    int num_threads = omp_get_max_threads();
    int *results = start_array(num_threads, INT_MIN);

    // Região de paralelização de busca
    #pragma omp parallel num_threads(num_threads)
    {
        int a, thread_num = omp_get_thread_num();
        #pragma omp for private(a)
        for(a=0; a<A; ++a){
            if(city[a] > results[thread_num]){
                results[thread_num] = city[a];
            }
        }
    }

    // Maximização
    int result = get_max(results, num_threads);
    free(results);
    return result;
}


// Calcula o tempo de execução do máximo global e o imprime
void print_country_max_grade(int ***grades, int R, int C, int A){
    int i, max;
    double start, end;
    double times[NUM_ATTEMPTS];
    for(i=0; i<NUM_ATTEMPTS; ++i){
        start = omp_get_wtime();
        max = get_country_max_grade(grades, R, C, A);
        end = omp_get_wtime();
        times[i] = end-start;
    }
    printf("country maximum grade: %d. Mean time elapsed: %lf.\n", max, get_mean(times, NUM_ATTEMPTS));
}


// Calcula o tempo de execução do máximo da região e o imprime
void print_region_max_grade(int ***grades, int region, int C, int A){
    int i, max;
    double start, end;
    double times[NUM_ATTEMPTS];
    for(i=0; i<NUM_ATTEMPTS; ++i){
        start = omp_get_wtime();
        max = get_region_max_grade(grades[region], C, A);
        end = omp_get_wtime();
        times[i] = end-start;
    }
    printf("region %d maximum grade: %d. Mean time elapsed: %lf.\n", region, max, get_mean(times, NUM_ATTEMPTS));
}


// Calcula o tempo de execução do máximo da cidade e o imprime
void print_city_max_grade(int ***grades, int region, int city, int A){
    int i, max;
    double start, end;
    double times[NUM_ATTEMPTS];
    for(i=0; i<NUM_ATTEMPTS; ++i){
        start = omp_get_wtime();
        max = get_city_max_grade(grades[region][city], A);
        end = omp_get_wtime();
        times[i] = end-start;
    }
    printf("region %d, city %d maximum grade: %d. Mean time elapsed: %lf.\n", region, city, max, get_mean(times, NUM_ATTEMPTS));
}


// Obtém a nota média do país
int get_country_mean_grade(int ***grades, int R, int C, int A) {
    int sum = 0;
    int r, c, a;
    #pragma omp parallel for reduction(+:sum) private(r,c,a)
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
    #pragma omp parallel for reduction(+:sum) private(c,a)
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
    #pragma omp parallel for reduction(+:sum) private(a)
    for(a=0; a<A; ++a){
        sum += city[a];
    }
    return sum / A;
}


// Calcula o tempo de execução da média do país e o imprime
void print_country_mean_grade(int ***grades, int R, int C, int A){
    int i, mean;
    double start, end;
    double times[NUM_ATTEMPTS];
    for(i=0; i<NUM_ATTEMPTS; ++i){
        start = omp_get_wtime();
        mean = get_country_mean_grade(grades, R, C, A);
        end = omp_get_wtime();
        times[i] = end-start;
    }
    printf("country mean grade: %d. Mean time elapsed: %lf.\n", mean, get_mean(times, NUM_ATTEMPTS));
}


// Calcula o tempo de execução da média da região e o imprime
void print_region_mean_grade(int ***grades, int region, int C, int A){
    int i, mean;
    double start, end;
    double times[NUM_ATTEMPTS];
    for(i=0; i<NUM_ATTEMPTS; ++i){
        start = omp_get_wtime();
        mean = get_region_mean_grade(grades[region], C, A);
        end = omp_get_wtime();
        times[i] = end-start;
    }
    printf("region %d mean grade: %d. Mean time elapsed: %lf.\n", region, mean, get_mean(times, NUM_ATTEMPTS));
}


// Calcula o tempo de execução da média da cidade e o imprime
void print_city_mean_grade(int ***grades, int region, int city, int A){
    int i, mean;
    double start, end;
    double times[NUM_ATTEMPTS];
    for(i=0; i<NUM_ATTEMPTS; ++i){
        start = omp_get_wtime();
        mean = get_city_mean_grade(grades[region][city], A);
        end = omp_get_wtime();
        times[i] = end-start;
    }
    printf("region %d, city %d mean grade: %d. Mean time elapsed: %lf.\n", region, city, mean, get_mean(times, NUM_ATTEMPTS));
}


// Obtém o desvio padrão das notas do país
int get_country_grades_std_deviation(int ***grades, int R, int C, int A) {
    int sum = 0;
    int r, c, a;
    int mean = get_country_mean_grade(grades, R, C, A);
    #pragma omp parallel for reduction(+:sum) shared(mean) private(r,c,a)
    for(r=0; r<R; ++r){
        for(c=0; c<C; ++c){
            for(a=0; a<A; ++a){
                sum += pow((grades[r][c][a] - mean), 2.0);
            }
        }
    }
    return sqrt(sum / (R*C*A));
}


// Obtém o desvio padrão das notas da região
int get_region_grades_std_deviation(int **region, int C, int A) {
    int sum = 0;
    int c, a;
    int mean = get_region_mean_grade(region, C, A);
    #pragma omp parallel for reduction(+:sum) shared(mean) private(c,a)
    for(c=0; c<C; ++c){
        for(a=0; a<A; ++a){
            sum += pow((region[c][a] - mean), 2.0);
        }
    }
    return sqrt(sum / (C*A));
}


// Obtém o desvio padrão das notas da cidade
int get_city_grades_std_deviation(int *city, int A) {
    int sum = 0;
    int a;
    int mean = get_city_mean_grade(city, A);
    #pragma omp parallel for reduction(+:sum) shared(mean) private(a)
    for(a=0; a<A; ++a){
        sum += pow((city[a] - mean), 2.0);
    }
    return sqrt(sum / A);
}


// Calcula o tempo de execução do desvio padrão do país e o imprime
void print_country_std_grade(int ***grades, int R, int C, int A){
    int i, std;
    double start, end;
    double times[NUM_ATTEMPTS];
    for(i=0; i<NUM_ATTEMPTS; ++i){
        start = omp_get_wtime();
        std = get_country_grades_std_deviation(grades, R, C, A);
        end = omp_get_wtime();
        times[i] = end-start;
    }
    printf("country grade std: %d. Mean time elapsed: %lf.\n", std, get_mean(times, NUM_ATTEMPTS));
}


// Calcula o tempo de execução do desvio padrão da região e o imprime
void print_region_std_grade(int ***grades, int region, int C, int A){
    int i, std;
    double start, end;
    double times[NUM_ATTEMPTS];
    for(i=0; i<NUM_ATTEMPTS; ++i){
        start = omp_get_wtime();
        std = get_region_grades_std_deviation(grades[region], C, A);
        end = omp_get_wtime();
        times[i] = end-start;
    }
    printf("region %d grade std: %d. Mean time elapsed: %lf.\n", region, std, get_mean(times, NUM_ATTEMPTS));
}


// Calcula o tempo de execução do desvio padrão da cidade e o imprime
void print_city_std_grade(int ***grades, int region, int city, int A){
    int i, std;
    double start, end;
    double times[NUM_ATTEMPTS];
    for(i=0; i<NUM_ATTEMPTS; ++i){
        start = omp_get_wtime();
        std = get_city_grades_std_deviation(grades[region][city], A);
        end = omp_get_wtime();
        times[i] = end-start;
    }
    printf("region %d, city %d grade std: %d. Mean time elapsed: %lf.\n", region, city, std, get_mean(times, NUM_ATTEMPTS));
}


// Achata um vetor de notas de um país
int *flatten_country_grades(int ***grades, int R, int C, int A){
    int *output = (int *) malloc(R * C * A * sizeof(int));
    int output_index = 0, r, c, a;
    for(r=0; r<R; ++r){
        for(c=0; c<C; ++c){
            for(a=0; a<A; ++a){
                output[output_index] = grades[r][c][a];
                ++output_index;
            }
        }
    }
    return output;
}


// Achata um vetor de notas de uma região
int *flatten_region_grades(int **region, int C, int A){
    int *output = (int *) malloc(C * A * sizeof(int));
    int output_index = 0, c, a;
    for(c=0; c<C; ++c){
        for(a=0; a<A; ++a){
            output[output_index] = region[c][a];
            ++output_index;
        }
    }
    return output;
}


// Cria uma cópia do vetor de uma cidade
int *copy_city(int *city, int A){
    int *output = (int *) malloc(A * sizeof(int));
    int a;
    for(a=0; a<A; ++a)
        output[a] = city[a];
    return output;
}


// Merges two subarrays of arr[].
// First subarray is arr[l..m]
// Second subarray is arr[m+1..r]
void merge(int arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;
 
    /* create temp arrays */
    int *L = (int *) malloc(n1 * sizeof(int));
    int *R = (int *) malloc(n2 * sizeof(int));
 
    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];
 
    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
 
    /* Copy the remaining elements of L[], if there
    are any */
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
 
    /* Copy the remaining elements of R[], if there
    are any */
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }

    free(L);
    free(R);
}
 

/* l is for left index and r is right index of the
sub-array of arr to be sorted */
void mergeSort(int arr[], int l, int r)
{
    if (l < r) {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int m = l + (r - l) / 2;
 
        // Sort first and second halves
        #pragma omp parallel num_threads(2)
        {
            #pragma omp single nowait
            mergeSort(arr, l, m);

            #pragma omp single
            mergeSort(arr, m + 1, r);

            #pragma omp single
            merge(arr, l, m, r);
        }
    }
}


// Obtém a mediana de um arranjo
int get_median(int *array, int len){
    mergeSort(array, 0, len-1);
    if(len % 2 != 0)
        return array[len/2];
    return (array[len/2] + array[len/2 - 1]) / 2;
}


// Calcula o tempo de execução da mediana do país e o imprime
void print_country_median_grade(int ***grades, int R, int C, int A){
    int i, median;
    double start, end;
    double times[NUM_ATTEMPTS];
    for(i=0; i<NUM_ATTEMPTS; ++i){

        // Preparação para utilizar o mergesort com a mesma entrada várias vezes
        int *flatten_grades = flatten_country_grades(grades, R, C, A);

        // Região de cálculo de tempo
        start = omp_get_wtime();
        median = get_median(flatten_grades, R*C*A);
        end = omp_get_wtime();
        times[i] = end-start;

        // Liberação do vetor temporário
        free(flatten_grades);
    }
    printf("country median grade: %d. Mean time elapsed: %lf.\n", median, get_mean(times, NUM_ATTEMPTS));
}


// Calcula o tempo de execução da mediana da região e o imprime
void print_region_median_grade(int ***grades, int region, int C, int A){
    int i, median;
    double start, end;
    double times[NUM_ATTEMPTS];
    for(i=0; i<NUM_ATTEMPTS; ++i){

        // Preparação para utilizar o mergesort com a mesma entrada várias vezes
        int *flatten_grades = flatten_region_grades(grades[region], C, A);

        // Região de cálculo de tempo
        start = omp_get_wtime();
        median = get_median(flatten_grades, C*A);
        end = omp_get_wtime();
        times[i] = end-start;

        // Liberação do vetor temporário
        free(flatten_grades);
    }
    printf("region %d median grade: %d. Mean time elapsed: %lf.\n", region, median, get_mean(times, NUM_ATTEMPTS));
}


// Calcula o tempo de execução da mediana da cidade e o imprime
void print_city_median_grade(int ***grades, int region, int city, int A){
    int i, median;
    double start, end;
    double times[NUM_ATTEMPTS];
    for(i=0; i<NUM_ATTEMPTS; ++i){

        // Preparação para utilizar o mergesort com a mesma entrada várias vezes
        int *flatten_grades = copy_city(grades[region][city], A);

        // Região de cálculo de tempo
        start = omp_get_wtime();
        median = get_median(flatten_grades, A);
        end = omp_get_wtime();
        times[i] = end-start;

        // Liberação do vetor temporário
        free(flatten_grades);
    }
    printf("region %d, city %d median grade: %d. Mean time elapsed: %lf.\n", region, city, median, get_mean(times, NUM_ATTEMPTS));
}


int main(void) {

    // Variáveis locais
    int R, C, A, seed;

    // Leitura dos parâmetros e ativação de semente
    scanf("%d%d%d%d", &R, &C, &A, &seed);
    srand(seed);

    // Geração das notas
    int ***grades = generate_grades(R, C, A, MAX_GRADE);
    print_grades(grades, R, C, A);

    // Aviso sobre o número de tentativas
    printf("obs.: for every output, %d attempts were taken to get mean time elapsed.\n\n", NUM_ATTEMPTS);
    
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
    
    // Desvios-padrão
    print_country_std_grade(grades, R, C, A);
    print_region_std_grade(grades, 0, C, A);
    print_city_std_grade(grades, 0, 0, A);
    printf("\n");
    
    // Medianas
    print_country_median_grade(grades, R, C, A);
    print_region_median_grade(grades, 0, C, A);
    print_city_median_grade(grades, 0, 0, A);
    printf("\n");

    // Finalização
    free_grades(grades, R, C);
    return EXIT_SUCCESS;
}