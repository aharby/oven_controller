
#include "LiquidCrystal_I2C.h"
#include "Wire.h"
#include "I2CKeyPad.h"
#include "String.h"





#define TEMP_MODE                           0
#define TIMER_MODE                          1
#define TIMER_ALARM_PIN                     5
#define PAUSE_TIMER                        'A'
#define CHANGE_MODE                        'B'
#define CHANGE_CONTROL                     'C'
#define MANUAL_OFF                         'D'
#define SET_DOT                            '*'
#define CURSOR_MOVE_LEFT                   '*'
#define CURSOR_MOVE_RIGHT                  '#'
#define CURSOR_LEFT_BOUNDARY                0
#define CURSOR_RIGHT_BOUNDARY               7
#define INPUT_LOWER_BOUNDARY               '0'
#define INPUT_UPPER_BOUNDARY               '9'
#define KEYPADI2CADDRESS  0x20






class DisplayHandler{

public:
DisplayHandler();


char key;
uint8_t keyIdx;





bool control;

int mode;

bool timerState ;

//timer alarm going off or not
bool timerAlarmState;
//temporary array for timer values
char tempTimer[8];
//array for timer values
char timer[8];
//temporary hours/minutes/seconds of set timer
int tempTimerHours;
int tempTimerMins ;
int tempTimerSecs;
//hours/minutes/seconds of set timer (start at 0)
int timerHours;
int timerMins ;
int timerSecs;
//current temperature reading
float currentTemperature;
//temporary value for temperature limit
int tempControlTemperature;
//temperature limit set by user
int controlTemperature;
//represents the relay state

int relayState;
int manualSwitch;

//for clearing out lcd
String blank ;
//horizontal position of the led cursor in temperature mode
int cursorPositionTempX ;
//vertical position of led cursor in temperature mode
int cursorPositionTempY;
//horizontal position of the led cursor in timer mode
int cursorPositionX ;
//vertical position of led cursor in timer mode
int cursorPositionY ;
//horizontal position of cursor at 0
int cursorDefaultX ;
//vertical position of cursor at 0
int cursorDefaultY;
//vertical position of cursor at 1
int cursorSecondaryY;






void init();



//switch between control and display mode
void changeControl();

//switch between timer and temperature mode
void changeMode();

//format timer output
void printTimer() ;

void printTempTimer() ;

//write data to lcd
void writeToLCD(String firstRow, String secondRow);

//clear out lcd text
void clearLCD();

//convert minutes, seconds and hours into proper time format
void numOverflow();
//refresh the led screen output
void refreshScreen();


// ticker function
void timerCount() ;

void setDigit(char num);
void startPauseTimer();
void checkKeys();
};//end class
