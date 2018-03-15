// CMPUT 274 FINAL PROJECT HEADER FILE

#ifndef HEADER_H
#define HEADER_H

struct mine {
  int xLoc;
    int yLoc;
};

struct number{
  int xLoc;
  int yLoc;
  int value;
};




void setup();
void displayTitle();
void minesweeper();
void placemines(int genNum);
void displayGrid();
void redrawCursor(int x, int y);
void clearSpaces();
bool isMine(int blockSelecX,int blockSelecY);
void assignNums();
void clearBlock(int i, int j);
void clearSpaces(int i, int j);
void showNum(int i, int j);
void clearSurrounding(int i, int j);
int jToX(int j);
int iToY(int i);
int xToJ(int x);
int yToI(int y);
bool checkWin();
void displayWin();
void diffSelect();
int custom();
void displayLose();


#endif
