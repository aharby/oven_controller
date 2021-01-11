
#include "display_handler.h"




LiquidCrystal_I2C lcd(0x27,16,2);

I2CKeyPad keyPad;

char keys[] = "123A456B789C*0#DNF";  // N = Nokey, F = Fail



DisplayHandler::DisplayHandler(void){


control = 0;

mode = 0;


timerState = false;

//timer alarm going off or not
timerAlarmState = false;

//hours/minutes/seconds of set timer (start at 0)
timerHours = 0;
timerMins = 0;
timerSecs = 0;

//for clearing out lcd
blank = "               ";
//horizontal position of the led cursor in temperature mode
cursorPositionTempX = 9;
//vertical position of led cursor in temperature mode
cursorPositionTempY = 0;
//horizontal position of the led cursor in timer mode
cursorPositionX = 0;
//vertical position of led cursor in timer mode
cursorPositionY = 1;
//horizontal position of cursor at 0
cursorDefaultX = 0;
//vertical position of cursor at 0
cursorDefaultY = 0;
//vertical position of cursor at 1
cursorSecondaryY = 1;

}//constructor




void DisplayHandler::init() {
   
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();

  if (keyPad.begin(KEYPADI2CADDRESS) == false)
  {
    Serial.println("\nERROR: cannot communicate to keypad.\nPlease reboot.\n");
    while(1);
  }
}




//switch between control and display mode
void DisplayHandler::changeControl() {

	if(control) {
		control = false;
		lcd.noCursor();
	} else {
		control = true;
		if(mode == TEMP_MODE) {
			lcd.cursor();
		}
		if(mode == TIMER_MODE) {
			cursorPositionX = 0;
			cursorPositionY = 1;
			lcd.cursor();
		}
	}

}

//switch between timer and temperature mode
void DisplayHandler::changeMode() {

	if(mode == TEMP_MODE) {
		mode = TIMER_MODE;
		if(control) {
			cursorPositionX = 0;
			cursorPositionY = 1;
		}
	} else {
		mode = TEMP_MODE;
	}

}

// ticker function
void DisplayHandler::timerCount() {

	//tick down timer if timer is on and time isnt at 00:00:00
	if (timerState && !(timerSecs == 0 && timerMins == 0 && timerHours == 0)) {
		timerSecs--;
		numOverflow();
		printTimer();

	}

	//if time is at 00:00:00 turn timerAlarmState on
	if (timerState && timerSecs == 0 && timerMins == 0 && timerHours == 0) {
		timerAlarmState = true;
    //timerState= false;
	} else {
		timerAlarmState = false;
	}
}

//format timer output
void DisplayHandler::printTimer() {

	sprintf(timer, "%02d:%02d:%02d", timerHours, timerMins, timerSecs);

}

//write data to lcd
void DisplayHandler::writeToLCD(String firstRow, String secondRow) {

	lcd.setCursor(cursorDefaultX, cursorDefaultY);
	lcd.print(firstRow);
	lcd.setCursor(cursorDefaultX, cursorSecondaryY);
	lcd.print(secondRow);
	if(mode == TIMER_MODE) {
		lcd.setCursor(cursorPositionX, cursorPositionY);
	} else {
		lcd.setCursor(cursorPositionTempX, cursorPositionTempY);
	}

}

//clear out lcd text
void DisplayHandler::clearLCD() {
	lcd.setCursor(cursorDefaultX, cursorDefaultY);
	lcd.print(blank);
	lcd.setCursor(cursorDefaultX, cursorSecondaryY);
	lcd.print(blank);
}

//convert minutes, seconds and hours into proper time format
void DisplayHandler::numOverflow() {

	//if seconds get to 60, change to 0 and increment minutes
	if (timerSecs > 59) {
		timerSecs = 0;
		timerMins++;
	}
	//if seconds get below 0, change to 59 and decrement minutes
	if (timerSecs < 0) {
		timerSecs = 59;
		timerMins--;
	}
	//if minutes get to 60, change to 0 and increment hours
	if (timerMins > 59) {
		timerMins = 0;
		timerHours++;
	}
	//if minutes get below 0, change to 59 and decrement hours
	if (timerMins < 0) {
		timerMins = 59;
		timerHours--;
	}
	//limit upper hour boundary at 99
	if (timerHours > 99) {
		timerHours = 99;
	}
	//limit lower hour boundary at 0
	if (timerHours < 0) {
		timerHours = 0;
	}

}

//refresh the led screen output
void DisplayHandler::refreshScreen() {
  String s1;
  
	switch (mode) {

	case TEMP_MODE: {
   
		if(control) {
			writeToLCD("Tmp lim: " + String(controlTemperature) + " ",
					"");
			break;
		} else {
			writeToLCD("Tmp: " + String(currentTemperature), "S1=" +String(manualSwitch)+ ", " +"Relay: " + String(relayState));
			break;
		}

	}

	case TIMER_MODE: {

		sprintf(timer, "%02d:%02d:%02d", timerHours, timerMins, timerSecs);
		if(control) {
			writeToLCD("Set timer:", timer);
			break;
		} else {
			writeToLCD("Timer: " + String(timerState), timer);
			break;
		}

	}

	}

}



