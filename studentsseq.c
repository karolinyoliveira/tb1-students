#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h> // Usado para padronizar a coleta do tempo de execução entre as implementações

int ***geradorDeNotas(int R, int C, int A, int seed);

int main(void)
{
    int R, C, A, seed;
    scanf("%d%d%d%d", &R, &C, &A, &seed);
    srand(seed);

    int ***notas = geradorDeNotas(R, C, A, seed);

    // PRINT NOTAS
    for (int i = 0; i < R; i++)
    {
        for (int j = 0; j < C; j++)
        {
            for (int k = 0; k < A; k++)
                printf("%d ", notas[i][j][k]);
            printf("\n");
        }
        printf("\n");
    }

    // --------- Cálculos --------- //
    // variáveis de cidade
    int **minCid = malloc(sizeof(int) * R);
    for (int i = 0; i < C; i++)
        minCid[i] = (int *)malloc(sizeof(int) * C);

    int **maxCid = malloc(sizeof(int) * R);
    for (int i = 0; i < C; i++)
        maxCid[i] = (int *)malloc(sizeof(int) * C);

    float ***estatsCid;
    estatsCid = (float ***)malloc(sizeof(float **) * R);
    for (int i = 0; i < C; i++)
        estatsCid[i] = (float **)malloc(sizeof(float *) * C);

    for (int i = 0; i < C; i++)
    {
        for (int j = 0; j < A; j++)
            estatsCid[i][j] = (float *)malloc(sizeof(int) * 3); // mediana, media, desvpad
    }

    // variáveis de Região
    int *minReg = (int *)malloc(sizeof(int) * R);
    int *maxReg = (int *)malloc(sizeof(int) * R);

    float **estatsReg;
    estatsReg = (float **)malloc(sizeof(float *) * R);
    for (int i = 0; i < R; i++)
        estatsReg[i] = (float *)malloc(sizeof(float) * 3); // mediana, media, desvpad

    // variaveis de País
    int menor = 101, maior = -1, melhorReg, melhorCid[2];
    float mediana, media, desvpad;

    // ========================================================================= //
    float begin = omp_get_wtime();

    // min, max e medianas
    for (int regiao = 0; regiao < R; regiao++)
    {
        minReg[regiao] = 101, maxReg[regiao] = -1;
        for (int cidade = 0; cidade < C; cidade++)
        {
            minCid[regiao][cidade] = 101, maxCid[regiao][cidade] = -1;
            for (int aluno = 0; aluno < A; aluno++)
            {
                if (notas[regiao][cidade][aluno] < minCid[regiao][cidade])
                    minCid[regiao][cidade] = notas[regiao][cidade][aluno];

                if (notas[regiao][cidade][aluno] > maxCid[regiao][cidade])
                    maxCid[regiao][cidade] = notas[regiao][cidade][aluno];
            }
            if (minCid[regiao][cidade] < minReg[regiao])
                minReg[regiao] = minCid[regiao][cidade];

            if (maxCid[regiao][cidade] > maxReg[regiao])
                maxReg[regiao] = maxCid[regiao][cidade];
        }
        if (minReg[regiao] < menor)
            menor = minReg[regiao];

        if (maxReg[regiao] > maior)
            maior = maxReg[regiao];
    }

    // medias
    float somaCid, somaReg, soma = 0;
    for (int regiao = 0; regiao < R; regiao++)
    {
        somaReg = 0;
        for (int cidade = 0; cidade < C; cidade++)
        {
            somaCid = 0;
            for (int aluno = 0; aluno < A; aluno++)
            {
                somaCid += notas[regiao][cidade][aluno];
            }
            somaReg += somaCid;
            estatsCid[regiao][cidade][1] = (float)(somaCid / A);
        }
        estatsReg[regiao][1] = (float)(somaReg / (C * A));
        soma += somaReg;
    }
    media = (float)soma / (R * C * A);

    // desvpads
    float varCid = 0.0, varReg = 0.0, var = 0.0;
    for (int regiao = 0; regiao < R; regiao++)
    {
        somaReg = 0;
        for (int cidade = 0; cidade < C; cidade++)
        {
            somaCid = 0;
            for (int aluno = 0; aluno < A; aluno++)
            {
                float v = estatsCid[regiao][cidade][1] - notas[regiao][cidade][aluno];
                varCid += v * v;
            }
            varReg += varCid;
            estatsCid[regiao][cidade][2] = sqrt(varCid / A);
        }
        estatsReg[regiao][2] = sqrt(varReg / (C * A));
        var += varReg;
    }
    desvpad = sqrt(var / (R * C * A));

    // melhores
    float currCid = -1, currReg = -1;
    for (int regiao = 0; regiao < R; regiao++)
    {
        for (int cidade = 0; cidade < C; cidade++)
        {
            if (estatsCid[regiao][cidade][1] > currCid)
            {
                currCid = estatsCid[regiao][cidade][1];
                melhorCid[0] = regiao;
                melhorCid[1] = cidade;
            }

            currReg += currCid;
            estatsCid[regiao][cidade][1] = (float)currCid / A;
        }
        if (estatsReg[regiao][1] > currCid)
        {
            currReg = estatsReg[regiao][1];
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
            printf("Reg %d - Cid %d: menor: %d, maior: %d, mediana: %.2f, média: %.2f e DP: %.2f\n", regiao, cidade, minCid[regiao][cidade], maxCid[regiao][cidade], estatsCid[regiao][cidade][0], estatsCid[regiao][cidade][1], estatsCid[regiao][cidade][2]);
        }
        printf("\n");
    }

    // Por Região
    for (int regiao = 0; regiao < R; regiao++)
    {
        printf("Reg %d: menor: %d, maior: %d, mediana: %.2f, média: %.2f e DP: %.2f\n", regiao, minReg[regiao], maxReg[regiao], estatsReg[regiao][0], estatsReg[regiao][1], estatsReg[regiao][2]);
    }
    printf("\n");

    // Brasil
    printf("Brasil: menor: %d, maior: %d, mediana: %.2f, média: %.2f e DP: %.2f\n", menor, maior, mediana, media, desvpad);
    printf("\n");

    // Melhor Região e cidade
    printf("Melhor região: Região %d\n", melhorReg);
    printf("Melhor cidade: Região %d, Cidade %d\n", melhorCid[0], melhorCid[1]);
    printf("\n");

    // Tempo de Resposta
    printf("Tempo de resposta sem considerar E/S, em segundos: %.8fs\n", end - begin);

    // --------- Desalocações --------- //
    for (int i = 0; i < R; i++)
    {
        for (int j = 0; j < C; j++)
        {
            free(notas[i][j]);
            free(estatsCid[i][j]);
        }
    }

    for (int i = 0; i < A; i++)
    {
        free(notas[i]);
        free(estatsCid[i]);
        free(estatsReg[i]);
    }

    free(notas);
    free(minCid);
    free(maxCid);
    free(minReg);
    free(maxReg);
    free(estatsCid);
    free(estatsReg);
    return 0;
}

