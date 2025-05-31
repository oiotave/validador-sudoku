# Validador de Sudoku

## Introdução
A seguinte implementação foi apresentada como projeto final da disciplina de Sistemas Operacionais, na Universidade Federal da Paraíba (UFPB). Desenvolvido em C++, o código faz uma análise simples de velocidade e desempenho de um algoritmo de validação de jogos de Sudoku, usando multithreading por meio da biblioteca ```pthreads.h```.

## Colaboradores
O desenvolvimento do código ficou sob responsabilidade dos seguintes alunos: [Davi de Lacerda Teixeira](https://github.com/DavideLacerdaT), [João Victor Fernandes da Silveira](https://github.com/oiotave) e [Josué Guedes Ferreira](https://github.com/JosueGuedes).

### Contribuições
* **Davi**: responsável pela implementação das threads e da lógica de leitura de linha, além da identificação de erros;
* **João Victor**: responsável pela implementação das threads e das funções de análise e divisão de threads;
* **Josué**: responsável pela implementação da lógica de partição do sudoku e da organização das threads.

## Instruções de compilação
Por usar comando da biblioteca ```pthreads.h```, o código acima só pode ser compilado e executado em sistemas operacionais como Linux e MacOS. Para usuários de Windows, é necessária a instalação de algum subsistema (como WSL para Linux) para a execução correta. Uma vez que tal requisito esteja garantido, pode-se compilar e executar o código da seguinte forma:
1. Compilar: ```g++ main.cpp -o sudoku```
2. Executar: ```./sudoku```

## Notas
Além do arquivo principal, consta no projeto uma pasta de arquivos txt com variados jogos de sudoku resolvidos. Alguns deles são soluções válidas, mas outras não, com propósito de teste. Soluções inválidas estão nomeadas com o padrão ```inv_sudoku```.