#ifndef SUDOKU_HPP
#define SUDOKU_HPP

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class Sudoku {
    public:
        string caminho;
        string jogo;
        int head;
        int tail;

        Sudoku(string);
        void ler_arquivo();
        void print();
        static void print(string);
        static string slice_line(string, int, char);
};
#endif // SUDOKU_HPP