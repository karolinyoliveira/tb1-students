# Introdução

O problema em análise consiste em um vetor tridimensional, em que suas três dimensões representam, respectivamente, a região do país, a cidade da região e a nota do estudante dessa cidade. Assim sendo, foram solicitados os seguintes itens referentes a esses dados: os valores mínimo, máximo e médio, além da mediana e do desvio padrão, em que cada um deles pode ser calculado para uma cidade, para uma região e, finalmente, para todo o país -- neste caso tratando todos os dados conjuntamente. Logo, considerando-se o requisito de projeto de paralelização do problema, foram implementadas duas variantes para sua resolução -- sendo uma delas sequencial e, a outra, paralela por meio do uso da API OpenMP para a linguagem de programação C; os tempos de resposta de ambas as versões serão utilizadas posteriormente para cômputo de ganhos de eficiência da paralelização sobre a execução sequencial.

# Análise PCAM

A fim de se propor um projeto de paralelização, a análise PCAM será utilizada -- sendo ela referente às técnicas de Particionamento, Comunicação, Aglomeração e Mapeamento. Desse modo, uma análise será feita individualmente para cada um dos requisitos apresentados, sendo que o Mapeamento considerará, em todo caso, a execução realizada em um *cluster* contendo XXX processadores homogêneos, além de que, na Aglomeração, como tal, será sempre considerado uma máquina MIMD com mémoria compartilhada. Portanto, o Mapeamento dividirá, de maneira estática, os aglomerados obtidos ao longo dos processadores do sistema computacional -- utilizando, para tanto, uma fila circular; essa configuração será constante ao longo dos tópicos e, dessa forma, será otimida nas seções seguintes.

## Mínimo

### Particionamento
Para o Particionamento do problema do valor mínimo, considere o grafo abaixo e tome $h\in\{1,2,...,11\}$ como a altura corrente na árvore, em que paralelização a nível de tarefas pode ser realizado nas alturas intermediárias. Assim sendo, após a obtenção dos dados e a sua formatação em um vetor tridimensional, realiza-se particionamento por dados -- dividindo o problema em regiões, atribuindo cada uma delas a uma tarefa. Em seguida, faz-se particionamento por cidades para cada região e, para cada cidade, particiona-se em estudantes. Por fim, faz-se redução por minimização dois a dois, isto é, a cada par de estudantes, toma-se a menor nota dentre as duas tarefas, eventualmente tomando mínimos entre cidades e entre regiões. Logo, em $h=7$, produz-se  o valor mínimo da cidade; em $h=9$, o da região; em $h=11$, o do país.

