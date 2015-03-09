#include <WildFire_CC3000.h>
#include <WildFire.h>
#include <ccspi.h>
#include <SPI.h>
#include <TinyWatchdog.h>
#include <plotly_streaming_wildfire.h>
WildFire wf;
TinyWatchdog tinyWDT;

#define WLAN_SSID       "wifi_network_name"
#define WLAN_PASS       "wifi_network_password"
#define WLAN_SECURITY   WLAN_SEC_WPA2

// Sign up to plotly here: https://plot.ly
// View your API key and streamtokens here: https://plot.ly/settings
#define nTraces 2
// View your tokens here: https://plot.ly/settings
// Supply as many tokens as data traces
// e.g. if you want to ploty A0 and A1 vs time, supply two tokens
char *tokens[nTraces] = {"token_1", "token_2"};
// arguments: username, api key, streaming token, filename
plotly graph = plotly("plotly_username", "plotly_api_key", tokens, "your_filename", nTraces);

unsigned long previousMillis = 0;        // will store last time LED was updated
const long wdt_pet_interval = 1000;           // interval at which to blink (milliseconds)

void wifi_connect(){
  graph.cc3000.enableTinyWatchdog(14, 1000);
  
  /* Initialise the module */
  Serial.println(F("\n... Initializing..."));
  if (!graph.cc3000.begin())
  {
    Serial.println(F("... Couldn't begin()! Check your wiring?"));
    while(1);
  }

  // Optional SSID scan
  // listSSIDResults();

  if (!graph.cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while(1);
  }

  Serial.println(F("... Connected!"));

  /* Wait for DHCP to complete */
  Serial.println(F("... Request DHCP"));
  while (!graph.cc3000.checkDHCP())
  {
    delay(100); // ToDo: Insert a DHCP timeout!
  }
}


void setup() {
  wf.begin();
  tinyWDT.begin(500, 10000);
  
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  wifi_connect();

  graph.fileopt="overwrite"; // See the "Usage" section in https://github.com/plotly/arduino-api for details
  bool success;
  success = graph.init();
  if(!success){while(true){}}
  graph.openStream();
}

unsigned long x;
int y;

void loop() {
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= wdt_pet_interval) {
    previousMillis = currentMillis;  
    tinyWDT.pet();    
  }
  
  graph.plot(millis(), analogRead(A0), tokens[0]);
  graph.plot(millis(), analogRead(A1), tokens[1]);


}
