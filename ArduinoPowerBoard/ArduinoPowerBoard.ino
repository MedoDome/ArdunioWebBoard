/*

  License: MIT

  Code is created by modification of Examples from Arduino IDE or Libraries
  which are included or installed with required module
*/



/*
  Configuration
*/
#include <Sodaq_SHT2x.h>        // Temperature and humidity (MOD_SH21)
#include <Adafruit_TSL2561_U.h> // Lux (MOD_LUX)
#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>

// Module configuration

bool DEBUG = true;          // Suppress some logs
bool MOD_ETH_CLIENT = true; // Enable Ethernet shield
bool MOD_ETH_SERVER = true; // Enable Web Server
bool MOD_ETH_SEND_DATA = true; // Enable sending data to server
bool MOD_LUX = true; // Enable Lux module
bool MOD_SH21 = true; // Enable SH21 module
bool MOD_RELAY = true; // Relay
// RelayPin
#define RELAY1 2

/* Default variables */

bool lampOff = true;                                                                // Variable to check is lamp off (If you have connected the source cable to NC set this variable to false)
bool lampOverride = false;                                                          // Overrider of relay
char *PROPERTIES[] = {"humidity", "temperature", "lux", "relay", "roride", "luxt"}; // If you turn of modules, properties will be shrinked automaticaly  {MOD_SH21, MOD_SH21, MOD_LUX, !lampOff, lampOverride}
float VALUES[] = {0.00, 0.00, 0.00, 0.00, 0.00, 30.00};
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 60 * 1000; // Loop period(ms) seconds * 1000;

/* Ethernet  settings */
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress server(192, 168, 50, 50); // numeric IP for Google (no DNS)
//char server[] = "mss.ba";          // name address for Google (using DNS)
int serverPort = 32769;

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 50, 145);
IPAddress myDns(192, 168, 50, 1);

EthernetClient client;
bool ethernetActivated = false;

/* HTTP SERVER */
EthernetServer ethServer = NULL;
int ethServerPort = 80;

/* Lux Sensor */
Adafruit_TSL2561_Unified tsl = NULL;

/**
   Print arduino's properties and it's values
*/

void printPropertiesConfig()
{
  Serial.println("== Printing properties ==");

  for (int i = 0; i < getValuesMaxSize(); i++)
  {
    Serial.print(PROPERTIES[i]);
    Serial.print(" : ");
    Serial.println(VALUES[i]);
  }
  Serial.println("== End of properties ==");
}

/**
   Print arduino's config and values
*/

void printConfig()
{

  Serial.println("==== Configuration ====");
  Serial.print("DEBUG: ");
  Serial.println(DEBUG);
  Serial.println("==== Modules ====");
  Serial.print("MOD_ETH_CLIENT: ");
  Serial.println(MOD_ETH_CLIENT);
  Serial.print("MOD_ETH_SERVER: ");
  Serial.println(MOD_ETH_SERVER);
  Serial.print("MOD_LUX: ");
  Serial.println(MOD_LUX);
  Serial.print("MOD_SH21: ");
  Serial.println(MOD_SH21);
  Serial.print("MOD_RELAY: ");
  Serial.println(MOD_RELAY);
  Serial.println("==== Variables ====");
  Serial.print("Web port: ");
  Serial.println(ethServerPort);
  Serial.print("RELAY PIN: ");
  Serial.println(RELAY1);
  Serial.print("Relay NO: ");
  Serial.println(lampOff);
  Serial.print("Relay override: ");
  Serial.println(lampOverride);
  printPropertiesConfig();
  Serial.print("Loop period(ms): ");
  Serial.println(period);
}

/**
   Activate and configure lux sensor
*/

void configureLuxSensor()
{

  if (MOD_LUX)
  {
    Serial.println("Activating TSL2561 Lux");
    tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345); // Lux senzor
    if (!tsl.begin())
    {
      Serial.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
      while (1)
        ;
    }
    tsl.enableAutoRange(true);                            /* Auto-gain ... switches automatically between 1x and 16x */
    tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS); /* fast but low resolution */
  }
}

/**
   Print arduino status to serial port

   @param labels Array of strings.(Double pointer)
   @param values Array of float values.

*/

void formatJsonPrint(char *labels[], float values[], int max)
{

  Serial.print("{\"");
  for (int i = 0; i < max; i++)
  {
    Serial.print(labels[i]);
    Serial.print("\":");
    Serial.print(values[i]);
    if (i != max - 1)
    {
      Serial.print(",\"");
    }
  }
  Serial.println('}');
}


