/*
 * AUTHOR:      JUSTIN STRELKA
 * ASSIGNMENT:  HWK_03_SUDOKU_VERIFY
 * DATE:        6/11/20
 * LAST MOD:    6/11/20
*/

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

#define ROW 9
#define COLUMN 9
#define GRID 9
//const char filename[] = "/home/jstrelka/Desktop/cs_3600_stuff/c_sudoku_verifier/src/SudokuPuzzle.txt";
int sudokuPuzzle[ROW][COLUMN];
bool columnBool[COLUMN], rowBool[ROW], subgridBool[GRID];
//pthread_t tid_column[COLUMN], tid_row[ROW], tid_subgrid[ROW];

typedef struct Bounds {
    int topRow;
    int bottomRow;
    int leftColumn;
    int rightColumn;
} Bounds;

void populateArray(char param[]);
void print2DArray();
void verifySolution(pthread_t tid_column[COLUMN], pthread_t tid_row[ROW], pthread_t tid_subgrid[ROW]);
//void verifySolution();
void initColStructs(struct Bounds columns[COLUMN]);
void initColThreads(pthread_t tid_column[COLUMN], struct Bounds columns[COLUMN]);
//void initColThreads(struct Bounds columns[COLUMN]);
void *setColumnBool(void *param);
void initRowStructs(struct Bounds rows[ROW]);
void initRowThreads(pthread_t tid_row[ROW], struct Bounds rows[ROW]);
//void initRowThreads(struct Bounds rows[ROW]);
void *setRowBool(void *param);
void initSubgridStructs(struct Bounds subgrids[GRID]);
void initSubgridThreads(pthread_t tid_subgrid[ROW], struct Bounds subgrids[ROW]);
//void initSubgridThreads(struct Bounds subgrids[ROW]);
void *setSubgridBool(void *param);
void boolTruePrint(pthread_t self, int topRow, int bottomRow, int leftColumn, int rightColumn);
void boolFalsePrint(pthread_t self, int topRow, int bottomRow, int leftColumn, int rightColumn);
void printColumnFinal(pthread_t tid_column[COLUMN]);
void printRowFinal(pthread_t tid_row[ROW]);
void printSubgridFinal(pthread_t tid_subgrid[ROW]);
void printSudokuFinal();

int main(int argc, char *argv[]){
    pthread_t tid_column[COLUMN];
    pthread_t tid_row[ROW];
    pthread_t tid_subgrid[ROW];

    if(argc < 1){
        printf("Please enter at least ONE SudokuPuzzle.txt file as a parameter in the command line.");
        return 1;
    }
    for (int i=1; i<argc; i++) {
        populateArray(argv[i]);
        print2DArray();
        verifySolution(tid_column, tid_row, tid_subgrid);
        printColumnFinal(tid_column);
        printRowFinal(tid_row);
        printSubgridFinal(tid_subgrid);
        printSudokuFinal();
    }
    return 0;
}

void populateArray(char param[]) {
    FILE *fp;
    fp = fopen(param, "r");
    int nums[COLUMN];

    if (fp == NULL) {
        printf("File open error!!!");
        return;
    } else {
        for (int i=0;i<ROW;i++) {
            fscanf(fp, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", &nums[0], &nums[1], &nums[2], &nums[3], &nums[4],
                   &nums[5], &nums[6], &nums[7], &nums[8]);
            for (int j = 0; j < COLUMN; j++) {
                sudokuPuzzle[i][j] = nums[j];
            }
        }
    }
    fclose(fp);
}

void print2DArray(){
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            printf("%d\t",sudokuPuzzle[i][j]);
        }
        printf("\n");
    }
}

//void verifySolution(){
void verifySolution(pthread_t tid_column[COLUMN], pthread_t tid_row[ROW], pthread_t tid_subgrid[ROW]){
    struct Bounds columns[COLUMN];
    //pthread_t tid_column[COLUMN];
    struct Bounds rows[ROW];
    //pthread_t tid_row[ROW];
    struct Bounds subgrids[ROW];
    //pthread_t tid_subgrid[ROW];

    initColStructs(columns);
    initColThreads(tid_column, columns);
    //initColThreads(columns);

    initRowStructs(rows);
    initRowThreads(tid_row, rows);
    //initRowThreads(rows);

    initSubgridStructs(subgrids);
    initSubgridThreads(tid_subgrid, subgrids);
    //initSubgridThreads(subgrids);

    for (int i=0; i<COLUMN; i++) {
        pthread_join(tid_column[i],NULL);
        pthread_join(tid_row[i],NULL);
        pthread_join(tid_subgrid[i],NULL);
    }
}

