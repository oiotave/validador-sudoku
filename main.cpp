#include <iostream>
#include <string>
#include <thread>
#include <future>
#include <ctime>

using namespace std;

string sudoku_erros = "000000000000000000000000000000000000000000000000000000000000000000000000000000000";

void modifica_sudoku_erros(int numero, int i, int j, char tipo)
{
    if (tipo == 'l')
        sudoku_erros[j * 9 + i] = numero + '0';
    if (tipo == 'c')
        sudoku_erros[i * 9 + j] = numero + '0';
    if (tipo == 'q')
        sudoku_erros[((j / 3) * 27) + ((j % 3) * 3) + ((i / 3) * 9) + (i % 3)] = numero + '0';
}

// Verifica se um jogo passado e valido
bool verifica_linha(string linha, int j, char tipo)
{
    /*
        Esse array funciona como um hash;
        a posicao n indica que o o numero n + 1 nao esta nele
        se for igual a 0 ou que esta nele se for igual a 1.
    */
    int array[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    /*
        Loop logico principal;
        para cada posicao na linha recebida, ele verifica se o valor
        e valido e se a sua posicao no hash ja esta marcada como 1,
        o que indicaria que o numero ja foi lido;
        se algum numero novo nao tiver sido lido ainda, ele sera
        marcado como 1 no array.
    */
    for (int i = 0; i < 9; i++)
    {
        int n = linha[i] - '0';
        if (n < 1 || array[n - 1] > 0)
        {
            modifica_sudoku_erros(n, i, j, tipo);
            modifica_sudoku_erros(n, array[n - 1] - 1, j, tipo);
        }
        else
            array[n - 1] = i + 1;
    }

    int flag = 1;
    
    for (int i = 0; i < 9; i++)
    {
        if (!array[i])
            flag = 0;
    }

    if(!flag)
        return false;
    
    return true;
}

string slice_line(string tentativa, int group, char type)
{
    /*
    > Entradas:
        - tentativa: string de 81 caracteres com todos os numeros, assume-se completa e desprovida de 0s
        - group: Qual das 9 linhas horizontais vai ser extraída. Vai de 0 a 8
        - type: l pra linha, c pra coluna e q pra quadrante

    > Saída:
        - string de tamanho 9 cujo é os digitos daquela quadrante/linha/coluna
    */
    string resultado = "";

    switch (type)
    {
    case 'l':
        group *= 9;
        for (int j = 0; j < 9; j++)
            resultado += tentativa[group + j];
        break;

    case 'c':
        for (int j = 0; j < 9; j++)
            resultado += tentativa[group + j * 9];
        break;

    case 'q':
        if (group <= 2)
            group *= 3;
        else if (group >= 3 && group <= 5)
            group = (group % 3) * 3 + 27;
        else
            group = (group % 3) * 3 + 54;

        for (int j = 0; j < 9; j++)
        {
            resultado += tentativa[group + j];
            if (j % 3 == 2)
                group += 6;
        }
        break;

    default:
        break;
    }

    return resultado;
}

bool validateSudoku(string tentativa)
{
    char type = 'l';
    for (int j = 0; j < 27; j++)
    {
        if (j >= 9 && j <= 17)
        {
            type = 'c';
            // cout << '\n';
        }
        else if (j > 17)
        {
            type = 'q';
            // cout << '\n';
        }

        string parte = slice_line(tentativa, j % 9, type);
        if (!verifica_linha(parte, j % 9, type))
        {
            cout << "INVALIDO!!\n\tTipo: " << type << "\n\tNúmero: " << j % 9;
            // return false;
        }
    }
    return true;
}

bool validatePart(string total, char type)
{
    for (int j = 0; j < 9; j++)
    {
        string parte = slice_line(total, j, type);
        if (!verifica_linha(parte, j, type))
        {
            cout << "INVALIDO!!\n\tTipo: " << type << "\n\tNúmero: " << j % 9;
            return false;
        }
    }
    return true;
}

void printar_sudoku(string sudoku) {
    for (int i = 1; i <= 81; i++) {
        cout << sudoku[i - 1] << " ";
        if (i != 0) {
            if (((i % 3) == 0) && ((i % 9) != 0))
                cout << "| ";
            if ((i % 9) == 0)
                cout << endl;
            if (((i % 27) == 0) && (i != 81)) {
                for (int j = 0; j < 22; j++) {
                    if (((i % 3) == 0) && ((i % 9) != 0))
                        cout << " _";
                    else
                        cout << "_";
                }
            cout << endl;
            }
        }
    }

    cout << endl;
}

int main()
{
    string sudoku = "534678912672195348198342567859761423426853791713924856961537284287419635345286179";

    cout << "Seu sudoku: " << endl << endl;

    printar_sudoku(sudoku);

    // Uma única thread
    // Validando linhas

    clock_t ini = clock();

    bool validez = validateSudoku(sudoku);

    clock_t fim = clock() - ini;
    float duracao = float(fim) / CLOCKS_PER_SEC;

    cout << "Operação terminada em " << duracao << "s\n";
    cout << "O jogo mostrado é " << validez;

    // 3 threads
    ini = clock();

    std::future<bool> thread_linha = std::async(&validatePart, sudoku, 'l');
    std::future<bool> thread_coluna = std::async(&validatePart, sudoku, 'c');
    std::future<bool> thread_quadrante = std::async(&validatePart, sudoku, 'q');

    fim = clock() - ini;
    duracao = float(fim) / CLOCKS_PER_SEC;
    validez = thread_linha.get() && thread_coluna.get() && thread_quadrante.get(); // Incluir o get dentro do cronometro acrescenta +0.0004s

    

    cout << "\n\nOperação com 3 threads terminada em " << duracao << "s\n";
    cout << "O jogo mostrado é " << validez << endl << endl;

    cout << "Os erros no sudoku escolhido:" << endl << endl;
    printar_sudoku(sudoku_erros);
    return 0;
}
