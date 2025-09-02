#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include <algorithm>
#include <pthread.h>
#include "sudoku.hpp"

using namespace std;

string sudoku_erros(81, '0');       // Armazena as posições que invalidam um jogo de sudoku
char   types[3] = {'l', 'c', 'q'};  // Armazena os tipos de cada parte (linha, coluna, quadrante)
int    types_ctrl;                  // Controle para acessar os índices do array de tipos

//      FUNCOES DE CODIGO
void    modifica_sudoku_erros(int, int, int, char);
bool    verifica_linha(string, int, char);
bool    validate_sudoku(Sudoku);
void*   validate_part(void*);
bool    single_thread(Sudoku);
bool    multithreads(Sudoku, int);
void    menu();

int main() {
    string caminho;
    cout << "\nDigite o arquivo com o sudoku: ";
    cin  >> caminho;

    Sudoku sudoku(caminho);
    sudoku.ler_arquivo();

    while(sudoku.jogo == "") {
        cout << "\nDigite um arquivo valido: ";
        cin  >> caminho;
        sudoku.caminho = caminho;
        sudoku.ler_arquivo();
    }
    sudoku.print();

    bool validez;
    int  escolha;

    while(true) {
        menu();
        cout << "\nEscolha uma forma de resolucao: ";
        cin  >> escolha;

        switch(escolha) {
            case 1: validez = single_thread(sudoku); break;
            case 2: validez = multithreads(sudoku, 3); break;
            case 3: validez = multithreads(sudoku, 9); break;
            case 4: validez = multithreads(sudoku, 27); break;
            case 0: exit(0);
            default: break;
        }
        if(!validez) {
            cout << "Os erros no sudoku escolhido:" << endl << endl;
            Sudoku::print(sudoku_erros);
        }
    }
    return 0;
}

/**
 *  Inicia a validação com uma única thread
 * 
 *  @param param Struct com os parâmetros de cada thread
 *  @return 'Verdadeiro' se for um sudoku válido, 'falso' caso contrário
 */
bool single_thread(Sudoku param) {
    clock_t ini = clock();
    bool validez = validate_sudoku(param);
    clock_t fim = clock() - ini;

    cout << "\nO jogo mostrado é " << (validez ? "valido" : "invalido") << endl;
    cout << "Operacao com 1 thread terminada em " << ((long double) fim / CLOCKS_PER_SEC) << " segundos" << endl << endl;
    return validez;
}

/**
 *  Inicia a validação com uma várias threads
 * 
 *  @param param Struct com os parâmetros de cada thread
 *  @param qtd Quantidade de threads (3, 9, 27)
 *  @return 'Verdadeiro' se for um sudoku válido, 'falso' caso contrário
 */
bool multithreads(Sudoku param, int qtd) {
    clock_t ini, fim;
    vector<pthread_t> threads; // Vetor para guardar as threads
    
    bool validez = true;
    bool *resultado; // Guarda o resultado de cada thread

    switch(qtd) {
    case 3:
        param.head = 0; // Cada thread processara uma das partes do sudoku (linha, coluna ou quadrante)
        param.tail = 8;
        threads.reserve(3);
        
        ini = clock();

        // Cria as threads
        for(int i = 0; i < 3; i++) {
            types_ctrl = i; // Altera o controle, mudando o tipo que sera acessado
            pthread_create(&threads[i], NULL, validate_part, (void*) &param);
        }
        for(int j = 0; j < 3; j++) {
            pthread_join(threads[j], (void**) &resultado);
            validez = validez && *resultado; // Verifica a validez de cada analise
        
            if(!validez) {
                free(resultado);
                break;
            }
            free(resultado);
        }
        fim = clock() - ini;      
        threads.clear();
        break;

    case 9:
        threads.reserve(9);
        ini = clock();

        for(int i = 0; i < 9; i++) {
            param.head = (i % 3) * 3; // Cada thread processara um terco de cada uma das partes
            param.tail = (i % 3) * 3 + 2;
            
            if(i < 3) { // Threads que verificarao cada conjunto de tres linhas
                types_ctrl = 0;
                pthread_create(&threads[i], NULL, validate_part, (void*) &param);
            }
            else if(i >= 3 && i < 6) { // Threads que verificarao cada conjunto de tres colunas
                types_ctrl = 1;
                pthread_create(&threads[i], NULL, validate_part, (void*) &param);
            }
            else { // Threads que verificarao cada conjunto de tres quadrantes
                types_ctrl = 2;
                pthread_create(&threads[i], NULL, validate_part, (void*) &param);
            }
        }
        for(int j = 0; j < 9; j++) {
            pthread_join(threads[j], (void**) &resultado);
            validez = validez && *resultado;

            if(!validez) {
                free(resultado);
                break;
            }
            free(resultado);
        }
        fim = clock() - ini;
        threads.clear();
        break;

    case 27:
        threads.reserve(27);
        ini = clock();
        
        for(int i = 0; i < 27; i++) {
            param.head = (i % 9); // Cada thread processara uma unidade de cada parte
            param.tail = param.head;
            
            if(i < 9) { // Threads que verificarao cada uma das linhas
                types_ctrl = 0;
                pthread_create(&threads[i], NULL, validate_part, (void*) &param);
            }
            else if(i >= 9 && i < 18) { // Threads que verificarao cada uma das colunas
                types_ctrl = 1;
                pthread_create(&threads[i], NULL, validate_part, (void*) &param);
            }
            else { // Threads que verificarao cada um dos quadrantes
                types_ctrl = 2;
                pthread_create(&threads[i], NULL, validate_part, (void*) &param);
            }
        }
        for(int j = 0; j < 27; j++) {
            pthread_join(threads[j], (void**) &resultado);
            validez = validez && *resultado;
        
            if(!validez) {
                free(resultado);
                break;
            }
            free(resultado);
        }
        fim = clock() - ini;
        threads.clear();
        break;

    default: break;
    }
    cout << "\nO jogo mostrado é " << (validez ? "valido" : "invalido") << endl;
    cout << "Operacao com " << qtd << " threads terminada em " << ((long double) fim / CLOCKS_PER_SEC) << " segundos" << endl << endl;
    return validez;
}

