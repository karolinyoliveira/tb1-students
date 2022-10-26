#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>

#define NOTA_MAXIMA 100
#define N_NOTAS_CID A
#define N_NOTAS_REG (C * A)
#define N_NOTAS_BR (R * C * A)
#define NUM_ATTEMPTS 1 // valor usado para gerar uma média do tempo de execução no momento das coletas para o cálculo da Q3


/// @brief Gera uma matriz de notas aleatórias
/// @param R Quantia de regiões.
/// @param C Quantia de cidades por região.
/// @param A Quantia de estudantes por cidade.
/// @return Matriz com notas pseudoaleatórias, de cardinalidade R*C*A
int ***geradorDeNotas(int R, int C, int A);

/// @brief Obtém a mediana de um vetor de frequências
/// @param v Vetor de frequências, da qual será extraída a mediana
/// @param tam Número de elementos do vetor v
/// @return Mediana da amostra
float Mediana(int *v, int tam);

int main(void)
{
    int R, C, A, seed;
    scanf("%d%d%d%d", &R, &C, &A, &seed);
    srand(seed);

    int ***notas = geradorDeNotas(R, C, A);

    omp_set_num_threads(omp_get_max_threads());

    // --------- Cálculos --------- //
    // variáveis de cidade
    float ***estatsCid;
    estatsCid = (float ***)malloc(sizeof(float **) * R);
    for (int i = 0; i < R; i++)
        estatsCid[i] = (float **)malloc(sizeof(float *) * C);

    for (int i = 0; i < R; i++)
    {
        for (int j = 0; j < C; j++)
            estatsCid[i][j] = (float *)malloc(sizeof(float) * 5); // min, max, mediana, media, desvpad
    }

    // variáveis de Região
    float **estatsReg;
    estatsReg = (float **)malloc(sizeof(float *) * R);
    for (int i = 0; i < R; i++)
        estatsReg[i] = (float *)malloc(sizeof(float) * 5); // min, max, mediana, media, desvpad

    int **notasReg = (int **)malloc(sizeof(int *) * R);
    for (int i = 0; i < R; i++)
        notasReg[i] = (int *)calloc(101, sizeof(int));

    // variaveis de País
    int menor = 101, maior = -1;
    int melhorReg, melhorCid[2], notasBrasil[101] = {0};
    float mediana, media, desvpad;

    // ========================================================================= //
    float begin = omp_get_wtime();

// Min, Max
#pragma omp parallel for
    for (int regiao = 0; regiao < R; regiao++)
    {
        estatsReg[regiao][0] = 101, estatsReg[regiao][1] = -1;
        for (int cidade = 0; cidade < C; cidade++)
        {
            for (int min = 0; min <= NOTA_MAXIMA; min++)
            {
                if (notas[regiao][cidade][min])
                {
                    estatsCid[regiao][cidade][0] = min;
                    break;
                }
            }

            for (int max = NOTA_MAXIMA; max >= 0; max--)
            {
                if (notas[regiao][cidade][max])
                {
                    estatsCid[regiao][cidade][1] = max;
                    break;
                }
            }

            if (estatsCid[regiao][cidade][0] < estatsReg[regiao][0]) // minimo da cidade menor que mínimo da região
                estatsReg[regiao][0] = estatsCid[regiao][cidade][0];

            if (estatsCid[regiao][cidade][1] > estatsReg[regiao][1]) // máximo da cidade maior que máximo da região
                estatsReg[regiao][1] = estatsCid[regiao][cidade][1];
        }
        if (estatsReg[regiao][0] < menor) // minimo da região menor que mínimo do país
            menor = estatsReg[regiao][0];

        if (estatsReg[regiao][1] > maior) // máximo da região maior que máximo do país
            maior = estatsReg[regiao][1];
    }

// Merge das Notas
#pragma omp parallel for
    for (int regiao = 0; regiao < R; regiao++)
    {

        for (int cidade = 0; cidade < C; cidade++)
        {
#pragma omp simd
            for (int nota = 0; nota <= NOTA_MAXIMA; nota++)
            {
                notasReg[regiao][nota] += notas[regiao][cidade][nota];
            }
        }
    }

#pragma omp parallel for
    for (int regiao = 0; regiao < R; regiao++)
    {
#pragma omp simd
        for (int nota = 0; nota <= NOTA_MAXIMA; nota++)
        {
            notasBrasil[nota] += notasReg[regiao][nota];
        }
    }

    // Medianas
    mediana = Mediana(notasBrasil, N_NOTAS_BR);

#pragma omp parallel for
    for (int regiao = 0; regiao < R; regiao++)
    {
        for (int cidade = 0; cidade < C; cidade++)
        {
            estatsCid[regiao][cidade][2] = Mediana(notas[regiao][cidade], N_NOTAS_CID);
        }
        estatsReg[regiao][2] = Mediana(notasReg[regiao], N_NOTAS_REG);
    }

    // Medias
    long int somaCid, somaReg, soma = 0;

#pragma omp parallel for
    for (int regiao = 0; regiao < R; regiao++)
    {
        somaReg = 0;
        for (int cidade = 0; cidade < C; cidade++)
        {
            somaCid = 0;
            for (int nota = 0; nota <= NOTA_MAXIMA; nota++)
            {
                for (int ocorrencias = 0; ocorrencias < notas[regiao][cidade][nota]; ocorrencias++)
                    somaCid += nota;
            }
            somaReg += somaCid;
            estatsCid[regiao][cidade][3] = (float)somaCid / N_NOTAS_CID;
        }
        estatsReg[regiao][3] = (float)somaReg / N_NOTAS_REG;
        soma += somaReg;
    }

    media = (float)soma / N_NOTAS_BR;

    // Desvpads
    float varCid = 0.0, varReg = 0.0, var = 0.0, difCid, difReg, dif;

#pragma omp parallel for
    for (int regiao = 0; regiao < R; regiao++)
    {
        varReg = 0.0;
        for (int cidade = 0; cidade < C; cidade++)
        {
            varCid = 0.0;
            for (int nota = 0; nota <= NOTA_MAXIMA; nota++)
            {
                for (int ocorrencias = 0; ocorrencias < notas[regiao][cidade][nota]; ocorrencias++)
                {
                    difCid = estatsCid[regiao][cidade][3] - nota; // media da cidade menos nota atual
                    difReg = estatsReg[regiao][3] - nota;
                    dif = media - nota;

                    varCid += difCid * difCid;
                    varReg += difReg * difReg;
                    var += dif * dif;
                }
            }
            estatsCid[regiao][cidade][4] = sqrt(varCid / A);
        }
        estatsReg[regiao][4] = sqrt(varReg / (C * A));
    }
    desvpad = sqrt(var / (R * C * A));

    // Melhores
    float currCid = -1, currReg = -1;

#pragma omp parallel for
    for (int regiao = 0; regiao < R; regiao++)
    {
        for (int cidade = 0; cidade < C; cidade++)
        {
            if (estatsCid[regiao][cidade][3] > currCid)
            {
                currCid = estatsCid[regiao][cidade][3];
                melhorCid[0] = regiao;
                melhorCid[1] = cidade;
            }
        }

        if (estatsReg[regiao][3] > currReg)
        {
            currReg = estatsReg[regiao][3];
            melhorReg = regiao;
        }
    }

    float end = omp_get_wtime();
    // ========================================================================= //

    // --------- Resultados --------- //
    // Por Cidade
    for (int regiao = 0; regiao < R; regiao++)
    {
        for (int cidade = 0; cidade < C; cidade++)
        {
            printf("Reg %d - Cid %d: menor: %d, maior: %d, mediana: %.2f, média: %.2f e DP: %.2f\n", regiao, cidade, (int)estatsCid[regiao][cidade][0], (int)estatsCid[regiao][cidade][1], estatsCid[regiao][cidade][2], estatsCid[regiao][cidade][3], estatsCid[regiao][cidade][4]);
        }
        printf("\n");
    }

    // // Por Região
    for (int regiao = 0; regiao < R; regiao++)
    {
        printf("Reg %d: menor: %d, maior: %d, mediana: %.2f, média: %.2f e DP: %.2f\n", regiao, (int)estatsReg[regiao][0], (int)estatsReg[regiao][1], estatsReg[regiao][2], estatsReg[regiao][3], estatsReg[regiao][4]);
    }
    printf("\n");

    // Brasil
    printf("Brasil: menor: %d, maior: %d, mediana: %.2f, média: %.2f e DP: %.2f\n", menor, maior, mediana, media, desvpad);
    printf("\n");

    // // Melhor Região e cidade
    printf("Melhor região: Região %d\n", melhorReg);
    printf("Melhor cidade: Região %d, Cidade %d\n", melhorCid[0], melhorCid[1]);
    printf("\n");

    // Tempo de Resposta
    printf("Tempo de resposta sem considerar E/S, em segundos: %.3lffs\n", end - begin);

    // --------- Desalocações --------- //
    for (int i = 0; i < R; i++)
    {
        for (int j = 0; j < C; j++)
        {
            free(notas[i][j]);
            free(estatsCid[i][j]);
        }
        free(estatsReg[i]);
        free(notasReg[i]);
        free(estatsCid[i]);
        free(notas[i]);
    }

    free(notas);
    free(estatsCid);
    free(estatsReg);
    free(notasReg);

    return 0;
}

int ***geradorDeNotas(int R, int C, int A)
{
    int i, j, k;
    int ***notas = (int ***)malloc(sizeof(int **) * R);

    for (i = 0; i < R; i++)
        notas[i] = (int **)malloc(sizeof(int *) * C);

    for (i = 0; i < R; i++)
    {
        for (j = 0; j < C; j++)
            notas[i][j] = (int *)calloc(NOTA_MAXIMA + 1, sizeof(int)); // 101 notas possíveis
    }

    for (i = 0; i < R; i++)
    {
        for (j = 0; j < C; j++)
        {
            for (k = 0; k < A; k++)
            {
                int teste = rand() % 101;
                notas[i][j][teste]++;
            }
        }
    }
    return notas;
}

float Mediana(int *v, int tam)
{
    int contador = 0;
    for (int i = 0; i <= NOTA_MAXIMA; i++)
    {
        contador += v[i];
        if ((float)contador >= (float)(tam / 2 + (tam % 2 != 0))) // teto da divisão
        {
            if (tam % 2)
            {
                return i;
            }
            else
            {
                int j;

                for (j = i + 1; j <= NOTA_MAXIMA; j++)
                {
                    if (v[j])
                        break;
                }
                return (float)(i + j) / 2;
            }
        }
    }
}