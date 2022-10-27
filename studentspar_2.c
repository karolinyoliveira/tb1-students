#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <omp.h>

// Nota máxima exclusiva
#define MAX_GRADE 100
#define FREQS_LEN (MAX_GRADE + 1)

// Número de vezes que executa um algoritmo para tomar a média de tempo
#define NUM_ATTEMPTS 32

/// @brief Gera um vetor de notas.
/// @param R Quantia de regiões.
/// @param C Quantia de cidades por região.
/// @param A Quantia de estudantes por cidade.
/// @param max_grade Máxima nota alcançável exclusivamente.
/// @return Vetor com notas pseudoaleatórias, de cardinalidade R*C*A
int ***generate_grades(int R, int C, int A, int max_grade)
{
    int ***output = (int ***)malloc(R * sizeof(int **));
    int i, j, k;
    for (i = 0; i < R; ++i)
    {
        output[i] = (int **)malloc(C * sizeof(int *));
        for (j = 0; j < C; ++j)
        {
            output[i][j] = (int *)malloc(A * sizeof(int));
            for (k = 0; k < A; ++k)
            {
                output[i][j][k] = rand() % (max_grade + 1);
            }
        }
    }
    return output;
}

/// @brief Desaloca um vetor de notas.
/// @param grades Vetor de notas.
/// @param R Número de regiões.
/// @param C Número de cidades por região.
void free_grades(int ***grades, int R, int C)
{
    int i, j;
    for (i = 0; i < R; ++i)
    {
        for (j = 0; j < C; ++j)
        {
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
void print_grades_to_numpy(int ***grades, int R, int C, int A)
{
    int i, j, k;
    printf("np.array([\n");
    for (i = 0; i < R; ++i)
    {
        printf("\t[");
        for (j = 0; j < C; ++j)
        {
            printf("[");
            for (k = 0; k < A - 1; ++k)
            {
                printf("%d, ", grades[i][j][k]);
            }
            printf("%d],", grades[i][j][k]);
        }
        printf("],\n");
    }
    printf("])\n");
}

/// @brief Imprime, na saída padrão, um vetor de notas.
/// @param grades Vetor de notas.
/// @param R Quantia de regiões.
/// @param C Quantia de cidades por região.
/// @param A Quantia de alunos por cidade.
void print_grades(int ***grades, int R, int C, int A)
{
    int i, j, k;
    for (i = 0; i < R; ++i)
    {
        printf("regiao %d:\n", i);
        for (j = 0; j < C; ++j)
        {
            printf("cidade %d: ", j);
            for (k = 0; k < A - 1; ++k)
            {
                printf("%d ", grades[i][j][k]);
            }
            printf("%d\n", grades[i][j][k]);
        }
        printf("\n");
    }
}

// Obtém a média de um arranjo de números de dupla precisão
double get_mean(double *array, int len)
{
    double sum = 0.0;
    int i;
    for (i = 0; i < len; ++i)
        sum += array[i];
    return sum / len;
}

// Retorna um vetor inicializado com o valor fornecido, que é replicado
int *start_array(int len, int value)
{
    int *array = (int *)malloc(len * sizeof(int));
    int i;
    for (i = 0; i < len; ++i)
        array[i] = value;
    return array;
}

// Retorna o mínimo de um arranjo de inteiros
int get_min(int *array, int len)
{
    int i, min = INT_MAX;
    for (i = 0; i < len; ++i)
        if (array[i] < min)
            min = array[i];
    return min;
}

// Retorna o máximo de um arranjo de inteiros
int get_max(int *array, int len)
{
    int i, max = INT_MIN;
    for (i = 0; i < len; ++i)
        if (array[i] > max)
            max = array[i];
    return max;
}

// Retorna a nota mínima do país
int get_country_min_grade(int ***grades, int R, int C, int A)
{

    // Vetor de resultados temporários
    int num_threads = omp_get_max_threads();
    int *results = start_array(num_threads, INT_MAX);

// Região de paralelização de busca
#pragma omp parallel num_threads(num_threads)
    {
        int r, c, a, thread_num = omp_get_thread_num();
#pragma omp for simd private(r, c, a) schedule(simd \
                                               : static)
        for (r = 0; r < R; ++r)
        {
            for (c = 0; c < C; ++c)
            {
                for (a = 0; a < A; ++a)
                {
                    if (grades[r][c][a] < results[thread_num])
                    {
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
int get_region_min_grade(int **region, int C, int A)
{

    // Vetor de resultados temporários
    int num_threads = omp_get_max_threads();
    int *results = start_array(num_threads, INT_MAX);

// Região de paralelização de busca
#pragma omp parallel num_threads(num_threads)
    {
        int c, a, thread_num = omp_get_thread_num();
#pragma omp for simd private(c, a) schedule(simd \
                                            : static)
        for (c = 0; c < C; ++c)
        {
            for (a = 0; a < A; ++a)
            {
                if (region[c][a] < results[thread_num])
                {
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
int get_city_min_grade(int *city, int A)
{

    // Vetor de resultados temporários
    int num_threads = omp_get_max_threads();
    int *results = start_array(num_threads, INT_MAX);

// Região de paralelização de busca
#pragma omp parallel num_threads(num_threads)
    {
        int a, thread_num = omp_get_thread_num();
#pragma omp for simd private(a) schedule(simd \
                                         : static)
        for (a = 0; a < A; ++a)
        {
            if (city[a] < results[thread_num])
            {
                results[thread_num] = city[a];
            }
        }
    }

    // Minimização
    int result = get_min(results, num_threads);
    free(results);
    return result;
}

// Retorna a nota máxima do país
int get_country_max_grade(int ***grades, int R, int C, int A)
{

    // Vetor de resultados temporários
    int num_threads = omp_get_max_threads();
    int *results = start_array(num_threads, INT_MIN);

// Região de paralelização de busca
#pragma omp parallel num_threads(num_threads)
    {
        int r, c, a, thread_num = omp_get_thread_num();
#pragma omp for simd private(r, c, a) schedule(simd \
                                               : static)
        for (r = 0; r < R; ++r)
        {
            for (c = 0; c < C; ++c)
            {
                for (a = 0; a < A; ++a)
                {
                    if (grades[r][c][a] > results[thread_num])
                    {
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
int get_region_max_grade(int **region, int C, int A)
{

    // Vetor de resultados temporários
    int num_threads = omp_get_max_threads();
    int *results = start_array(num_threads, INT_MIN);

// Região de paralelização de busca
#pragma omp parallel num_threads(num_threads)
    {
        int c, a, thread_num = omp_get_thread_num();
#pragma omp for simd private(c, a) schedule(simd \
                                            : static)
        for (c = 0; c < C; ++c)
        {
            for (a = 0; a < A; ++a)
            {
                if (region[c][a] > results[thread_num])
                {
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
int get_city_max_grade(int *city, int A)
{

    // Vetor de resultados temporários
    int num_threads = omp_get_max_threads();
    int *results = start_array(num_threads, INT_MIN);

// Região de paralelização de busca
#pragma omp parallel num_threads(num_threads)
    {
        int a, thread_num = omp_get_thread_num();
#pragma omp for simd private(a) schedule(simd \
                                         : static)
        for (a = 0; a < A; ++a)
        {
            if (city[a] > results[thread_num])
            {
                results[thread_num] = city[a];
            }
        }
    }

    // Maximização
    int result = get_max(results, num_threads);
    free(results);
    return result;
}

// Obtém a nota média do país
int get_country_mean_grade(int ***grades, int R, int C, int A)
{
    float sum = 0;
    int r, c, a;
#pragma omp parallel for simd reduction(+                                     \
                                        : sum) private(r, c, a) schedule(simd \
                                                                         : static)
    for (r = 0; r < R; ++r)
    {
        for (c = 0; c < C; ++c)
        {
            for (a = 0; a < A; ++a)
            {
                sum += (float)grades[r][c][a];
            }
        }
    }
    return sum / (R * C * A);
}

// Obtém a nota média da região
float get_region_mean_grade(int **region, int C, int A)
{
    float sum = 0;
    int c, a;
#pragma omp parallel for simd reduction(+                                  \
                                        : sum) private(c, a) schedule(simd \
                                                                      : static)
    for (c = 0; c < C; ++c)
    {
        for (a = 0; a < A; ++a)
        {
            sum += (float)region[c][a];
        }
    }
    return sum / (C * A);
}

// Obtém a nota média da cidade
float get_city_mean_grade(int *city, int A)
{
    float sum = 0;
    int a;
#pragma omp parallel for simd reduction(+                               \
                                        : sum) private(a) schedule(simd \
                                                                   : static)
    for (a = 0; a < A; ++a)
    {
        sum += (float)city[a];
    }
    return sum / A;
}

// Obtém o desvio padrão das notas do país
float get_country_grades_std_deviation(int ***grades, int R, int C, int A, float mean)
{
    float sum = 0;
    int r, c, a;
#pragma omp parallel for simd reduction(+                                                  \
                                        : sum) shared(mean) private(r, c, a) schedule(simd \
                                                                                      : static)
    for (r = 0; r < R; ++r)
    {
        for (c = 0; c < C; ++c)
        {
            for (a = 0; a < A; ++a)
            {
                sum += pow((grades[r][c][a] - mean), 2.0);
            }
        }
    }
    return sqrt(sum / (R * C * A));
}

// Obtém o desvio padrão das notas da região
float get_region_grades_std_deviation(int **region, int C, int A, float mean)
{
    float sum = 0;
    int c, a;
#pragma omp parallel for simd reduction(+                                               \
                                        : sum) shared(mean) private(c, a) schedule(simd \
                                                                                   : static)
    for (c = 0; c < C; ++c)
    {
        for (a = 0; a < A; ++a)
        {
            sum += pow((region[c][a] - mean), 2.0);
        }
    }
    return sqrt(sum / (C * A));
}

// Obtém o desvio padrão das notas da cidade
float get_city_grades_std_deviation(int *city, int A, float mean)
{
    float sum = 0;
    int a;
#pragma omp parallel for simd reduction(+                                            \
                                        : sum) shared(mean) private(a) schedule(simd \
                                                                                : static)
    for (a = 0; a < A; ++a)
    {
        sum += pow((city[a] - mean), 2.0);
    }
    return sqrt(sum / A);
}

// Obtém a mediana a partir de um vetor de frequências
float get_median(int *v, int tam)
{
    // cria vetor de frequências
    int freq[MAX_GRADE + 1] = {0};
    for (int i = 0; i < tam; i++)
    {
        freq[v[i]]++;
    }

    // pega a mediana
    int contador = 0;
    for (int i = 0; i <= MAX_GRADE; i++)
    {
        contador += freq[i];
        if (contador == (tam / 2 + (tam % 2 != 0))) // teto da divisão
        {
            if (tam % 2)
            {
                return i;
            }
            else
            {
                int j;
                for (j = i + 1; j <= MAX_GRADE; j++)
                {
                    if (freq[j])
                        break;
                }
                return (float)(i + j) / 2;
            }
        }
        if (contador > (tam / 2 + (tam % 2 != 0)))
            return i;
    }
    return -1.0;
}

// Desaloca uma matriz de inteiros
void free_int_matrix(int **m, int rows)
{
    int i;
#pragma omp parallel for simd private(i) schedule(simd \
                                                  : static)
    for (i = 0; i < rows; ++i)
        free(m[i]);
    free(m);
}

// Mínimo
int _min(int a, int b)
{
    if (a < b)
        return a;
    return b;
}

// Faz mesclagem das notas para cálculo da mediana
int **merge_region_grades(int ***grades, int R, int C, int A)
{
    int **output = (int **)malloc(R * sizeof(int *));
    #pragma omp parallel for shared(output)
    for (int r = 0; r < R; ++r)
    {
        output[r] = (int *)calloc(C*A, sizeof(int));
        for (int c = 0; c < C; ++c)
        {
            for (int a = 0; a < A; ++a)
            {
                output[r][c*A+a] = grades[r][c][a];
            }
        }
    }
    return output;
}

// Mescla as notas do país para cálculo das medianas
int *merge_country_grades(int **region_merged_grades, int CA, int R)
{
    int *output = (int *)calloc(CA*R, sizeof(int));
    #pragma omp parallel for shared(output) 
    for (int r = 0; r < R; ++r)
    {
        for (int i = 0; i < CA; ++i)
        {
            output[r*CA+i] = region_merged_grades[r][i];
        }
    }
    return output;
}


int main(void)
{

    // Definição do número padrão de threads
    omp_set_num_threads(omp_get_max_threads());

    // Iteradores
    int iteration, region, city;

    // Dados do problema
    int R, C, A, seed;
    int ***grades = NULL;
    int **merged_region_grades = NULL;
    int *merged_country_grades = NULL;

    // Valores de interesse
    int min, max;
    float median, mean, std;

    // Otimização global
    float global_region_max = FLT_MIN;
    int global_region_argmax = 0;
    float global_city_max = FLT_MIN;
    int global_city_argmax[2] = {0, 0}; // (region,city)

    // Para cálculo de tempo
    double times[NUM_ATTEMPTS];
    double start, end, mean_time;

    // Leitura dos parâmetros e ativação de semente
    scanf("%d%d%d%d", &R, &C, &A, &seed);
    srand(seed);

    // Geração das notas
    grades = generate_grades(R, C, A, MAX_GRADE);
    print_grades(grades, R, C, A);

    // Notas mescladas para cálculo de mediana
    merged_region_grades = merge_region_grades(grades, R, C, A);
    merged_country_grades = merge_country_grades(merged_region_grades, A*C, R);

    // Dados das cidades
    for (region = 0; region < R; ++region)
    {
        for (city = 0; city < C; ++city)
        {
            min = get_city_min_grade(grades[region][city], A);
            max = get_city_max_grade(grades[region][city], A);
            median = get_median(grades[region][city], A);
            mean = get_city_mean_grade(grades[region][city], A);
            std = get_city_grades_std_deviation(grades[region][city], A, mean);
            printf(
                "Reg %d - Cid %d: menor: %d, maior: %d, mediana: %.2f, média: %.2f e DP: %.2f\n",
                region, city, min, max, median, mean, std);
            if (mean > global_city_max)
            {
                global_city_max = mean;
                global_city_argmax[0] = region;
                global_city_argmax[1] = city;
            }
        }
        printf("\n");
    }

    // Dados da região
    for (region = 0; region < R; ++region)
    {
        min = get_region_min_grade(grades[region], C, A);
        max = get_region_max_grade(grades[region], C, A);
        median = get_median(merged_region_grades[region], C * A);
        mean = get_region_mean_grade(grades[region], C, A);
        std = get_region_grades_std_deviation(grades[region], C, A, mean);
        printf(
            "Reg %d: menor: %d, maior: %d, mediana: %.2f, média: %.2f e DP: %.2f\n",
            region, min, max, median, mean, std);
        if (mean > global_region_max)
        {
            global_region_max = mean;
            global_region_argmax = region;
        }
    }

    // Dados do país
    min = get_country_min_grade(grades, R, C, A);
    max = get_country_max_grade(grades, R, C, A);
    median = get_median(merged_country_grades, R * C * A);
    mean = get_country_mean_grade(grades, R, C, A);
    std = get_country_grades_std_deviation(grades, R, C, A, mean);
    printf(
        "\nBrasil: menor: %d, maior: %d, mediana: %.2f, média: %.2f e DP: %.2f\n\n",
        min, max, median, mean, std);

    // Finalização da impressão dos dados
    printf("Melhor região: Região %d\n", global_region_argmax);
    printf("Melhor cidade: Região %d, Cidade %d\n\n", global_city_argmax[0], global_city_argmax[1]);

    // Desalocação
    free_int_matrix(merged_region_grades, R);
    free(merged_country_grades);


    // Para cálculo da média de tempo
    for(iteration=0; iteration<NUM_ATTEMPTS; ++iteration){

        // Início do cálculo de tempo
        start = omp_get_wtime();

        // Notas mescladas para cálculo de mediana
        merged_region_grades = merge_region_grades(grades, R, C, A);
        merged_country_grades = merge_country_grades(merged_region_grades, A*C, R);

        // Iteração ao longo das cidades das regiões
        for(region=0; region<R; ++region) {
            for(city=0; city<C; ++city){
                min = get_city_min_grade(grades[region][city], A);
                max = get_city_max_grade(grades[region][city], A);
                median = get_median(grades[region][city], A);
                mean = get_city_mean_grade(grades[region][city], A);
                std = get_city_grades_std_deviation(grades[region][city], A, mean);
            }
        }

        // Iteração ao longo das regiões
        for(region=0; region<R; ++region) {
            min = get_region_min_grade(grades[region], C, A);
            max = get_region_max_grade(grades[region], C, A);
            median = get_median(merged_region_grades[region], C * A);
            mean = get_region_mean_grade(grades[region], C, A);
            std = get_region_grades_std_deviation(grades[region], C, A, mean);
        }

        // Para o país
        min = get_country_min_grade(grades, R, C, A);
        max = get_country_max_grade(grades, R, C, A);
        median = get_median(merged_country_grades, R * C * A);
        mean = get_country_mean_grade(grades, R, C, A);
        std = get_country_grades_std_deviation(grades, R, C, A, mean);

        // Finalização
        end = omp_get_wtime();
        times[iteration] = end-start;

        // Desalocação
        free_int_matrix(merged_region_grades, R);
        free(merged_country_grades);
    }

    // Finalização
    mean_time = get_mean(times, NUM_ATTEMPTS);
    printf("Tempo de resposta sem considerar E/S, em segundos: %.3lfs\n", mean_time);
    
    free_grades(grades, R, C);
    return EXIT_SUCCESS;
}