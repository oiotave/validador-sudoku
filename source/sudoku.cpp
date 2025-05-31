#include <iostream>
#include <fstream>
#include <string>
#include "sudoku.hpp"

using namespace std;

Sudoku::Sudoku(string caminho) {
    this->caminho = caminho;
}

void Sudoku::ler_arquivo() {
    string sudoku = "";
    ifstream arquivo(caminho);

    if(!arquivo) cout << "\nErro ao abrir arquivo\n" << endl;

    else {
        char caractere;
        while(arquivo.get(caractere)) {
            if(caractere < 49 || caractere > 57) continue; // Ignora caracteres que não são números
            sudoku += caractere;
        }
        arquivo.close();
        this->jogo = sudoku;
    }
}

void Sudoku::print() {
    cout << "\n";
    for(int i = 1; i <= 81; i++) {
        cout << jogo[i - 1] << " ";
        
        if(i != 0) {
            if((i % 3 == 0) && (i % 9 != 0)) cout << "| ";
            
            if(i % 9 == 0) cout << endl;
            
            if((i % 27 == 0) && (i != 81)) {
                for(int j = 0; j < 22; j++) {
                    if((i % 3 == 0) && (i % 9 != 0)) cout << " -";
                    
                    else cout << "-";
                }
            cout << endl;
            }
        }
    }
    cout << endl;
}

void Sudoku::print(string jogo) {
    cout << "\n";
    for(int i = 1; i <= 81; i++) {
        cout << jogo[i - 1] << " ";
        
        if(i != 0) {
            if((i % 3 == 0) && (i % 9 != 0)) cout << "| ";
            
            if(i % 9 == 0) cout << endl;
            
            if((i % 27 == 0) && (i != 81)) {
                for(int j = 0; j < 22; j++) {
                    if((i % 3 == 0) && (i % 9 != 0)) cout << " -";
                    
                    else cout << "-";
                }
            cout << endl;
            }
        }
    }
    cout << endl;
}

/**
 * Extrai e isola uma parte do jogo (linha, coluna ou quadrante)
 * 
 * @param group Qual das nove partes será extraída
 * @param type Tipo de parte que será extraída
 * @return String com os dígitos daquela parte
 * 
 * @note Na variável 'type', 'l' é para linha, 'c' é para coluna e 'q' é para quadrante
 */
string Sudoku::slice_line(string jogo, int group, char type) {
    string resultado = "";

    switch(type) {
        case 'l':
            group *= 9;
            for(int j = 0; j < 9; j++) resultado += jogo[group + j];
            break;

        case 'c':
            for(int j = 0; j < 9; j++) resultado += jogo[group + j * 9];
            break;

        case 'q':
            if(group <= 2) group *= 3;
            
            else if(group >= 3 && group <= 5) group = (group % 3) * 3 + 27;
            
            else group = (group % 3) * 3 + 54;

            for(int j = 0; j < 9; j++) {
                resultado += jogo[group + j];
                
                if(j % 3 == 2) group += 6;
            }
            break;

        default: break;
    }
    return resultado;
}