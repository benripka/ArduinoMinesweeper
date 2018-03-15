//CMPUT 274 FINAL PROJECT FUNCTION FILE

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_ILI9341.h>
#include "header.h"
#include <TouchScreen.h>
#include <string.h>




//CONSTANT VARIABLES

#define TFT_DC 9
#define TFT_CS 10
#define JOY_SEL   2

#define DISPLAY_WIDTH  320
#define DISPLAY_HEIGHT 240
#define JOY_VERT  A1
#define JOY_HORIZ A0
#define JOY_SEL   2
#define JOY_CENTER   512
#define JOY_DEADZONE 64
#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940
#define JOY_CENTER   512
#define JOY_DEADZONE 64
#define FLAG_BUTTON 12


#define DIFFLOCATIONY DISPLAY_HEIGHT/2
#define EASYLOCATIONX 40
#define MEDLOCATIONX 100
#define HARDLOCATIONX    150
#define CUSTOMLOCATIONX    210

#define POTENTIOMETER A4


int blocks[13][17][2] = {0};

//GLOBAL VARIABLES
float version = 1.01;//for keeping track of who had what version of the
//game we made a global variable that stored this value

int difficulty;// will hold the number of mines

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

mine MINE[81];//mine structure with placeholder values

void setup() {

	init();//Initializing low level Arduino functions

	Serial.begin(9600);//Initializing Serial Monitor for debugging

	pinMode(JOY_SEL, INPUT_PULLUP);
	pinMode(FLAG_BUTTON, INPUT_PULLUP);


	tft.begin();//initialize LCD screen functionality

	tft.setRotation(3);

	Serial.println("Setup Complete... Launching... ");







}

//simply displays the title of the game along with
void displayTitle(){
	tft.fillScreen(ILI9341_WHITE);
	tft.fillRect(4,4,DISPLAY_WIDTH-8,DISPLAY_HEIGHT-8,ILI9341_BLACK);
	tft.fillRect(8,8,DISPLAY_WIDTH - 16, DISPLAY_HEIGHT - 16, ILI9341_WHITE);
	//above lines add a fancy border
	tft.setTextSize(3);
	tft.setTextColor(ILI9341_RED);
	tft.setCursor(DISPLAY_WIDTH/5-8,15);
	tft.print("Minesweeper!");//displaying title
	tft.setTextSize(2);
	tft.setCursor(60,50);//move cursor
	tft.setTextColor(ILI9341_RED);
	tft.print("By the COMPE Boys ");

}

