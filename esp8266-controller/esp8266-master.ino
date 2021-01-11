#include <ESP8266WiFi.h>
#include <espnow.h>
#include "display_handler.h"
#include <Ticker.h>

Ticker timerTicker;

#include "max6675.h" 

uint8_t broadcastAddress[] = {0xB4,0xE6, 0x2D, 0x33, 0x26, 0x3D};

int thermoDO = 12;
int thermoCS = 13;
int thermoCLK = 14;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);




DisplayHandler displayHandler;


//current temperature reading
float currentTemperature;
//temperature limit set by user
float controlTemperature;
//represents the relay state
int controlRelayState;
int incomingRelayState;
bool timerAlarmState;
int manualSwitch;


// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure
typedef struct structOutMessage {
    int controlRelayState;
} structOutMessage;

typedef struct structInMessage {
    int currentRelayState;
} structInMessage;

structOutMessage outMsg;
structInMessage inMsg;



// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}

// Callback when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&inMsg, incomingData, sizeof(inMsg));
  Serial.print("Bytes received: ");
  Serial.println(len);
  incomingRelayState = inMsg.currentRelayState;
}



void setup() {
  Serial.begin(115200);
  
  displayHandler.init();

  timerTicker.attach(1, timerCount); 

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Set ESP-NOW Role
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {

  currentTemperature =thermocouple.readCelsius();
  displayHandler.currentTemperature = currentTemperature;
  displayHandler.relayState = incomingRelayState;
  manualSwitch= displayHandler.manualSwitch;
  timerAlarmState= displayHandler.timerAlarmState;
  controlTemperature= displayHandler.controlTemperature;
  //refresh screen unless in control mode
  if(!displayHandler.control) {
    displayHandler.refreshScreen();
  }

  //read the keyboard and run key identification routine
  displayHandler.checkKeys();
  
  if (currentTemperature< controlTemperature && !timerAlarmState && manualSwitch){
       controlRelayState= 1;
     }
    else{
       controlRelayState= 0;
    }
  
    outMsg.controlRelayState= controlRelayState;
     // Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *) &outMsg, sizeof(outMsg));

  delay(250);
}

void timerCount(){
  displayHandler.timerCount();
}
