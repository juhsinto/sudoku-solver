/* 
 * File:   Sudoku.h
 * Author: Jm
 *
 * Created on September 9, 2014, 11:32 PM
 */

#ifndef SUDOKU_H
#define	SUDOKU_H
#include <vector>
#include <iostream>
using namespace std;

class Sudoku
{
    public:

    Sudoku(const vector< vector<int> > &input)
    {
        //assign puzzle to be inputPuzzle
        puzzle = input;

        Sudoku::BOX_SIZE = 3;
        Sudoku::GRID_MAXSIZE = 9;
    }

    void printPuzzle()
    {
        for (unsigned int i = 0; i < puzzle.size(); i++)
        {
            for (unsigned int j = 0; j < puzzle[i].size(); j++)
            {
                cout << puzzle[i][j];
            }
            cout << "\n";
        }
    }

    bool solve(int row, int col)
    {
        // if completed solving for current row
        if (row == GRID_MAXSIZE)
        {
            // reset to head of row
            row = 0;
            // if completed solving for current row and last column
            if (++col == GRID_MAXSIZE)
            {
                return true;
            }
        }

        // skip filled cells
        if (puzzle[row][col] != 0)
        {
            // recursive call to solve, with increment in position
            return solve(row + 1, col);
        }

        // Brute-Force 1-9 through for puzzle
        for (int val = 1; val <= GRID_MAXSIZE; ++val)
        {
            // check if BF value is legal
            if (isLegal(val, row, col))
            {
                puzzle[row][col] = val;
                // recursive call to solve, with increment in position
                if (solve(row + 1, col))
                {
                    return true;
                }
            }
        }

        // reset on backtracking
        puzzle[row][col] = 0;
        return false;
    }

private:
    int BOX_SIZE;
    int GRID_MAXSIZE;

    vector< vector<int> > puzzle;

    bool isLegal(int num, int row, int col)
    {
        int r = (row / BOX_SIZE) * BOX_SIZE;
        int c = (col / BOX_SIZE) * BOX_SIZE;

        for (int i = 0; i < BOX_SIZE; i++)
        {

            if (// check row for duplicate
                    this->getCell(row, i) == num ||
                    // check col for duplicate
                    this->getCell(i, col) == num ||
                    // check 3x3 box for duplicate -> http://pastebin.com/QGRE2243
                    this->getCell(r + (i % BOX_SIZE), c + (i / BOX_SIZE)) == num)
            {
                return false;
            }
        }
        return true;
    }

    int getCell(int row, int col)
    {
        return puzzle[row][col];
    }



};

#endif	/* SUDOKU_H */

