/*
  SETTINGS:
  - Connect PC to WIFI
  - Connect ARDUINO to PC with ETHERNET cable
  - Share the WIFI connection over the ETHERNET cable
  - Configure the ETHERNET network connection using DHCP with DHCP SERVER being the ROUTER
*/

#include <SPI.h>
#include <Ethernet.h>

const int LED1 = 9; 
const int LED2 = 8; 
const int LED3 = 7; 
const int LED4 = 6; 
String readString; 

// Initialize the Ethernet server library with port 80
EthernetServer server(80);

void setup() {
 
  // Define leds as output
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  
  // Open serial communications and wait for port to open
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect. Needed for native USB port only
  }

  // Set server MAC address
  byte mac[] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
  };

  // Start the Ethernet connection using DHCP
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP!");
    // No point in carrying on, so do nothing forevermore
    for (;;);
  }

  // Print your local dynamic IP address
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");
  }
  Serial.println("");

  // Set server IP address
  IPAddress ip(Ethernet.localIP()[0], Ethernet.localIP()[1], Ethernet.localIP()[2], Ethernet.localIP()[3]);

  // Start the Ethernet connection and the server
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("Server started at: ");
  Serial.println(Ethernet.localIP()); 
}

// Turn on/off the led
void doLedOperation(const int LED, boolean operation){
  
  Serial.println("");
  Serial.print("Led");
  Serial.print(10-LED);
  Serial.print(": ");
  Serial.print(operation);
  Serial.println("");
  digitalWrite(LED, operation);
}

// Runs party mode
void party(){
  
  int i=0;

  Serial.println("Party Mode!");

  digitalWrite(LED1,LOW);
  digitalWrite(LED2,LOW);
  digitalWrite(LED1,LOW);
  digitalWrite(LED4,LOW);

  while (i<20){
    
    digitalWrite(LED1,HIGH);
    digitalWrite(LED2,LOW);
    digitalWrite(LED3,HIGH);
    digitalWrite(LED4,LOW);

    delay(100);

    digitalWrite(LED1,LOW);
    digitalWrite(LED2,HIGH);
    digitalWrite(LED3,LOW);
    digitalWrite(LED4,HIGH);

    delay(100); 

    i++; 
  }

  digitalWrite(LED1,LOW);
  digitalWrite(LED2,LOW);
  digitalWrite(LED1,LOW);
  digitalWrite(LED4,LOW);
}

void loop() {
  
  // Listen for incoming clients
  EthernetClient client = server.available();
  
  if (client) {
    Serial.println("");
    Serial.println("New client connected!");
    Serial.println("");
    while (client.connected()) {
      if (client.available()) {

        // Read char by char of the request
        char c = client.read();
        
        if (readString.length() < 100) {
          
          // Store characters to string 
          readString += c; 
        } 

        //If request has ended
        if (c == '\n'){

          // Print what the server received to serial monitor
          Serial.println(readString);
          
          // Output data 
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/plain");
          client.println("Connection: close");
          client.println();
          client.println("Command received!");

          // Give time to receive the data
          delay(1);
          
          // Close the connection
          client.stop();
          Serial.println("Client disconnected!");

          // Check if needs to turn on/off the leds
          if(readString.indexOf("LED1ON") >0){
            doLedOperation(LED1, HIGH);
          }else if(readString.indexOf("LED1OFF") >0){
            doLedOperation(LED1, LOW);
          }else if(readString.indexOf("LED2ON") >0){
            doLedOperation(LED2, HIGH);
          }else if(readString.indexOf("LED2OFF") >0){
            doLedOperation(LED2, LOW);
          }else if(readString.indexOf("LED3ON") >0){
            doLedOperation(LED3, HIGH);
          }else if(readString.indexOf("LED3OFF") >0){
            doLedOperation(LED3, LOW);
          }else if(readString.indexOf("LED4ON") >0){
            doLedOperation(LED4, HIGH);
          }else if(readString.indexOf("LED4OFF") >0){
            doLedOperation(LED4, LOW);
          }else if(readString.indexOf("PARTY") >0){
            party();
          }

          // Clearing string for next read
          readString="";
        }
      }
    }
  }
}
