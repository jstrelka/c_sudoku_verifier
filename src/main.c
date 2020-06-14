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
const char filename[] = "/home/jstrelka/Desktop/cs_3600_stuff/c_sudoku_verifier/src/SudokuPuzzle.txt";
int sudokuPuzzle[ROW][COLUMN];
bool columnBool[COLUMN], rowBool[ROW], gridBool[GRID];

typedef struct Bounds {
    int topRow;
    int bottomRow;
    int leftColumn;
    int rightColumn;
} Bounds;

void populateArray();
void print2DArray();
void verifySolution();
void initColStructs(struct Bounds columns[COLUMN]);
void initColThreads(pthread_t tid_column[COLUMN], struct Bounds columns[COLUMN]);
void *setColumnBool(void *param);

int main(){
        populateArray();
        print2DArray();
        verifySolution();
}

void populateArray() {
    FILE *fp;
    fp = fopen(filename, "r");
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

void verifySolution(){
    struct Bounds columns[COLUMN];
    pthread_t tid_column[COLUMN];

    initColStructs(columns);
    initColThreads(tid_column, columns);
}

void initColStructs(struct Bounds columns[COLUMN]){
    for (int i=0; i<COLUMN;i++) {
        Bounds Bound = {0, 8, i, i};
        columns[i] = Bound;
    }
}

void initColThreads(pthread_t tid_column[COLUMN], struct Bounds columns[COLUMN]) {
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
        printf("0x%x TRow: %d, BRow: %d, LCol: %d, RCol: %d invalid!\n",self, topRow, bottomRow, leftColumn, rightColumn, stdout);
    }
    else{
        columnBool[leftColumn] = true;
        printf("0x%x TRow: %d, BRow: %d, LCol: %d, RCol: %d valid!\n",self, topRow, bottomRow, leftColumn, rightColumn, stdout);
    }

    pthread_exit(0);
}