void formatJsonSendJSON(char *labels[], float values[], int max)
{

  String data = "";
  data += "{\"";
  for (int i = 0; i < max; i++)
  {
    data += (labels[i]);
    data += "\":";
    data += (values[i]);
    if (i != max - 1)
    {
      data += ",\"";
    }
  }
  data += '}';
  sendRequest(data);
}

/**
   Send arduino status to a connected client.

   @param client Connected client.
   @param labels Array of strings.(Double pointer)
   @param values Array of float values.
   @param max Count size of array

*/

void clientPrint(EthernetClient client, char *labels[], float values[], int max)
{

  client.print("{\"");
  for (int i = 0; i < max; i++)
  {
    client.print(labels[i]);
    client.print("\":");
    client.print(values[i]);
    if (i != max - 1)
    {
      client.print(",\"");
    }
  }
  client.println('}');
}

/**
   Prepare arduino values as FormData and send it to server

   @param labels Array of strings.(Double pointer)
   @param values Array of float values.
   @param max Count size of array

*/

void formatSendForm(char *labels[], float values[], int max)
{
  String sendData = "";
  for (int i = 0; i < max; i++)
  {
    sendData += labels[i];
    sendData += "=";
    sendData += values[i];
    if (i != max - 1)
    {
      sendData += "&";
    }
  }
  if (DEBUG)
  {
    Serial.print("Sending data to server ");
    Serial.println(sendData);
  }
  sendRequest(sendData);
}

/**
   Activate ethernet module and web server module
*/

void activateEthernet()
{

  if (MOD_ETH_CLIENT)
  {
    // start the Ethernet connection:
    Serial.println("Initialize Ethernet with DHCP:");
    if (Ethernet.begin(mac) == 0)
    {
      Serial.println("Failed to configure Ethernet using DHCP");
      // Check for Ethernet hardware present
      if (Ethernet.hardwareStatus() == EthernetNoHardware)
      {
        Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
        while (true)
        {
          delay(1); // do nothing, no point running without Ethernet hardware
        }
      }
      if (Ethernet.linkStatus() == LinkOFF)
      {
        Serial.println("Ethernet cable is not connected.");
      }
      // try to congifure using IP address instead of DHCP:
      Ethernet.begin(mac, ip, myDns);
    }
    else
    {
      Serial.print("  DHCP assigned IP ");
      Serial.println(Ethernet.localIP());
    }
    // give the Ethernet shield a second to initialize:
    delay(10000);
    ethernetActivated = true;

    if (MOD_ETH_SERVER)
    {
      ethServer = EthernetServer(ethServerPort);
      Serial.println("Activating server");
      ethServer.begin();
      // give the Ethernet Web Server a second to initialize:
      delay(5000);
    }
  }
}

/**
   Send request to server

   @param data String of values in FormData naming convention

*/

void sendRequest(String data)
{

  Serial.println("=== Send request ===");
 if (client.connect("dev-httpsgw.klinkplatform.com", 80))
  {
    Serial.print("Connected to ");
    Serial.println(client.remoteIP());
    // Make a HTTP request:
    client.println("POST /api/DeviceMessages HTTP/1.1");
    client.println("Host: dev-httpsgw.klinkplatform.com");
    client.println("Content-Type: application/json");
    client.println("User-Agent: curl/7.55.1");
    client.println("OrganizationId: 2744bea7-8f6d-4fb6-b47d-89544223b5d8");
    client.println("DeviceId: ThisIsID");
    client.println("Accept: */*");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.print(data);
    client.println();
    Serial.println("=== Request sent ===");

  }
  else
  {
    // if you didn't get a connection to the server:
    Serial.println("=== Request failed. Host unreachable ===");
  }
}

/**
   Listen for Web request to Web server
*/