void diffSelect(){
	tft.fillRect(20, DISPLAY_HEIGHT/3, 280,100,ILI9341_BLACK);
	tft.fillRect(23, DISPLAY_HEIGHT/3+3, 274, 94, ILI9341_WHITE);
	//add another rectangle box that will contain the difficulty settings
	tft.setTextSize(2);
	tft.setTextColor(ILI9341_BLACK);
	tft.setCursor(EASYLOCATIONX, DIFFLOCATIONY);
	tft.print("EASY");
	tft.setCursor(MEDLOCATIONX, DIFFLOCATIONY);
	tft.print("MED");
	tft.setCursor(HARDLOCATIONX, DIFFLOCATIONY);
	tft.print("HARD");
	tft.setCursor(CUSTOMLOCATIONX, DIFFLOCATIONY);
	tft.print("CUSTOM");
	//above lines print the difficulty levels


	int selection = 1;//stores the value of the difficulty selected
	int diffArr[4] ={EASYLOCATIONX, MEDLOCATIONX, HARDLOCATIONX, CUSTOMLOCATIONX};
	//used for redrawing the words after they have been selected
	int lengthOfWord[4] = {53, 42, 53, 78};//so the black box covers each word
	//which is a different size
	String word[4] = {"EASY", "MED", "HARD", "CUSTOM"};//holds the value of the
	//word

	tft.setTextColor(ILI9341_WHITE);
	tft.fillRect(diffArr[selection] - 4, DIFFLOCATIONY - 4,lengthOfWord[selection], 21 ,ILI9341_BLACK);
	tft.setCursor(diffArr[selection], DIFFLOCATIONY);
	tft.print(word[selection]);
	//default selection difficulty is medium


	while(true){
		int joyVal = analogRead(JOY_HORIZ);
		int buttonVal = digitalRead(JOY_SEL);
		//reading joystick values
		delay(100);//helps to slow down and make the selection process more user
		//freindly

		if(joyVal > JOY_CENTER + JOY_DEADZONE && selection != 0){
			/*when a user wants to select a difficulty, they use the cursor to move between
			blocks, each block corresponds to a different difficulty, when the
			user moves one to the left/or right a new selection is stored and the
			old word is redrawn on a white background in black test colour */
			selection--;
			tft.setTextColor(ILI9341_WHITE);
			tft.fillRect(diffArr[selection] - 4, DIFFLOCATIONY - 4,lengthOfWord[selection], 21 ,ILI9341_BLACK);
			tft.setCursor(diffArr[selection], DIFFLOCATIONY);
			tft.print(word[selection]);
			tft.setTextColor(ILI9341_BLACK);
			tft.fillRect(diffArr[selection + 1] - 4, DIFFLOCATIONY - 4,lengthOfWord[selection + 1], 21 ,ILI9341_WHITE);
			tft.setCursor(diffArr[selection + 1], DIFFLOCATIONY);
			tft.print(word[selection + 1]);
		}
		else if(joyVal < JOY_CENTER - JOY_DEADZONE && selection != 3){
			selection++;
			tft.setTextColor(ILI9341_WHITE);
			tft.fillRect(diffArr[selection] - 4, DIFFLOCATIONY - 4,lengthOfWord[selection], 21 ,ILI9341_BLACK);
			tft.setCursor(diffArr[selection], DIFFLOCATIONY);
			tft.print(word[selection]);
			tft.setTextColor(ILI9341_BLACK);
			tft.fillRect(diffArr[selection - 1] - 4, DIFFLOCATIONY - 4,lengthOfWord[selection - 1], 21 ,ILI9341_WHITE);
			tft.setCursor(diffArr[selection -1 ], DIFFLOCATIONY);
			tft.print(word[selection - 1]);
		}

		if(buttonVal == false){
			Serial.println(selection);
			if( selection == 0){
				difficulty = 10;//corresponds to easy
			}
			else if(selection == 1){
				difficulty = 15;//corresponds to medium
			}
			else if(selection == 2){
				difficulty = 20;//corresponds to hard
			}
			else if(selection == 3){
				tft.setCursor(CUSTOMLOCATIONX - 75, DIFFLOCATIONY + 30);
				tft.print("MINE#: ");
				difficulty = custom();//get a custom set of mines using the POTENTIOMETER
			}
			break;//leave the function once a selection has been made

		}
	}
}

int custom(){
	delay(100);
	int button = digitalRead(JOY_SEL);
	int potenVal;
	int customMines;


	while(button != false){
		button = digitalRead(JOY_SEL);
		potenVal = analogRead(POTENTIOMETER);//reading the restistance value
		//from the POTENTIOMETER and then maps it to a set value
		//these values are the number of mines selected
		customMines = map(potenVal,1, 1005, 5, 76);//max mines are 75, min is 5
		tft.setTextColor(ILI9341_RED);
		tft.setCursor(CUSTOMLOCATIONX, DIFFLOCATIONY + 30);
		tft.print(customMines);//displays the number of mines
		delay(100);
		tft.setTextColor(ILI9341_WHITE);
		tft.setCursor(CUSTOMLOCATIONX, DIFFLOCATIONY + 30);
		tft.print(customMines);
	}

	return customMines;


}





void displayGrid(){

	//delay(1000);

	tft.fillScreen(ILI9341_WHITE);

	tft.drawRect(6,8,DISPLAY_WIDTH - 13,221,ILI9341_BLACK);
	//these for loops draw the grid by first drawing lines across the screen in
	// the x direction then in the y. it is set so that each 'box' is 9x9 pixels

	tft.fillRect(DISPLAY_WIDTH - 16, 9, 10, 220, ILI9341_BLACK);


	int numBoxesX = 16;
	int numBoxesY = 12;
	int drawValX = 6;
	int drawValY = 8;
	int boxSize = 20;

	//draws a grid using two for loops, one goes across the screen in the x
	//direction and another in the y direction
	for(int i = 1; i <= numBoxesY ; i++){
		drawValY = drawValY + boxSize;
		tft.drawLine(6,drawValY,DISPLAY_WIDTH - 15,drawValY, ILI9341_BLACK);
		tft.drawLine(6,drawValY+1,DISPLAY_WIDTH - 15,drawValY+1, ILI9341_BLACK);
	}

	for(int j = 1; j <= numBoxesX; j++){
		drawValX = drawValX + boxSize;
		tft.drawLine(drawValX,8,drawValX,DISPLAY_HEIGHT - 12, ILI9341_BLACK);
		tft.drawLine(drawValX+1,8,drawValX+1,DISPLAY_HEIGHT - 12, ILI9341_BLACK);

	}
	tft.setTextSize(1);
	tft.setCursor(DISPLAY_WIDTH - 76, 0);
	tft.setTextColor(ILI9341_BLACK);
	tft.print("Version:");
	tft.print(version);

}