void initColStructs(struct Bounds columns[COLUMN]){
    for (int i=0; i<COLUMN;i++) {
        Bounds Bound = {0, 8, i, i};
        columns[i] = Bound;
    }
}

void initColThreads(pthread_t tid_column[COLUMN], struct Bounds columns[COLUMN]) {
//void initColThreads(struct Bounds columns[COLUMN]) {
    pthread_attr_t attr[COLUMN];
    pthread_t tid[COLUMN];

    for (int i=0; i<COLUMN; i++){
        pthread_attr_init(&(attr[i]));
    }

    for (int i=0; i<COLUMN;i++) {
        pthread_create(&(tid[i]), &(attr[i]), setColumnBool, &columns[i]);
        tid_column[i] = tid[i];
    }

    for (int i=0; i<COLUMN; i++) {
        pthread_join(tid[i],NULL);
    }
}

void *setColumnBool(void *param) {

    Bounds *inP;
    int topRow, bottomRow, leftColumn, rightColumn;
    pthread_t self;
    int columnValues[COLUMN];
    int repeatedValues;

    inP = (Bounds *)param;
    topRow = inP->topRow;
    bottomRow = inP->bottomRow;
    leftColumn = inP->leftColumn;
    rightColumn = inP->rightColumn;
    self = pthread_self();
    repeatedValues = 0;

    for (int i=0; i<ROW; i++){
        columnValues[i] = sudokuPuzzle[i][leftColumn];
    }

    for (int i=0; i<ROW; i++){
        if(columnValues[i] >=1 && columnValues[i]<=9) {
            for (int j = i + 1; j < ROW; j++) {
                if (columnValues[i] == columnValues[j]){
                    repeatedValues++;
                    break;
                }
            }
        }
    }

    if(repeatedValues > 0){
        columnBool[leftColumn] = false;
        boolFalsePrint(self, topRow, bottomRow, leftColumn, rightColumn);
        //printf("0x%x TRow: %d, BRow: %d, LCol: %d, RCol: %d invalid!\n",self, topRow, bottomRow, leftColumn, rightColumn, stdout);
    }
    else{
        columnBool[leftColumn] = true;
        boolTruePrint(self, topRow, bottomRow, leftColumn, rightColumn);
        //printf("0x%x TRow: %d, BRow: %d, LCol: %d, RCol: %d valid!\n",self, topRow, bottomRow, leftColumn, rightColumn, stdout);
    }

    pthread_exit(0);
}

void initRowStructs(struct Bounds rows[ROW]) {
    for (int i=0; i<ROW;i++) {
        Bounds Bound = {i, i, 0, 8};
        rows[i] = Bound;
    }
}

void initRowThreads(pthread_t *tid_row, struct Bounds rows[ROW]) {
//void initRowThreads(struct Bounds rows[ROW]) {
    pthread_attr_t attr[ROW];
    pthread_t tid[ROW];

    for (int i=0; i<ROW; i++){
        pthread_attr_init(&(attr[i]));
    }

    for (int i=0; i<ROW;i++) {
        pthread_create(&(tid[i]), &(attr[i]), setRowBool, &rows[i]);
        tid_row[i] = tid[i];
    }

    for (int i=0; i<ROW; i++) {
        pthread_join(tid[i],NULL);
    }
}

void *setRowBool(void *param) {
    Bounds *inP;
    int topRow, bottomRow, leftColumn, rightColumn;
    pthread_t self;
    int rowValues[ROW];
    int repeatedValues;

    inP = (Bounds *)param;
    topRow = inP->topRow;
    bottomRow = inP->bottomRow;
    leftColumn = inP->leftColumn;
    rightColumn = inP->rightColumn;
    self = pthread_self();
    repeatedValues = 0;

    for (int i=0; i<COLUMN; i++){
        rowValues[i] = sudokuPuzzle[i][topRow];
    }

    for (int i=0; i<COLUMN; i++){
        if(rowValues[i] >=1 && rowValues[i]<=9) {
            for (int j = i + 1; j < COLUMN; j++) {
                if (rowValues[i] == rowValues[j]){
                    repeatedValues++;
                    break;
                }
            }
        }
    }

    if(repeatedValues > 0){
        rowBool[topRow] = false;
        boolFalsePrint(self, topRow, bottomRow, leftColumn, rightColumn);
        //printf("0x%x TRow: %d, BRow: %d, LCol: %d, RCol: %d invalid!\n",self, topRow, bottomRow, leftColumn, rightColumn, stdout);
    }
    else{
        rowBool[topRow] = true;
        boolTruePrint(self, topRow, bottomRow, leftColumn, rightColumn);
        //printf("0x%x TRow: %d, BRow: %d, LCol: %d, RCol: %d valid!\n",self, topRow, bottomRow, leftColumn, rightColumn, stdout);
    }

    pthread_exit(0);
}

