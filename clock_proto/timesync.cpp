#include <ESP8266WiFi.h>
#include "timesync.h"
#include "cred.h"

static unsigned long lastSync;
static bool isSynced;
static unsigned long resyncDelay;

WiFiClient TCP_client;

void TimeSyncInit()
{
  Serial.println("Start wifi");

  WiFi.setHostname("clock_proto");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("Connected to wifi network");

  lastSync = 48 * 3600 * 1000; // 2 days in milli seconds
  isSynced = false;
  resyncDelay = 30 * 1000;

}

bool TimeSync(unsigned long &spm, bool &synced)
{
  char spmBuf[16];

  if (millis() - lastSync < resyncDelay) {
    synced = isSynced;
    return false;
  }

  lastSync = millis();

  spmBuf[0] = '\0';

  Serial.println("Connect to time server");
  if (!TCP_client.connect("time.harry.thuis", 1962)) {
    Serial.println("Failed");
    isSynced = false;
    synced = isSynced;
    return false;
  }

  Serial.println("Connected");
  TCP_client.write("SPM");
  TCP_client.flush();

  // wait for response
  unsigned long start = millis();
  while (TCP_client.available() == 0) {
    if (millis() - start > 500) {
      TCP_client.stop();
      return false;    
    }
  } 
  // Read data from server and print them to Serial
  int i, count;
  if ((count = TCP_client.available()) > 0) {
    for (i = 0; i < count && i < sizeof(spmBuf) -1; i++) {
      char c = TCP_client.read();
      spmBuf[i] = c;
    }
    spmBuf[i] = '\0';
  }
  TCP_client.stop();    

  Serial.print("Received ");
  Serial.println(spmBuf);

  int s;
  if (sscanf(spmBuf, "[%d]", &s) != 1) {
    return false;
  }

  isSynced = true;
  synced = isSynced;
  //resyncDelay = 3600 * 1000; // Every hour
  resyncDelay = 60 * 1000; // Every minute

  spm = (unsigned long)s;
  return true;
}