int ***geradorDeNotas(int R, int C, int A, int seed)
{
    int i, j, k;
    int ***notas = (int ***)malloc(sizeof(int **) * R);

    for (i = 0; i < C; i++)
        notas[i] = (int **)malloc(sizeof(int *) * C);

    for (i = 0; i < C; i++)
    {
        for (int j = 0; j < A; j++)
            notas[i][j] = (int *)malloc(sizeof(int) * A);
    }

    for (i = 0; i < R; i++)
    {
        for (j = 0; j < C; j++)
        {
            for (k = 0; k < A; k++)
                notas[i][j][k] = rand() % 101;
        }
    }
    return notas;
}

void merge(int *X, int n, int *tmp)
{
    int i = 0;
    int j = n / 2;
    int ti = 0;

    while (i < n / 2 && j < n)
    {
        if (X[i] < X[j])
        {
            tmp[ti] = X[i];
            ti++;
            i++;
        }
        else
        {
            tmp[ti] = X[j];
            ti++;
            j++;
        }
    }
    while (i < n / 2)
    {
        tmp[ti] = X[i];
        ti++;
        i++;
    }
    while (j < n)
    {
        tmp[ti] = X[j];
        ti++;
        j++;
    }
    memcpy(X, tmp, n * sizeof(int));
}

void mergesort(int *X, int n, int *tmp)
{
    if (n < 2)
        return;

    mergesort(X, n / 2, tmp);

    mergesort(X + (n / 2), n - (n / 2), tmp);

    merge(X, n, tmp);
}
