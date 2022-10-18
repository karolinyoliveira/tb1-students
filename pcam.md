```mermaid
%% Clivo de Eratosthenes N=60 %%
graph TD 
  DefN --> CriarVet
 'K=2' --- CriarVet %% isso dps do criarVet
  CriarVet --> 'Vet_2'
  CriarVet --> 'Vet_3'
  CriarVet --> 'Vet_4'
  CriarVet --> 'Vet_5'
  CriarVet --> 'Vet_6'
  CriarVet --> 'Vet_7'
  CriarVet --> '...'
  CriarVet --> 'Vet_60'

%% etapa 3b
	'Vet_2'--> 'menorQK'
	'Vet_3'--> 'menorQK'

	'Vet_4'--> 'menorQK*'
	'Vet_5'--> 'menorQK*'

	'Vet_6'--> 'menorQK**'
	'Vet_7'--> 'menorQK**'

	'menorQK'--> 'menorQK_l2'
	'menorQK*'--> 'menorQK_l2'

	'menorQK**'--> 'menorQK_l2*'
	'Vet_60'--> 'menorQK_l2*'

  'menorQK_l2' --> menorQK_l3
  'menorQK_l2*' --> menorQK_l3

	menorQK_l3 --- 'K=3'
  'K=3' --> Vet_2
  'K=3' --> Vet_3
  'K=3' --> Vet_4
  'K=3' --> Vet_5
  'K=3' --> Vet_6
  'K=3' --> Vet_7
  'K=3' --> '....'
  'K=3' --> Vet_60

	'K=3' --- repeteLoopAnterior
```