//takes both value of new cursor postion and the old one
void redrawCursor(int x, int y, int oldx, int oldy){
	tft.drawRect(x+1,y,20,20,ILI9341_BLUE);
	tft.drawRect(x+2,y+1,18,18,ILI9341_BLUE);
	//redrawing over old position
	tft.drawRect(oldx+1,oldy,20,20,ILI9341_BLACK);
	tft.drawRect(oldx+2,oldy+1,18,18,ILI9341_WHITE);

}

void drawFlag(int x, int y) {
	tft.fillRect(x+5, y+15, 10, 3, ILI9341_BLACK);
	tft.fillRect(x+7, y+13, 6, 2, ILI9341_BLACK);
	tft.drawLine(x+10, y+1, x+10, y+13, ILI9341_BLACK);
	tft.fillRect(x+10, y+1, 8, 6, ILI9341_RED);
	blocks[yToI(y)+1][xToJ(x)+1][1] = 1;

}



/* The following four functions are used to convert between the indicies of
the blocks[][] (which contains a virtual game board) to coordinates on screen
*/
int xToJ(int x) {
	int j;
	j = (x-6)/20;
	return j;
}
int yToI(int y) {
	int i;
	i = (y-9)/20;
	return i;
}
int jToX(int j) {
	int x;
	x = (j*20)+6;
	return x;
}
int iToY(int i) {
	int y;
	y = (i*20)+9;
	return y;
}




int cleared[13][17] = {0};

void clearBlock(int i, int j) {
	tft.fillRect(jToX(j-1),iToY(i-1),20,20,ILI9341_BLACK);
	tft.drawRect(jToX(j-1)+2,iToY(i-1)+1,18,18,ILI9341_WHITE);
	//tft.drawRect(oldx+2,oldy+1,18,18,ILI9341_WHITE);
}
// showNum() is used to display the number of surrounding mines for a square
void showNum(int i, int j) {
	tft.setTextSize(2);
	tft.setTextColor(ILI9341_RED);
	tft.setCursor(jToX(j-1)+5, iToY(i-1)+4);
	tft.print(blocks[i][j][0]);
}
void clearSpaces(int i, int j) {
	if(blocks[i][j][0] == 0) {
		clearSurrounding(i+1, j);
		clearSurrounding(i-1, j);
		clearSurrounding(i, j+1);
		clearSurrounding(i, j-1);
		clearSurrounding(i+1, j-1);
		clearSurrounding(i-1, j+1);
		clearSurrounding(i+1, j+1);
		clearSurrounding(i-1,j+1);
	}
	else {
		cleared[i][j] = 1;
		showNum(i,j);
	}
}

void clearSurrounding(int i, int j) {
if(i == 0 || j == 0 || i == 12 || j == 16 || cleared[i][j] == 1
	|| blocks[i][j][1] == 1 || blocks[i][j][0] == 10) {
		return;
	}
	if(blocks[i][j][0] == 0) {
		cleared[i][j] = 1;
		clearBlock(i, j);
		clearSpaces(i, j);
	}
	else if (blocks[i][j][0] != 10){
		cleared[i][j] = 1;
		showNum(i, j);
	}
}