void initSubgridStructs(struct Bounds *subgrids) {
    int topRow, bottomRow, leftColumn, rightColumn;

    for (int i=0; i<GRID;i++) {
        if (i == 0 || i == 1 || i == 2){
            topRow = 0;
            bottomRow = 2;
            if (i == 0) {
                leftColumn = 0;
                rightColumn = 2;
            }
            else if  (i == 1){
                leftColumn = 3;
                rightColumn =5;
            }
            else if (i == 2){
                leftColumn = 6;
                rightColumn = 8;
            }
        }
        else if (i == 3 || i == 4 || i == 5){
            topRow = 3;
            bottomRow = 5;
            if (i == 3) {
                leftColumn = 0;
                rightColumn = 2;
            }
            else if  (i == 4){
                leftColumn = 3;
                rightColumn =5;
            }
            else if (i == 5){
                leftColumn = 6;
                rightColumn = 8;
            }
        }
        else {
            topRow = 6;
            bottomRow = 8;
            if (i == 6) {
                leftColumn = 0;
                rightColumn = 2;
            }
            else if  (i == 7){
                leftColumn = 3;
                rightColumn =5;
            }
            else if (i == 8){
                leftColumn = 6;
                rightColumn = 8;
            }
        }

        Bounds Bound = {topRow, bottomRow, leftColumn, rightColumn};
        subgrids[i] = Bound;
    }
}

void initSubgridThreads(pthread_t *tid_subgrid, struct Bounds *subgrids) {
//void initSubgridThreads(struct Bounds *subgrids) {
    pthread_attr_t attr[GRID];
    pthread_t tid[GRID];

    for (int i=0; i<GRID; i++){
        pthread_attr_init(&(attr[i]));
    }

    for (int i=0; i<GRID;i++) {
        pthread_create(&(tid[i]), &(attr[i]), setSubgridBool, &subgrids[i]);
        tid_subgrid[i] = tid[i];
    }

    for (int i=0; i<GRID; i++) {
        pthread_join(tid[i],NULL);
    }
}

