/*
 * AUTHOR:      JUSTIN STRELKA
 * ASSIGNMENT:  HWK_03_SUDOKU_VERIFY
 * DATE:        6/11/20
 * LAST MOD:    6/11/20
*/

#define typedef int bool;
const string filename = "SudokuPuzzle.txt";
int sudokuPuzzle[9][9];


void populateArray(string filename, int *sudokuPuzzle[9][9]);


int main(){

    populateArray(filename, sudokuPuzzle);


}


void populateArray(string filepath, int *sudokuPuzzle[9][9]){
    FILE *fp;
    fp = fopen(filename, "r");
    int fscanf(fp)
}