void handleWebRequest()
{
  if (MOD_ETH_SERVER)
  {
    // listen for incoming clients
    EthernetClient client = ethServer.available();
    if (client)
    {
      Serial.println("Client");
      // an http request ends with a blank line
      boolean currentLineIsBlank = true;
      while (client.connected())
      {
        while (client.available())
        {
          char c = client.read();
          // if you've gotten to the end of the line (received a newline
          // character) and the line is blank, the http request has ended,
          // so you can send a reply
          if (c == '\n' && currentLineIsBlank)
          {

            // Here is where the POST data is.

            char body[31];
            int index = 0;
            while (client.available())
            {

              char c = client.read();
              body[index] = c;
              if (DEBUG)
              {
                Serial.print("Char ");
                Serial.println(body[index]);
              }
              index++;
              if (index == 30)
              {
                index = 0;
              }
            }
            body[index] = '\0';

            if (index > 0)
            {
              if (DEBUG)
              {

                Serial.print("index ");
                Serial.println(index);
              }

              handleClientRequest(body);
              index = 0;
            }
            refreshParams();
            Serial.println("Sending response");
            // send a standard http response header
            client.println("HTTP/1.0 200 OK");
            client.println("Access-Control-Allow-Origin: *");
            client.println("Content-Type: application/json");
            client.println();
            clientPrint(client, PROPERTIES, VALUES, getValuesMaxSize());
            client.stop();
          }
          else if (c == '\n')
          {
            // you're starting a new line
            currentLineIsBlank = true;
          }
          else if (c != '\r')
          {
            // you've gotten a character on the current line
            currentLineIsBlank = false;
          }
        }
      }
      Serial.println("Disconnected");
    }
  }
}

void setup()
{


  Serial.begin(9600);
  printConfig();
  if (MOD_RELAY)
  {
    pinMode(RELAY1, OUTPUT);
    digitalWrite(RELAY1, HIGH); // Turns OFF Relays 1
  }
  configureLuxSensor();
  activateEthernet();
  startMillis = millis(); //initial start time

  
  cli();//stop interrupts

  //set timer4 interrupt at 1Hz
  TCCR4A = 0;// set entire TCCR1A register to 0
  TCCR4B = 0;// same for TCCR1B
  TCNT4  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR4A = 62499;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR4B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR4B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK4 |= (1 << OCIE4A);

  sei();//allow interrupts

}

/**
   Get data from modules
*/

void refreshParams()
{

  if (MOD_SH21)
  {
    setValue("humidity", SHT2x.GetHumidity());
    setValue("temperature", SHT2x.GetTemperature());
  }
  else
  {
    setValue("humidity", -2.00);
    setValue("temperature", -2.00);
  }
  if (MOD_LUX)
  {
    sensors_event_t luxEvent;
    tsl.getEvent(&luxEvent);
    setValue("lux", luxEvent.light);
    setValue("luxt", luxEvent.light);
  }
  else
  {
    setValue("lux", -2.00);
  }
  if (MOD_RELAY)
  {
    setValue("relay", !lampOff);
    setValue("roride", lampOverride);
  }
  else
  {
    setValue("relay", -2.00);
    setValue("roride", -2.00);
  }
}

/**
   Get last value of module from "database"

   @param char[] Name of propertie.

   @return float value of propertie

*/

float getValue(char v[])
{
  float value = -1.00;
  int max = getValuesMaxSize();
  if (DEBUG)
  {
    Serial.println("=== Get Value ===");
  }
  for (int i = 0; i < max; i++)
  {
    if (DEBUG)
    {
      Serial.print(v);
      Serial.print(" == ");
      Serial.println(PROPERTIES[i]);
    }
    if (strcmp(v, PROPERTIES[i]) == 0)
    {
      value = VALUES[i];
      i = max;
    }
    if (i == max && DEBUG)
    {
      Serial.println("=== End Get Value");
    }
  }

  return value;
}

/**
   Set last value of module to "database"

   @param char[] Name of propertie.
   @param float value of propertie

*/

void setValue(char v[], float value)
{
  int max = getValuesMaxSize();
  if (DEBUG)
  {
    Serial.print("=== Set Value => "); Serial.print(v); Serial.println(value);
  }
  for (int i = 0; i < max; i++)
  {
    if (strcmp(v, PROPERTIES[i]) == 0)
    {
      VALUES[i] = value;
      i = max;
    }
  }
}

/**
   Max size of array

   Function added for readability of code

   @return int max size of properties/modules.

*/

int getValuesMaxSize()
{
  int arrayBytes = 4; //Since we are using float
  return sizeof(VALUES) / arrayBytes;
}

 bool firstLoop = true;

ISR(TIMER4_COMPA_vect){

  sei();//allow interrupts

    if(DEBUG){
      Serial.println("TT");
    }
    refreshParams();
//    handleLamp(getValue("lux"));
}
 

