//CMPUT 274 FINAL PROJECT MAIN FUNCTION

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_ILI9341.h>
#include "header.h"
#include <TouchScreen.h>
#include <string.h>


int main(){

	setup();

	while(true){
		minesweeper();
	}


	Serial.end();
	return 0;
}
