#include <iostream>
#include <string>
#include <thread>
#include <ctime>

using namespace std;

// Verifica se um jogo passado e valido
bool validate(string linhaTentativa, char type)
{
    cout << linhaTentativa << "\tTipo: " << type << "\n";

    for (int j = 0; j < 9; j++)
    {
        if (linhaTentativa.find(linhaTentativa[j]) != linhaTentativa.rfind(linhaTentativa[j]))
            return false;
    }
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
            cout << '\n';
        }
        else if (j > 17)
        {
            type = 'q';
            cout << '\n';
        }

        string parte = slice_line(tentativa, j % 9, type);
        if (!validate(parte, type))
        {
            cout << "INVALIDO!!\n\tTipo: " << type << "\n\tNúmero: " << j % 9;
            return false;
        }
    }
    return true;
}

int main()
{
    string sudoku = "534678912672195348198342567859761423426853791713924856961537284287419635345286179";
    clock_t ini = clock();

    // Uma única thread
    // Validando linhas
    bool validez = validateSudoku(sudoku);
    clock_t fim = clock() - ini;
    float duracao = float(fim) / CLOCKS_PER_SEC;

    cout << "\nOperação terminada em " << duracao << "s\n";
    cout << "O jogo mostrado é " << validez;

    /*
        Sugestões de comparações e números de threads:
        - 3 threads, um para cada tipo de string (linhas, colunas, quadrantes) --> 9 operações para cada thread
        - 27 threads, uma operação apenas para cada string
    */

    return 0;
}