#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include <ArduinoOTA.h>

#include <EEPROM.h>
#include <Wire.h>
#include <PolledTimeout.h>
#include <TypeConversion.h>
#include <Crypto.h>

#include "SSD1306Wire.h"
#include "images.h"
SSD1306Wire display(0x3c, SDA, SCL);


//Memory
#define EEPROM_length 4096
#define EEPROM_offset 1
#define usable_address_bits 7
int max_value_address = 0;

String memory_map[50];
int memory_type[50] = {-1};
enum types{
  text,
  command,
  password
};
int settings[50];

int checkpoint_memory[20];

int command_length[50];
//short int changes[50];
int sector_loaded = 0;

int i, a, r, c, q;


//Commands
enum credential_commands {
  addrExtSSID,
  addrExtPassword
};

//I2C
#define SDA_PIN 4
#define SCL_PIN 5
const int16_t I2C_MASTER = 0x42;
const int16_t I2C_SLAVE = 0x08;

//Serial comunication
bool stringComplete = false;
String inputString = "";
String serialString[10];
unsigned long int execution_time;
int serial_reporter;


//Encryption
namespace TypeCast = experimental::TypeConversion;
using namespace experimental::crypto;
uint8_t resultArray[SHA256::NATURAL_LENGTH] { 0 };
uint8_t derivedKey[ENCRYPTION_KEY_LENGTH] { 0 };
char masterKey[] = "w86vn@rp"; //esempio
uint8_t resultingNonce[12] { 0 };
uint8_t resultingTag[16] { 0 };
static uint32_t encryptionCounter = 5;
uint8_t hkdfSalt[16] { 0 };


//Buttons
#define up D5
#define confirm D6
#define down D7

//Network
bool netStat = 0; //status of the network
bool netMode = 0;
bool ota_initialised = false;
//SSID and PSWD of Cosimo's Network
const char* personal_ssid = "Easyino Cosimo";
const char* personal_password = "12345678";

//Variables declaration
struct section {
  String title;
  String email;
  String password;
};
int n_section = 0;
struct section section[10];
//SSID and PSWD of external wifi network
String ext_ssid = "federixo-Inspiron-5567";
String ext_password = "pasword";
IPAddress local_ip(10, 10, 10, 1);
IPAddress gateway(10, 10, 10, 1);
IPAddress subnet(255, 0, 0, 0);
ESP8266WebServer server(80);

#define addrNetMode 0 //0 for creating a webpage; 1 for connecting to an existing one