/**
 *  Altera a string "sudoku_erros" para registrar as posições onde foram encontrados erros
 *  e quais números causaram isso
 *
 *  @param numero Valor que invalidou o sudoku
 *  @param i Posição do valor dentro da parte (linha, coluna ou quadrante) analisada
 *  @param j Posição da parte no jogo (vai de 0 a 8)
 *  @param tipo Tipo da parte que estava em análise quando o erro foi percebido
 */
void modifica_sudoku_erros(int numero, int i, int j, char tipo) {
    if(tipo == 'l') sudoku_erros[j * 9 + i] = numero + '0';
    if(tipo == 'c') sudoku_erros[i * 9 + j] = numero + '0';
    if(tipo == 'q') sudoku_erros[((j / 3) * 27) + ((j % 3) * 3) + ((i / 3) * 9) + (i % 3)] = numero + '0';
}

/**
 *  Verifica se a parte (linha, coluna ou quadrante) é válida e atualiza a matriz de erros
 *  caso necessário
 * 
 *  @param linha Conjunto de números da parte
 *  @param coord Posição da parte no jogo (vai de 0 a 8)
 *  @param tipo Tipo da parte analisada
 *  @return 'Verdadeiro' se a parte for válida, 'falso' caso contrário
 */
bool verifica_linha(string linha, int coord, char tipo) {
    /*
     *   Esse array funciona como um hash: a posição 'n' indica que o número 'n + 1'
     *   não está nele se for igual a 0, ou que está nele se for igual a 1.
     */
    int array[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    /*
     *  Para cada posicao na linha recebida, verifica-se se o valor é válido e se a sua
     *  posição no hash já está marcada como 1. Se algum número novo não tiver sido lido ainda,
     *  ele será marcado como 1 no array.
     */
    for(int i = 0; i < 9; i++) {
        int n = linha[i] - '0';
        
        if(n < 1 || array[n - 1] > 0) {
            modifica_sudoku_erros(n, i, coord, tipo);
            modifica_sudoku_erros(n, array[n - 1] - 1, coord, tipo);
        }
        else array[n - 1] = i + 1;
    }
    int flag = 1;
    
    for(int i = 0; i < 9; i++) if(!array[i]) flag = 0;

    if(!flag) return false;
    return true;
}

/**
 *  Faz a validação do jogo completo
 * 
 *  @param tentativa A string com o jogo de sudoku completp
 *  @return 'Verdadeiro' se a parte for válida, 'falso' caso contrário
 * 
 *  @note Usado apenas para simulação single-thread
 */
bool validate_sudoku(Sudoku param) {
    types_ctrl = 0;

    for(int i = 0; i < 27; i++) {
        if(i > 8 && i < 18) types_ctrl = 1;
        
        else if(i > 17) types_ctrl = 2;
        string parte = Sudoku::slice_line(param.jogo, i % 9, types[types_ctrl]);
        
        if(!verifica_linha(parte, i % 9, types[types_ctrl])) return false;
    }
    return true;
}

/**
 *  Faz a validação da parte (linha, coluna ou quadrante)
 * 
 *  @param thread_params Parâmetros da thread existentes na struct
 *  @return 'Verdadeiro' se a parte for válida, 'falso' caso contrário
 */
void* validate_part(void* thread_params) {
    Sudoku* p = (Sudoku*) thread_params;
    bool* retorno = (bool*) malloc(sizeof(bool));
    
    *retorno = true;
    for(int i = p->head; i <= p->tail; i++) {
        string parte = Sudoku::slice_line(p->jogo, i, types[types_ctrl]);
        *retorno = *retorno && verifica_linha(parte, i, types[types_ctrl]);
    }
    pthread_exit((void*) retorno);
}

void menu() {
    cout << "(1)    Uma thread apenas" << endl;
    cout << "(2)    Três threads" << endl;
    cout << "(3)    Nove threads" << endl;
    cout << "(4)    Vinte e sete threads" << endl;
    cout << "(0)    Encerrar" << endl;
}

