#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include <fstream>
#include <algorithm>
#include <pthread.h>

using namespace std;

void    modifica_sudoku_erros(int, int, char);
bool    verifica_linha(string, int, char);
string  slice_line(string, int, char);
bool    validate_sudoku(string);
void*   validate_part(void*);
void    printar_sudoku(string);
string  ler_arquivo(string); 

 // Struct que guarda os parametros passados para as threads
    typedef struct params {
        string sudoku;
        int    head;
        int    tail;
    } params;

 // Variaveis globais
    string sudoku_erros(81, '0');       // Armazena as posições que invalidam um jogo de sudoku
    char   types[3] = {'l', 'c', 'q'};  // Armazena os tipos de cada parte (linha, coluna, quadrante)
    int    types_ctrl;                  // Controle para acessar os índices do array de tipos

int main() {
    string caminho;
    params params;

    // Le o arquivo com o jogo de sudoku
    while(params.sudoku == "") {
        cout << "\nDigite o arquivo com o sudoku: ";
        cin  >> caminho;

        params.sudoku = ler_arquivo(caminho);
    }

    cout << "\nSeu sudoku: " << endl << endl;
    printar_sudoku(params.sudoku);

    // RESOLUCAO SINGLE THREAD/PROCESS

    clock_t ini = clock();
    
    bool validez = validate_sudoku(params.sudoku);

    clock_t fim = clock() - ini;

    cout << "O jogo mostrado é " << (validez ? "valido" : "invalido") << endl << endl;

    cout << "Operacao com 1 thread terminada em " <<((long double) fim / CLOCKS_PER_SEC) << " segundos" << endl << endl;

    // RESOLUCAO COM TRES THREADS

    params.head = 0; // Cada thread processara uma parte especifica
    params.tail = 8;

    vector<pthread_t> threads; // Vetor para guardar as threads
    threads.reserve(3);

    bool *resultado;

    ini = clock();
    
    for(int i = 0; i < 3; i++) {
        types_ctrl = i; // Altera o controle, mudando o tipo que sera acessado

        pthread_create(&threads[i], NULL, validate_part, (void*) &params);
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

    cout << "Operacao com 3 threads terminada em " << ((long double) fim / CLOCKS_PER_SEC) << " segundos" << endl << endl;

    threads.clear();

    // RESOLUCAO COM NOVE THREADS

    threads.reserve(9);

    ini = clock();
    
    for(int i = 0; i < 9; i++) {
        params.head = (i % 3) * 3; // Cada thread processara um terco de cada uma das partes
        params.tail = (i % 3) * 3 + 2;
        
        if(i < 3) { // Threads que verificarao cada conjunto de tres linhas
            types_ctrl = 0;
            pthread_create(&threads[i], NULL, validate_part, (void*) &params);
        }
        else if(i >= 3 && i < 6) { // Threads que verificarao cada conjunto de tres colunas
            types_ctrl = 1;
            pthread_create(&threads[i], NULL, validate_part, (void*) &params);
        }
        else { // Threads que verificarao cada conjunto de tres quadrantes
            types_ctrl = 2;
            pthread_create(&threads[i], NULL, validate_part, (void*) &params);
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

    cout << "Operacao com 9 threads terminada em " << ((long double) fim / CLOCKS_PER_SEC) << " segundos" << endl << endl;

    threads.clear();

    // RESOLUCAO COM VINTE E SETE THREADS

    threads.reserve(27);

    ini = clock();
    
    for(int i = 0; i < 27; i++) {
        params.head = (i % 9); // Cada thread processara uma unidade de cada parte
        params.tail = params.head;
        
        if(i < 9) { // Threads que verificarao cada uma das linhas
            types_ctrl = 0;
            pthread_create(&threads[i], NULL, validate_part, (void*) &params);
        }
        else if(i >= 9 && i < 18) { // Threads que verificarao cada uma das colunas
            types_ctrl = 1;
            pthread_create(&threads[i], NULL, validate_part, (void*) &params);
        }
        else { // Threads que verificarao cada um dos quadrantes
            types_ctrl = 2;
            pthread_create(&threads[i], NULL, validate_part, (void*) &params);
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

    cout << "Operacao com 27 threads terminada em " << ((long double) fim / CLOCKS_PER_SEC) << " segundos" << endl << endl;

    threads.clear();

    // Mostra os numeros que causaram invalidez no jogo passado
    if(!validez) {
        cout << "Os erros no sudoku escolhido:" << endl << endl;
        printar_sudoku(sudoku_erros);
    }

    return 0;
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
        else
            array[n - 1] = i + 1;
    }

    int flag = 1;
    
    for(int i = 0; i < 9; i++) if(!array[i]) flag = 0;

    if(!flag) return false;
    
    return true;
}

/**
 * Extrai e isola uma parte do jogo (linha, coluna ou quadrante)
 * 
 * @param tentativa String com todos os números do jogo
 * @param group Qual das nove partes será extraída
 * @param type Tipo de parte que será extraída
 * @return String com os dígitos daquela parte
 * 
 * @note Na variável 'type', 'l' é para linha, 'c' é para coluna e 'q' é para quadrante
 */
string slice_line(string tentativa, int group, char type) {
    string resultado = "";

    switch(type) {
        case 'l':
            group *= 9;
            for(int j = 0; j < 9; j++) resultado += tentativa[group + j];
            break;

        case 'c':
            for(int j = 0; j < 9; j++) resultado += tentativa[group + j * 9];
            break;

        case 'q':
            if(group <= 2) group *= 3;
            
            else if(group >= 3 && group <= 5) group = (group % 3) * 3 + 27;
            
            else group = (group % 3) * 3 + 54;

            for(int j = 0; j < 9; j++) {
                resultado += tentativa[group + j];
                
                if(j % 3 == 2) group += 6;
            }
            break;

        default:
            break;
    }

    return resultado;
}

/**
 *  Faz a validação do jogo completo
 * 
 *  @param tentativa A string com o jogo de sudoku completp
 *  @return 'Verdadeiro' se a parte for válida, 'falso' caso contrário
 * 
 *  @note Usado apenas para simulação single-thread
 */
bool validate_sudoku(string tentativa) {
    char type = 'l';
    
    for(int j = 0; j < 27; j++) {
        if(j >= 9 && j <= 17) type = 'c';
        
        else if(j > 17) type = 'q';

        string parte = slice_line(tentativa, j % 9, type);
        
        if(!verifica_linha(parte, j % 9, type)) return false;
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
    params* p = (params*) thread_params;
    
    bool* retorno = (bool*) malloc(sizeof(bool));

    *retorno = true;

    for(int i = p->head; i <= p->tail; i++) {
        string parte = slice_line(p->sudoku, i, types[types_ctrl]);
        
        *retorno = *retorno && verifica_linha(parte, i, types[types_ctrl]);
    }

    pthread_exit((void*) retorno);
}

/**
 *  Mostra o jogo de sudoku na tela
 * 
 *  @param sudoku String que contém os números do jogo
 */
void printar_sudoku(string sudoku) {
    for(int i = 1; i <= 81; i++) {
        cout << sudoku[i - 1] << " ";
        
        if(i != 0) {
            if((i % 3 == 0) && (i % 9 != 0)) cout << "| ";
            
            if(i % 9 == 0) cout << endl;
            
            if((i % 27 == 0) && (i != 81)) {
                for(int j = 0; j < 22; j++) {
                    if((i % 3 == 0) && (i % 9 != 0)) cout << " _";
                    
                    else cout << "_";
                }
            cout << endl;
            }
        }
    }
    cout << endl;
}

/**
 *  Faz a leitura do arquivo com o jogo de sudoku
 * 
 *  @param caminho Diretório do arquivo com o jogo
 *  @return Uma string com os elementos do jogo, ou uma string vazia em caso de erro de leitura
 */
string ler_arquivo(string caminho) {
    string sudoku = "";
    
    ifstream arquivo(caminho);

    if(!arquivo) cout << "\nErro ao abrir arquivo\n" << endl;

    else {
        char caractere;

        while(arquivo.get(caractere)) {
            if(caractere < 49 || caractere > 57) continue;
            sudoku += caractere;
        }
        arquivo.close();

        return sudoku;
    }
    return "";
}