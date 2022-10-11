#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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

    double ***estatsCid;
    estatsCid = (double ***)malloc(sizeof(double **) * R);
    for (int i = 0; i < C; i++)
        estatsCid[i] = (double **)malloc(sizeof(double *) * C);

    for (int i = 0; i < C; i++)
    {
        for (int j = 0; j < A; j++)
            estatsCid[i][j] = (double *)malloc(sizeof(int) * 3); // mediana, media, desvpad
    }

    // variáveis de Região
    int *minReg = (int *)malloc(sizeof(int) * R);
    int *maxReg = (int *)malloc(sizeof(int) * R);

    double **estatsReg;
    estatsReg = (double **)malloc(sizeof(double *) * R);
    for (int i = 0; i < R; i++)
        estatsReg[i] = (double *)malloc(sizeof(double) * 3); // mediana, media, desvpad

    // variaveis do país
    int menor = 101, maior = -1, melhorReg, melhorCid[2];
    double mediana, media, desvpad, segs = 0.0;

    // ========================================================================= //
    clock_t begin = clock();

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

    // medias, desvpads e melhores
    double somaCid, somaReg;
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
            estatsCid[regiao][cidade][1] = (double)(somaCid / A);
        }
        estatsReg[regiao][1] = (double)(somaReg / C);
    }

    // melhores
    // double currCid = -1, currReg = -1;
    // for (int regiao = 0; regiao < R; regiao++)
    // {
    //     for (int cidade = 0; cidade < C; cidade++)
    //     {
    //         if (estatsCid[regiao][cidade][1] > currCid)
    //         {
    //             currCid = estatsCid[regiao][cidade][1];
    //             melhorCid[0] = regiao;
    //             melhorCid[1] = cidade;
    //         }

    //         currReg += currCid;
    //         estatsCid[regiao][cidade][1] = (double)currCid / A;
    //     }
    //     if (estatsReg[regiao][1] > currCid)
    //     {
    //         currReg = estatsReg[regiao][1];
    //         melhorReg = regiao;
    //     }
    // }

    clock_t end = clock();
    segs += (double)(end - begin) / CLOCKS_PER_SEC;
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
    printf("Tempo de resposta sem considerar E/S, em segundos: %.8fs\n", segs);

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