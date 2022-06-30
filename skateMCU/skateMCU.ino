#include <SPI.h>
#include <WiFi.h>
#include <MFRC522.h>
#include <WiFiUdp.h>

#define RST_PIN 22
#define SS_PIN 5
#define Buz1 12
#define UID_1 109
#define UID_2 113
#define UID_3 117
#define UID_4 121

int status = WL_IDLE_STATUS;
const char * networkName = "IOT";
const char * networkPswd = "12345678";
const char * udpAddress = "192.168.137.1";
const int udpPort = 10010;

//Are we currently connected?
boolean connected = false;

void connectToWiFi(const char * ssid, const char * pwd){
  Serial.println("Connecting to WiFi network: " + String(ssid));

  // delete old config
  WiFi.disconnect(true);
  //register event handler
  WiFi.onEvent(WiFiEvent);
  
  //Initiate connection
  WiFi.begin(ssid, pwd);

  Serial.println("Waiting for WIFI connection...");
}

WiFiUDP Udp;
MFRC522 mfrc522(SS_PIN, RST_PIN);
void setup() {
  Serial.begin(115200);
  SPI.begin();
  /* Initialize MFRC-522 and send version info to serial port (can be seen in serial monitor of arduino IDE)
   */
  pinMode(Buz1, OUTPUT);
  connectToWiFi(networkName, networkPswd);
  Serial.println("Connected to wifi");
  printWifiStatus();
  mfrc522.PCD_Init();
  mfrc522.PCD_DumpVersionToSerial();
  Serial.println(F("Scan UID"));
}

int uid_detect;
void loop() {
  uid_detect = 0;
  /*  PICC_IsNewCardPresent() is to detect card and 
   *  if there is no card, restart the loop()
   */
  if(!mfrc522.PICC_IsNewCardPresent()){
    return;  
  }

  if(!mfrc522.PICC_ReadCardSerial()){
    return;
  }
  /*Show information of card ID */
  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));  
  /* sum up the ID value */
  for(int i = 0; i < 8; i++){
    uid_detect += mfrc522.uid.uidByte[i];
  } 
  /* see the detected ID is what we want, now I just set every ID 
   * will be detected so the buzzer will be trigger for every ID tag
   */
  switch(uid_detect){
    case UID_1:
    case UID_2:
    case UID_3:
    case UID_4:
      {
        Serial.println("detected");
        digitalWrite(Buz1, HIGH);
        delay(100);
        digitalWrite(Buz1, LOW);
      }
      break;
  }
  Serial.println(uid_detect);

  // send data if connected
  if(connected){
    //Send a packet
    Udp.beginPacket(udpAddress,udpPort);
    Udp.printf("mills since boot: %lu, Uid Detected: %d", millis(), uid_detect);
    Udp.endPacket();
  }
  //Wait for 1 second
  delay(1000);

}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

//wifi event handler
void WiFiEvent(WiFiEvent_t event){
    switch(event) {
      case SYSTEM_EVENT_STA_GOT_IP:
          //When connected set 
          Serial.print("WiFi connected! IP address: ");
          Serial.println(WiFi.localIP());  
          //initializes the UDP state
          //This initializes the transfer buffer
          Udp.begin(WiFi.localIP(),udpPort);
          connected = true;
          break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
          Serial.println("WiFi lost connection");
          connected = false;
          break;
      default: break;
    }
}
