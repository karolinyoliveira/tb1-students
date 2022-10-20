# tb1-students
Project developed for High-Performance Computing classes

## Minimização:
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