void loop()
{
   formatJsonSendJSON(PROPERTIES, VALUES, getValuesMaxSize());


  
  //  formatJsonPrint(properties, VALUES, getValuesMaxSize());
  //  handleSerial();
  //  handleLamp(luxEvent.light);
  currentMillis = millis();                  //get the current time
  if ((currentMillis - startMillis >= period) || firstLoop) //test whether the period has elapsed
  {
    if (firstLoop) {
      firstLoop = false;
    }
    /* Get a new sensor values */
    if (ethernetActivated && MOD_ETH_SEND_DATA)
    {
      Serial.println("Sending data");
//      formatSendForm(PROPERTIES, VALUES, getValuesMaxSize());
//        formatJsonSendJSON(PROPERTIES, VALUES, getValuesMaxSize());
//
    }

    if (!ethernetActivated && DEBUG)
    {
      formatJsonPrint(PROPERTIES, VALUES, getValuesMaxSize());
    }
    startMillis = currentMillis; //IMPORTANT to save the current time of the loop
  }
  if (MOD_ETH_SERVER)
  {

    handleWebRequest(); //WEbServer
  }
}

/**
   Logic function to handle lamp by lux treshold

   @param int current lux value
*/

void handleLamp(int lux)
{
  if (MOD_RELAY && MOD_LUX)
  {
    float luxt = getValue("luxt");
    float lampon = getValue("relay"); // Since we are storing
    float lampoverride = getValue("roride");
    if (DEBUG)
    {
      Serial.println("=== HandleLamp ===");
      Serial.print("Lux: ");
      Serial.println(lux);
      Serial.print("Treshold: ");
      Serial.println(luxt);
      Serial.print("LampOff: ");
      Serial.println(!lampon);
      Serial.print("lampOverride: ");
      Serial.println(lampOverride);
      Serial.println("=== End of HandleLamp ===");
    }
    if (((lux < luxt && !lampon) || (lux >= luxt && lampon)) && !lampOverride)
    {
      if (DEBUG)
      {
        Serial.println("Toggling lamp");
      }

      toggleLamp(false);
    }
  }
}

/**
   Toggle lamp

   @param bool override is used to activate "admin mode"
*/

void toggleLamp(bool override)
{
  if (MOD_RELAY)
  {
    lampOff = !lampOff;
    setValue("relay", !lampOff);
    if (override)
    {
      toggleOverride();
    }
    digitalWrite(RELAY1, lampOff); // Turns OFF/ON Relay 1
  }
}

/**
   Function to set lux treshold for auto switching the lamp
*/

void setLuxTreshold(int lux)
{

  if (MOD_LUX)
  {
    if (DEBUG)
    {
      Serial.print("Lux treshold set :  ");
      Serial.println(lux);
    }
    if (lux > 1)
    {
      setValue("luxt", lux);
      toggleLamp(false);
    }
  }
}

/**
   Debug function for checking available commands
*/

void handleSerial()
{
  while (Serial.available() > 0)
  {
    char inputChar = Serial.read();
    handleCommand(inputChar, 1);
  }
}

/**
   Command handler

   @param char command to execute
   @param int value provided with command
*/

void handleCommand(char inputChar, int value)
{
  switch (inputChar)
  {
    case 'r':
      toggleLamp(false);
      break;
    case 'o':
      toggleOverride();
      break;
    case 'l':
      setLuxTreshold(value);
      break;
  }
}

/**
   Toggle "admin" mode of relay
*/

void toggleOverride()
{
  if (MOD_RELAY)
  {
    lampOverride = !lampOverride;
    setValue("roride", lampOverride);
  }
}

/**
   Handle client web request

   @param char[] POST body handler/parser
*/

void handleClientRequest(char body[])
{
  if (DEBUG)
  {
    Serial.print("SIZE: ");
    Serial.println(sizeof(body));
  }

  for (int i = 0; i <= sizeof(body); i += 3)
  {
    if (DEBUG)
    {
      Serial.println("=== HandleClientRequest ===");
      Serial.print("Command: ");
      Serial.println(body[i]);
      Serial.print("Value: ");
      Serial.println(body[i + 2]);
      Serial.println("=== End of HandleClientRequest ===");
    }
    handleCommand(body[i], body[i + 2]);
  }
  delay(500);
  refreshParams();
}