```mermaid
%% Paralelização do problema de minimização

%% Tipo de grafo
graph TD 

%% Regiões
id1[(read_data)] --> region_1
id1[(read_data)] --> region_2
id1[(read_data)] --> id2{{...}}
id1[(read_data)] --> region_R

%% Cidades :: região 1
region_1 --> city_11
region_1 --> city_12
region_1 --> id3{{...}}
region_1 --> city_1C

%% Cidades :: região 2
region_2 --> city_21
region_2 --> city_22
region_2 --> id4{{...}}
region_2 --> city_2C

%% Cidades :: região R
region_R --> city_R1
region_R --> city_R2
region_R --> id5{{...}}
region_R --> city_RC

%% Alunos :: região 1 :: cidade 1
city_11 --> student_111
city_11 --> student_112
city_11 --> id6{{...}}
city_11 --> student_11A

%% Alunos :: região 1 :: cidade 2
city_12 --> student_121
city_12 --> student_122
city_12 --> id7{{...}}
city_12 --> student_12A

%% Alunos :: região 1 :: cidade C
city_1C --> student_1C1
city_1C --> student_1C2
city_1C --> id8{{...}}
city_1C --> student_1CA

%% Alunos :: região 2 :: cidades
city_21 --> id9{{...}}
city_22 --> id10{{...}}
city_2C --> id11{{...}}

%% Alunos :: região R :: cidades
city_R1 --> id12{{...}}
city_R2 --> id13{{...}}
city_RC --> id14{{...}}

%% Redução :: região 1 :: cidade 1
student_111 --> id15((min))
student_112 --> id15((min))
student_112 --> id16((min))
id6{{...}} --> id16((min))
id6{{...}} --> id17((min))
student_11A --> id17((min))
id15((min)) --> id24((min))
id16((min)) --> id24((min))
id16((min)) --> id25((min))
id17((min)) --> id25((min))
id24((min)) --> id26((min)) %% ...
id25((min)) --> id26((min)) %% Redução da cidade 1

%% Redução :: região 1 :: cidade 2
student_121 --> id18((min))
student_122 --> id18((min))
student_122 --> id19((min))
id7{{...}} --> id19((min))
id7{{...}} --> id20((min))
student_12A --> id20((min))
id18((min)) --> id27((min))
id19((min)) --> id27((min))
id19((min)) --> id28((min))
id20((min)) --> id28((min))
id27((min)) --> id29((min)) %% ...
id28((min)) --> id29((min)) %% Redução da cidade 2

%% Redução :: região 1 :: cidade C
student_1C1 --> id21((min))
student_1C2 --> id21((min))
student_1C2 --> id22((min))
id8{{...}} --> id22((min))
id8{{...}} --> id23((min))
student_1CA --> id23((min))
id21((min)) --> id30((min))
id22((min)) --> id30((min))
id22((min)) --> id31((min))
id23((min)) --> id31((min))
id30((min)) --> id32((min)) %% ...
id31((min)) --> id32((min)) %% Redução da cidade C

%% Redução da região 1
id26((min)) --> id33((min))
id29((min)) --> id33((min))
id29((min)) --> id34((min))
id32((min)) --> id34((min))
id33((min)) --> id35((min)) %% ...
id34((min)) --> id35((min)) %% Redução da Região 1

%% Redução da região 2
id9{{...}} --> id36((min))
id10{{...}} --> id36((min))
id10{{...}} --> id37((min))
id4{{...}} --> id37((min))
id4{{...}} --> id38((min))
id11{{...}} --> id38((min))
id36((min)) --> id42((min))
id37((min)) --> id42((min))
id37((min)) --> id43((min))
id38((min)) --> id43((min))
id42((min)) --> id44((min)) %% ...
id43((min)) --> id44((min)) %% Redução da região 2

%% Redução da região R
id12{{...}} --> id39((min))
id13{{...}} --> id39((min))
id13{{...}} --> id40((min))
id5{{...}} --> id40((min))
id5{{...}} --> id41((min))
id14{{...}} --> id41((min))
id39((min)) --> id45((min))
id40((min)) --> id45((min))
id40((min)) --> id46((min))
id41((min)) --> id46((min))
id45((min)) --> id47((min)) %% ...
id46((min)) --> id47((min)) %% Redução da região R

%% Redução do país
id35((min)) --> id48((min))
id44((min)) --> id48((min))
id44((min)) --> id49((min))
id47((min)) --> id49((min))
id48((min)) --> id50((min)) %% ...
id49((min)) --> id50((min)) %% Redução do país
```
### Comunicação
Para a Comunicação neste problema, o fluxo de dados segue o grafo de dependências das tarefas. Os dados obtidos pela tarefa inicial são transmitidos às tarefas de obtenção de mínimos por região, cada qual transmite seus dados para as respectivas tarefas de obtenção de mínimos por cidade que, por sua vez, transmitem os dados para as tarefas atômicas de obtenção de mínimos por estudante -- as quais retornam a nota dele em si. A partir daí, a comunicação ocorre em pares em níveis equivalentes da árvore, até que se obtenha o mínimo buscado -- seja ele por cidade, por região ou por país, bastando tomar o subgrafo correspondente para cômputo.

### Aglomeração
Em geral, são criados tantos processos quantos elementos de processamento disponíveis no sistema computacional em questão. Todavia, a tarefa inicial é atribuída a único processo -- tal como a tarefa final, que retornará o mínimo buscado. Daí, caso seja buscado o mínimo da cidade, da região ou do país, respectivamente, será criado um *pool* de tarefas para cada par de estudante, cada cidade ou cada região -- o qual será consumido pelos processos criados anteriormente. Em geral, não haverá regiões críticas entre processos.

## Máximo

A análise PCAM, para a obtenção de máximos locais ou globais, é inteiramente semelhante àquela feita para o valor mínimo, sendo a única diferença a substituição da redução de minimização por uma redução de maximização -- isto é, a cada par de tarefas, recuperar sempre o maior valor armazenado entre as duas.
