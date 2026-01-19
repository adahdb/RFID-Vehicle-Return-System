#include <WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <ArduinoJson.h>
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>


// Define the pins for SPI communication.
#define PN532_SCK  (18)
#define PN532_MOSI (23)
#define PN532_SS   (5)
#define PN532_MISO (19)

// Telegram credentials
#define BOTtoken "input cred"
#define CHAT_ID "-input id"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Store multiple UIDs in an array of arrays.
// names of array are licence plates of the vans
uint8_t kcc71h[] = { 0x03, 0x0A, 0xDA, 0x34 };
uint8_t rdo497[] = { 0xB3, 0xCB, 0xCA, 0x0C };

Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

// WiFi credentials
const char* ssid = "input ssid";
const char* password = "0123456789";

// Server details 
const char* server_ip = "input ip";
const uint16_t server_port = 5555;

// Event handler
void WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("⚠️ WiFi disconnected!");
      WiFi.reconnect();
      break;

    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      Serial.println("✅ WiFi connected to AP.");
      break;

    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.print("📶 Got IP: ");
      Serial.println(WiFi.localIP());
      break;

    default:
      break;
  }
}

// Function to compare two UIDs
bool compareUID(uint8_t* uid1, uint8_t* uid2, uint8_t length) {
  for (uint8_t i = 0; i < length; i++) {
    if (uid1[i] != uid2[i]) {
      return false;  // UIDs do not match
    }
  }
  return true;  // UIDs match
}

void setup() {
  Serial.begin(115200);

  // Register event handler (must use 2-argument version now)
  WiFi.onEvent(WiFiEvent);

  // Start connecting to WiFi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

  Serial.println("Waiting for an ISO14443A Card ...");
  client.setInsecure();  // ⚠️ Insecure, use root cert in production
  delay(2000);  // Wait for connection
  bot.sendMessage(CHAT_ID, "🚀 Bot started up", "");
}

void loop() {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");

    // Compare the UID with multiple predefined UIDs
    if (uidLength == 4) {
      // Mifare Classic card (4-byte UID)
      if (compareUID(uid, kcc71h, uidLength)) {
        Serial.println("Mercedes Van Has Been Returned - Reg Nr: KCC71H");
        bot.sendMessage(CHAT_ID, "Mercedes Van Has Been Returned - Reg Nr: KCC71H", "");
      }
      else if (compareUID(uid, rdo497, uidLength)) {
        Serial.println("Van Has Been Returned - Reg Nr: RDO497");
          bot.sendMessage(CHAT_ID, "Ford Transit Has Been Returned - Reg Nr: RDO497", "");
      }
      else {
        Serial.println("UIDs do not match any predefined cards.");
      }
    }

    if (uidLength == 7) {
      // Mifare Ultralight card (7-byte UID)
      Serial.println("Seems to be a Mifare Ultralight tag (7 byte UID)");
      // Add logic if needed for other 7-byte UIDs
    }
    delay(2000);
  }
}
