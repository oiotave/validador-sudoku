#include <iostream>
#include <string>
#include <thread>
#include <future>
#include <ctime>
#include <locale>
#include <vector>
#include <fstream>
#include <algorithm>

using namespace std;

void    modifica_sudoku_erros(int, int, char);
bool    verifica_linha(string, int, char);
string  slice_line(string, int, char);
bool    validate_sudoku(string);
bool    validate_part(string, char, int, int);
void    printar_sudoku(string);
string  ler_arquivo(string);


string sudoku_erros(81, '0'); // Variável que armazena as posições que invalidam um jogo de sudoku

int main() {
    setlocale(LC_ALL, "pt_BR.utf-8");
    
    string caminho, sudoku;

    cout << "\nDigite o arquivo com o sudoku: ";
    cin  >> caminho;

    sudoku = ler_arquivo(caminho);

    cout << "\nSeu sudoku: " << endl << endl;
    printar_sudoku(sudoku);

    // Resolução com uma única thread

    clock_t ini = clock();

    bool validez = validate_sudoku(sudoku);
    
    clock_t fim = clock() - ini;

    cout << "O jogo mostrado é " << (validez ? "válido" : "inválido") << endl << endl;

    cout << "Operação com 1 thread terminada em " <<((long double) fim / CLOCKS_PER_SEC) << " segundos" << endl << endl;

    // Resolução com três threads

    ini = clock();

    future<bool> thread_linha = async(&validate_part, sudoku, 'l', 0, 8);
    future<bool> thread_coluna = async(&validate_part, sudoku, 'c', 0, 8);
    future<bool> thread_quadrante = async(&validate_part, sudoku, 'q', 0, 8);

    fim = clock() - ini;

    validez = thread_linha.get() && thread_coluna.get() && thread_quadrante.get(); // Incluir o get dentro do cronometro acrescenta +0.0004s

    cout << "Operação com 3 threads terminada em " << ((long double) fim / CLOCKS_PER_SEC) << " segundos" << endl << endl;

    // Resolução com nove threads

    vector<future<bool>> threads_linha, threads_coluna, threads_quadrante; // Arrays de threads

    ini = clock();

    for(int i = 0; i < 3; i++) {
        threads_linha.push_back(async(&validate_part, sudoku, 'l', i * 3, i * 3 + 2));
        threads_coluna.push_back(async(&validate_part, sudoku, 'c', i * 3, i * 3 + 2));
        threads_quadrante.push_back(async(&validate_part, sudoku, 'q', i * 3, i * 3 + 2));
    }

    fim = clock() - ini;
    
    for(int j = 0; j < 3; j++)
        validez = validez && threads_linha[j].get() && threads_coluna[j].get() && threads_quadrante[j].get();

    cout << "Operação com 9 threads terminada em " << ((long double) fim / CLOCKS_PER_SEC) << " segundos" << endl << endl;

    // Resolução com vinte e sete threads

    threads_linha.clear();
    threads_coluna.clear();
    threads_quadrante.clear();

    ini = clock();

    for(int i = 0; i < 9; i++) {
        threads_coluna.push_back(async(&validate_part, sudoku, 'c', i, i));
        threads_quadrante.push_back(async(&validate_part, sudoku, 'q', i, i));
        threads_linha.push_back(async(&validate_part, sudoku, 'l', i, i));
    }

    fim = clock() - ini;
    
    for(int j = 0; j < 9; j++)
        validez = validez && threads_linha[j].get() && threads_coluna[j].get() && threads_quadrante[j].get();

    cout << "Operação com 27 threads terminada em " << ((long double) fim / CLOCKS_PER_SEC) << " segundos" << endl << endl;

    // Em caso de erro, mostra a matriz de erros com as posições inválidas
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
 *  @param total A string com o jogo de sudoku completp
 *  @param type O tipo de parte analisada (linha, coluna ou quadrante)
 *  @param head A partir de que coordenada será feita a verificação
 *  @param tail Até qual coordenada será feita a verificação
 *  @return 'Verdadeiro' se a parte for válida, 'falso' caso contrário
 */
bool validate_part(string total, char type, int head, int tail) {  
    for(int i = head; i <= tail; i++) {
        string parte = slice_line(total, i, type);
        
        if(!verifica_linha(parte, i, type)) return false;
    }
    return true;
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
    string linha, sudoku;
    
    ifstream arquivo(caminho);

    if(!arquivo) cout << "\nErro ao abrir arquivo\n" << endl;

    else {
        while(getline(arquivo, linha)) sudoku += linha;

        arquivo.close();

        sudoku.erase(remove(sudoku.begin(), sudoku.end(), ' '), sudoku.end());
        sudoku.erase(remove(sudoku.begin(), sudoku.end(), '\t'), sudoku.end());
    
        return sudoku;
    }

    return "";
}