void minesweeper(){
	displayTitle();
	diffSelect();
	displayGrid();

	int genNum;
	genNum = analogRead(13);//read from the analog pin
	placemines(genNum);// the random generator that places a set number of mines on the
	//field

	assignNums();//assigns numbers to all blocks *doesnt work*

	int startingXVal = 146;//pixel for starting cursor position
	int startingYVal = 109;

	int blockSelecX = startingXVal;
	int blockSelecY = startingYVal;

	int anaXVal;
	int anaYVal;

	bool buttonActive = true;


	tft.drawRect(startingXVal+1,startingYVal,20,20,ILI9341_BLUE);
	tft.drawRect(startingXVal+2,startingYVal+1,18,18,ILI9341_BLUE);
	//read value of button on joystick

	while(buttonActive == true){
		if(!digitalRead(FLAG_BUTTON)){
			if(blocks[yToI(blockSelecY)+1][xToJ(blockSelecX)+1][1] == 0) {
				drawFlag(blockSelecX, blockSelecY);
				delay(100);

			}
			else {
				tft.fillRect(blockSelecX+2,blockSelecY+1,19,19, ILI9341_WHITE);
				blocks[yToI(blockSelecY)+1][xToJ(blockSelecX)+1][1] = 0;
				if(blocks[yToI(blockSelecY)+1][xToJ(blockSelecX)+1][0] > 0 &&
					blocks[yToI(blockSelecY)+1][xToJ(blockSelecX)+1][0] < 9 &&
					cleared[yToI(blockSelecY)+1][xToJ(blockSelecX)+1]) {

						showNum(yToI(blockSelecY)+1, xToJ(blockSelecX)+1);
					}
				else if(cleared[yToI(blockSelecY)+1][xToJ(blockSelecX)+1]) {
					clearBlock(yToI(blockSelecY)+1, xToJ(blockSelecX)+1);
				}
			}
			delay(10);
		}

		loop :

		/*1. read joystick input
		2.if movement threshold reached, move cursor in that direction
		3. while loop quits if touchIsActive == false */
		delay(100);
		anaXVal = analogRead(JOY_HORIZ);
		anaYVal = analogRead(JOY_VERT);
		buttonActive = digitalRead(2);
		//following cases move the cursor depending on position of the joystick

		if (anaYVal < JOY_CENTER - JOY_DEADZONE && blockSelecY != 9) {
			blockSelecY = blockSelecY - 20;//moving cursor up
			Serial.println(blockSelecY);    //
			// }
			redrawCursor(blockSelecX, blockSelecY,blockSelecX,blockSelecY + 20);
		}
		else if (anaYVal > JOY_CENTER + JOY_DEADZONE && blockSelecY != 209) {
			blockSelecY = blockSelecY + 20;
			Serial.println(blockSelecY);
			redrawCursor(blockSelecX, blockSelecY,blockSelecX,blockSelecY - 20);
		}

		// remember the x-reading increases as we push left
		if (anaXVal > JOY_CENTER + JOY_DEADZONE && blockSelecX != 6) {
			blockSelecX = blockSelecX - 20;
			Serial.println(blockSelecX);
			redrawCursor(blockSelecX, blockSelecY,blockSelecX + 20,blockSelecY);

		}
		else if (anaXVal < JOY_CENTER - JOY_DEADZONE && blockSelecX != 286) {
			blockSelecX = blockSelecX + 20;
			Serial.println(blockSelecX);
			redrawCursor(blockSelecX, blockSelecY,blockSelecX - 20,blockSelecY);

		}


	}

	//will be used to test if a mine is at a location
	bool mine;

	mine = isMine(blockSelecX, blockSelecY);

	/*when button clicked & not empty box
	1. see if mine is under cursor, if so end game
	2. else, clear all ajacent empty tiles
	3. place numbers in boxes with adjacent mines
	*/

	Serial.print("mine: ");
	Serial.print(mine);
	if(mine == false){
		clearSpaces(yToI(blockSelecY)+1, xToJ(blockSelecX)+1);
		if(checkWin()){
			for(int i = 0; i < 13; i++) {
				for(int j = 0; j < 17; j++) {
					blocks[i][j][0] = 0;
					blocks[i][j][1] = 0;
					cleared[i][j] = 0;
				}
			}
			delay(1000);
			displayWin();
		}
		else{
			goto loop;
			//restarts the while 'movement' loop
		}
	}
	else if(mine == true){
		//flash red circle indicating mine has been hit
		for(int i = 0; i < 5; i++){
			delay(100);
			tft.fillCircle(blockSelecX + 10, blockSelecY + 10, 6, ILI9341_RED);
			delay(100);
			tft.fillCircle(blockSelecX + 10, blockSelecY + 10, 6, ILI9341_WHITE);
		}

		for(int i = 0; i < 13; i++) {
			for(int j = 0; j < 17; j++) {
				blocks[i][j][0] = 0;
				blocks[i][j][1] = 0;
				cleared[i][j] = 0;
			}
		}
		delay(1000);
		displayLose();
	}
}





//passes the value genNum which is taken from the analog port of the Arduino
void placemines(int genNum){

	int randIndexX;//set to random value
	int randIndexY;
	randomSeed(genNum);//sets the seed of the random function so that it outputs
	//different numbers when it is called

	//following is an array of all possible mine starting values for X/Y
	int MINEX[15] = {6,26,46,66,86,106,126,146,166,186,206,226,246,266,286};
	int MINEY[11] = {9,29,49,69,89,109,129,149,169,189,209};


	//following structure will contains the mines location


	for(int i = 0; i < difficulty; i++){
		randIndexX = random(0,15);
		MINE[i].xLoc = MINEX[randIndexX];
		randIndexY = random(0,11);
		MINE[i].yLoc = MINEY[randIndexY];
		blocks[randIndexY+1][randIndexX+1][0] = 10;

	}

}