//set the digits of timer alarm/temperature limit using keypad
void DisplayHandler::setDigit(char num) {

	//check for timer/temperature mode
	if(mode == TIMER_MODE) {
		    //declare switch case for cursor position identification
			switch (cursorPositionX) {

			//if in position 0 set tens digit of hours to pressed number
			//by transforming char value into correct int digit
			case 0: {

				int tempHours = (((int) num + 2) % 50) * 10 + timerHours % 10;
				timerHours = tempHours;
				break;

			}

			//if in position 1 set ones digit of hours to pressed number
			//by transforming char value into correct int digit
			case 1: {

				int tempHours = (timerHours / 10) * 10 + ((int) num + 2) % 50;
				timerHours = tempHours;
				break;

			}

			//if in position 3 set tens digit of minutes to pressed number
			//by transforming char value into correct int digit
			case 3: {
				//limit the possible input by up to 5 to avoid overflow
				if (num >= '0' && num <= '5') {
					int tempMins = (((int) num + 2) % 50) * 10 + timerMins % 10;
					timerMins = tempMins;
				}
				break;

			}

			//if in position 4 set ones digit of minutes to pressed number
			//by transforming char value into correct int digit
			case 4: {

				int tempMins = (timerMins / 10) * 10 + ((int) num + 2) % 50;
				timerMins = tempMins;
				break;

			}

			//if in position 6 set tens digit of seconds to pressed number
			//by transforming char value into correct int digit
			case 6: {
				//limit the possible input by up to 5 to avoid overflow
				if (num >= '0' && num <= '5') {
					int tempSecs = (((int) num + 2) % 50) * 10 + timerSecs % 10;
					timerSecs = tempSecs;
				}
				break;

			}

			//if in position 7 set ones digit of seconds to pressed number
			//by transforming char value into correct int digit
			case 7: {

				int tempSecs = (timerSecs / 10) * 10 + ((int) num + 2) % 50;
				timerSecs = tempSecs;
				break;

			}
			}
	} else {

		//multiply current control temperature by 10 and add new input
		controlTemperature = controlTemperature * 10 + (((int) num + 2) % 50);
		cursorPositionTempX++;

	}
}

//start/pause timer countdown
void DisplayHandler::startPauseTimer() {

	if (timerState) {
		//turn off the timer and set timer text to off
		timerState = false;

	} else {
		//turn on the timer and set timer text to on
		timerState = true;

		//make sure set timer state is off and disable cursor
	}

}

//function for identifying keys pressed and performing appropriate function
void DisplayHandler::checkKeys() {
	
	if(!keyPad.isPressed()) return;
  Serial.println("pPPPresssed");
  keyIdx= keyPad.getKey();
	key= keys[keyIdx];
  Serial.println(key);
  Serial.println(key);
  Serial.println(key);
  Serial.println(key);
  Serial.println(key);
	if (key != 'N') //N = No_key
	{ 
		//set A key for starting/stopping timer (timer mode only)
		if (key == PAUSE_TIMER && mode == TIMER_MODE) {
			startPauseTimer();
		}

		//set B key to changing between timer and temperature modes
		if (key == CHANGE_MODE) {
			clearLCD();
			changeMode();
			refreshScreen();
		}

		//set C key for changing between control and display modes
		if (key == CHANGE_CONTROL) {
			clearLCD();
			changeControl();
			refreshScreen();
		}

		//set D key for manually turning relay off/on
		if (key == MANUAL_OFF) {
			if(manualSwitch) {
				manualSwitch = 0;
			} else {
				manualSwitch = 1;
			}
		}

		//set * key for deleting rightmost digit of control temperature
		//while control temperature mode is on
		if (key == CURSOR_MOVE_LEFT && mode == TEMP_MODE) {
			controlTemperature /= 10;
			cursorPositionTempX--;
			refreshScreen();
		}

		//set * key for moving the cursor left while control timer mode is on
		//and cursor is not in the leftmost position
		if (key == CURSOR_MOVE_LEFT && mode == TIMER_MODE
				&& cursorPositionX > CURSOR_LEFT_BOUNDARY) {
			//make sure cursor jumps over the : separation columns, otherwise move one to the left
			if (cursorPositionX == 3 || cursorPositionX == 6) {
				cursorPositionX -= 2;
				lcd.setCursor(cursorPositionX, cursorPositionY);
			} else {
				cursorPositionX--;
				lcd.setCursor(cursorPositionX, cursorPositionY);
			}
			refreshScreen();
		}

		//set # key for moving the cursor right while control timer mode is on
		//and cursor is not in the rightmost position
		if (key == CURSOR_MOVE_RIGHT && mode == TIMER_MODE
				&& cursorPositionX < CURSOR_RIGHT_BOUNDARY) {
			//make sure cursor jumps over the : separation columns, otherwise move one to the right
			if (cursorPositionX == 1 || cursorPositionX == 4) {
				cursorPositionX += 2;
				lcd.setCursor(cursorPositionX, cursorPositionY);
			} else {
				cursorPositionX++;
				lcd.setCursor(cursorPositionX, cursorPositionY);
			}
			refreshScreen();

		}

		//set 0-9 keys to recognize and set corresponding digit to the current cursor position
		//as long as the control mode is on
		if (key >= INPUT_LOWER_BOUNDARY && key <= INPUT_UPPER_BOUNDARY
				&& control) {
			setDigit(key);
			printTimer();
			refreshScreen();
		}
	}
}
