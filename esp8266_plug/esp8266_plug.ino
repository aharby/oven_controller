
//Complete project details at https://RandomNerdTutorials.com/esp-now-two-way-communication-esp8266-nodemcu/


#include <ESP8266WiFi.h>
#include <espnow.h>


#define relayPin 4

uint8_t broadcastAddress[] = {0xA0,0x20, 0xA6, 0x16, 0xAD, 0x40};



// Define variables to store incoming readings
int incomingControlRelayState;

int currentRelayState;

// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure
typedef struct structInMessage {
    int controlRelayState;
} structInMessage;

typedef struct structOutMessage {
    int currentRelayState;
} structOutMessage;

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
  incomingControlRelayState = inMsg.controlRelayState;
}


 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  pinMode(relayPin, OUTPUT);

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


  if (incomingControlRelayState){
    digitalWrite (relayPin, HIGH);
    currentRelayState = 1;
  }
  else{
    digitalWrite (relayPin, LOW);
    currentRelayState =0;
  }
  
    outMsg.currentRelayState= currentRelayState;
    esp_now_send(broadcastAddress, (uint8_t *) &outMsg, sizeof(outMsg));

    // Print incoming readings
    Serial.println(incomingControlRelayState);
    delay(200);
  }