number BOXNUMS[165];//structure that contains the locations and possible value
//of all boxes

void assignNums(){
	for(int i = 1; i < 13; i++) {
		for(int j = 1; j < 17; j++) {
			if (blocks[i][j][0] != 10) {
				if(blocks[i-1][j][0] == 10) {
					blocks[i][j][0]++;
				}
				if(blocks[i+1][j][0] == 10) {
					blocks[i][j][0]++;
				}
				if(blocks[i][j-1][0] == 10) {
					blocks[i][j][0]++;
				}
				if(blocks[i][j+1][0] == 10) {
					blocks[i][j][0]++;
				}
				if(blocks[i-1][j-1][0] == 10) {
					blocks[i][j][0]++;
				}
				if(blocks[i+1][j+1][0] == 10) {
					blocks[i][j][0]++;
				}
				if(blocks[i-1][j+1][0] == 10) {
					blocks[i][j][0]++;
				}
				if(blocks[i+1][j-1][0] == 10) {
					blocks[i][j][0]++;
				}
			}

		}
	}


}


bool isMine(int xFromCursor, int yFromCursor){
	for(int i = 0; i < difficulty; i++ ){
		if((xFromCursor == MINE[i].xLoc) && (yFromCursor == MINE[i].yLoc))

		return true;
	}
	return false;

}

bool checkWin() {

	int clearedBlocks = 0;
	for (int i = 0; i < 13; i++) {
		for(int j = 0; j < 17; j++) {
			if(cleared[i][j] == 1) {
				clearedBlocks++;
			}
		}
	}

	if(clearedBlocks == 165-difficulty) {
		return true;
	}
	else{

		return false;
	}
}
void displayWin() {
	tft.fillScreen(ILI9341_BLACK);
	tft.setCursor(DISPLAY_WIDTH/4,10);
	tft.setTextSize(3);
	tft.setTextColor(ILI9341_GREEN);

	for(int i = 0; i < 3; i++){
		tft.print("You Win!");
		delay(700);
		tft.setTextColor(ILI9341_BLACK);
		tft.setCursor(DISPLAY_WIDTH/4,10);
		tft.print("You Win!");
		delay(700);
		tft.setTextColor(ILI9341_GREEN);
		tft.setCursor(DISPLAY_WIDTH/4,10);
	}
	tft.setTextColor(ILI9341_GREEN);
	tft.setCursor(DISPLAY_WIDTH/4,10);
	tft.print("You Win!");
	delay(500);

	tft.setTextSize(2);
	tft.setCursor(DISPLAY_WIDTH/4 - 20, 50);
	tft.print("Restarting... ");

	for(int i = 3; i > 0; i-- ){
		tft.setCursor(DISPLAY_WIDTH/2 + 60, 50);
		tft.setTextColor(ILI9341_GREEN);
		tft.print(i);
		delay(1000);
		tft.setCursor(DISPLAY_WIDTH/2 + 60, 50);
		tft.setTextColor(ILI9341_BLACK);
		tft.print(i);
	}
	delay(1000);
}

void displayLose(){
	/*1. clear playing field
	2. display stored users score
	3. after certian time period, restart the game
	*/
	tft.fillScreen(ILI9341_BLACK);
	tft.setCursor(DISPLAY_WIDTH/4 - 5,10);
	tft.setTextSize(3);
	tft.setTextColor(ILI9341_RED);

	for(int i = 0; i < 3; i++){
		tft.print("Game Over!");
		delay(400);
		tft.setTextColor(ILI9341_BLACK);
		tft.setCursor(DISPLAY_WIDTH/4 - 5,10);
		tft.print("Game Over!");
		delay(400);
		tft.setTextColor(ILI9341_RED);
		tft.setCursor(DISPLAY_WIDTH/4 - 5,10);
	}
	tft.setTextColor(ILI9341_RED);
	tft.setCursor(DISPLAY_WIDTH/4 - 5,10);
	tft.print("Game Over!");
	delay(300);

	tft.setTextSize(2);
	tft.setCursor(DISPLAY_WIDTH/4 - 20, 50);
	tft.print("Restarting... ");

	for(int i = 3; i > 0; i-- ){
		tft.setCursor(DISPLAY_WIDTH/2 + 60, 50);
		tft.setTextColor(ILI9341_RED);
		tft.print(i);
		delay(800);
		tft.setCursor(DISPLAY_WIDTH/2 + 60, 50);
		tft.setTextColor(ILI9341_BLACK);
		tft.print(i);
	}

	delay(1000);

}