void *setSubgridBool(void *param) {
    Bounds *inP;
    int topRow, bottomRow, leftColumn, rightColumn;
    pthread_t self;
    int subgridValues[GRID];
    int element;
    int repeatedValues;

    inP = (Bounds *)param;
    topRow = inP->topRow;
    bottomRow = inP->bottomRow;
    leftColumn = inP->leftColumn;
    rightColumn = inP->rightColumn;
    self = pthread_self();
    repeatedValues = 0;
    element = 0;

    for (int i=topRow; i<=bottomRow; i++){
        for (int j=leftColumn; j<=rightColumn; j++) {
            subgridValues[element] = sudokuPuzzle[i][j];
            element++;
        }
    }

    for (int i=0; i<GRID; i++){
        if(subgridValues[i] >=1 && subgridValues[i]<=9) {
            for (int j = i + 1; j < GRID; j++) {
                if (subgridValues[i] == subgridValues[j]){
                    repeatedValues++;
                    break;
                }
            }
        }
    }

    if(repeatedValues > 0){
        if(topRow == 0 && bottomRow == 2) {
            if(leftColumn == 0 && rightColumn == 2) {
                subgridBool[0] = false;
                boolFalsePrint(self, topRow, bottomRow, leftColumn, rightColumn);
            }
            else if (leftColumn == 3 && rightColumn == 5){
                subgridBool[1] = false;
                boolFalsePrint(self, topRow, bottomRow, leftColumn, rightColumn);
            }
            else if (leftColumn == 6 && rightColumn == 8){
                subgridBool[2] = false;
                boolFalsePrint(self, topRow, bottomRow, leftColumn, rightColumn);
            }
        }
        else if(topRow == 3 && bottomRow == 5){
            if(leftColumn == 0 && rightColumn == 2) {
                subgridBool[3] = false;
                boolFalsePrint(self, topRow, bottomRow, leftColumn, rightColumn);
            }
            else if (leftColumn == 3 && rightColumn == 5){
                subgridBool[4] = false;
                boolFalsePrint(self, topRow, bottomRow, leftColumn, rightColumn);
            }
            else if (leftColumn == 6 && rightColumn == 8){
                subgridBool[5] = false;
                boolFalsePrint(self, topRow, bottomRow, leftColumn, rightColumn);
            }
        }
        else if(topRow == 6 && bottomRow == 8){
            if(leftColumn == 0 && rightColumn == 2) {
                subgridBool[6] = false;
                boolFalsePrint(self, topRow, bottomRow, leftColumn, rightColumn);
            }
            else if (leftColumn == 3 && rightColumn == 5){
                subgridBool[7] = false;
                boolFalsePrint(self, topRow, bottomRow, leftColumn, rightColumn);
            }
            else if (leftColumn == 6 && rightColumn == 8){
                subgridBool[8] = false;
                boolFalsePrint(self, topRow, bottomRow, leftColumn, rightColumn);
            }
        }
    }
    else{
        if(topRow == 0 && bottomRow == 2) {
            if(leftColumn == 0 && rightColumn == 2) {
                subgridBool[0] = true;
                boolTruePrint(self, topRow, bottomRow, leftColumn, rightColumn);
            }
            else if(leftColumn == 3 && rightColumn == 5) {
                subgridBool[1] = true;
                boolTruePrint(self, topRow, bottomRow, leftColumn, rightColumn);
            }
            else if(leftColumn == 6 && rightColumn == 8) {
                subgridBool[2] = true;
                boolTruePrint(self, topRow, bottomRow, leftColumn, rightColumn);
            }
        }
        else if(topRow == 3 && bottomRow == 5) {
            if(leftColumn == 0 && rightColumn == 2) {
                subgridBool[3] = true;
                boolTruePrint(self, topRow, bottomRow, leftColumn, rightColumn);
            }
            else if(leftColumn == 3 && rightColumn == 5) {
                subgridBool[4] = true;
                boolTruePrint(self, topRow, bottomRow, leftColumn, rightColumn);
            }
            else if(leftColumn == 6 && rightColumn == 8) {
                subgridBool[5] = true;
                boolTruePrint(self, topRow, bottomRow, leftColumn, rightColumn);
            }
        }
        else if(topRow == 6 && bottomRow == 8) {
            if(leftColumn == 0 && rightColumn == 2) {
                subgridBool[6] = true;
                boolTruePrint(self, topRow, bottomRow, leftColumn, rightColumn);
            }
            else if(leftColumn == 3 && rightColumn == 5) {
                subgridBool[7] = true;
                boolTruePrint(self, topRow, bottomRow, leftColumn, rightColumn);
            }
            else if(leftColumn == 6 && rightColumn == 8) {
                subgridBool[8] = true;
                boolTruePrint(self, topRow, bottomRow, leftColumn, rightColumn);
            }
        }
    }
    pthread_exit(0);
}

void boolTruePrint(pthread_t self, int topRow, int bottomRow, int leftColumn, int rightColumn) {
    printf("0x%lx TRow: %d, BRow: %d, LCol: %d, RCol: %d valid!\n",self, topRow, bottomRow, leftColumn, rightColumn, stdout);
}

void boolFalsePrint(pthread_t self, int topRow, int bottomRow, int leftColumn, int rightColumn) {
    printf("0x%lx TRow: %d, BRow: %d, LCol: %d, RCol: %d invalid!\n", self, topRow, bottomRow, leftColumn, rightColumn, stdout);
}

void printColumnFinal(pthread_t tid_column[COLUMN]){
    for (int i=0; i<COLUMN; i++){
        printf("Column: 0x%lx %s\n", tid_column[i], columnBool[i] ? "valid" : "invalid");
    }
}

void printRowFinal(pthread_t tid_row[ROW]){
    for (int i=0; i<ROW; i++){
        printf("Row: 0x%lx %s\n", tid_row[i], rowBool[i] ? "valid" : "invalid");
    }
}

void printSubgridFinal(pthread_t tid_subgrid[ROW]) {
    for (int i=0; i<GRID; i++){
        printf("Subgrid: 0x%lx %s\n", tid_subgrid[i], subgridBool[i] ? "valid" : "invalid");
    }
}

void printSudokuFinal() {
    bool valid = true;
    for (int i=0; i<COLUMN; i++){
        if (columnBool[i] == false || rowBool[i] == false || subgridBool[i] == false){
            valid = false;
        }
    }
    printf("Sudoku Puzzle: %s\n", valid ? "valid" : "invalid");
}
