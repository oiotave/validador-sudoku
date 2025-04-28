### Introdução

A seguinte implementanção é referente ao projeto final da disciplina de Sistemas Operacionais
da Universidade Federal da Paraíba (UFPB). Seu objetivo é apresentar uma análise sobre o uso de threads
em um algoritmo de validação de jogos de sudoku, escrito em C++. A implementação foi efetuado pelos seguintes
discente de Ciência da Computação:

* [Davi de Lacerda Teixeira](https://github.com/DavideLacerdaT) - responsável pela implementação das threads e da lógica de leitura de linha além de identificação dos erros;
* [João Victor Fernandes da Silveira](https://github.com/oiotave) - responsável pela implementação das threads e das funções de análise das threads;
* [Josué Guedes Ferreira](https://github.com/JosueGuedes) - responsável pela implementação da lógica de partição do sudoku e da organização das threads.

### Instruções de compilação

O código acima, por exigir o uso da biblioteca "<threads.h>", só pode ser compilada nativamente
em sistemas operacionais que suportam POSIX (Unix, Linux ou MacOS). Para o desenvolvimento do projeto,
foi usado o Ubuntu, baseado em Linux. Portanto, para esse sistema, os comandos de compilação e
execução seguem abaixo:

    g++ main.cpp -o sudoku
    ./sudoku

### Notas

Além do arquivo principal, consta no projeto uma pasta de arquivos txt com variados jogos de sudoku resolvidos.
Alguns deles são soluções válidas, mas outras não, como propósito de teste. Soluções inválidas estão nomeadas com
o padrão "inv_sudoku".
