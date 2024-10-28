#include <Adafruit_Fingerprint.h>
#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Keypad.h>

// Kredensial WiFi
const char* ssid = "Jeboll";
const char* password = "29082002";

// Kredensial IFTTT
String Event_Name = "fingerprint";
String Key = "bTqsHemd3m9Aog37VEfUljNzJ4RzKBEedVIoJ-bpevV";
String resource = "/trigger/" + Event_Name + "/with/key/" + Key;
const char* server = "maker.ifttt.com";

// Sensor sidik jari
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial2);
const char* NAME;
const char* ID;

// Inisialisasi LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Inisialisasi Hex Keypad
const byte ROWS = 4; // Empat baris
const byte COLS = 4; // Empat kolom
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {13, 12, 14, 27}; 
byte colPins[COLS] = {26, 25, 33, 32}; 
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Deklarasi fungsi
uint8_t getFingerprintIDez();
void makeIFTTTRequest(String action);

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);

  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();
  lcd.print("WELCOME TO");
  lcd.setCursor(0, 1);
  lcd.print("RTF LIBRARY");

  // Inisialisasi sensor sidik jari
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  // Koneksi WiFi
  Serial.print("Connecting to: ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  int timeout = 40;
  while (WiFi.status() != WL_CONNECTED && (timeout-- > 0)) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect, going back to sleep");
  }
  Serial.print("WiFi connected in: ");
  Serial.print(millis());
  Serial.print(", IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  char key = keypad.getKey();

  if (key == 'A') {
    lcd.clear();
    lcd.print("Peminjaman");
    makeIFTTTRequest("Peminjaman");
  } else if (key == 'B') {
    lcd.clear();
    lcd.print("Pengembalian");
    makeIFTTTRequest("Pengembalian");
  } else {
    // Tetapkan tindakan default jika tombol selain A dan B ditekan
  }

  getFingerprintIDez();
  
  if (finger.fingerID == 1) {
    NAME = "Ahmad Rifaat";
    ID = "1";
    if (finger.confidence >= 60) {
      Serial.print("Attendance Marked for ");
      Serial.println(NAME);
      lcd.clear();
      lcd.print("Halo ");
      lcd.setCursor(0, 1);
      lcd.print(NAME);
      makeIFTTTRequest("Peminjaman");
    }
  } else if (finger.fingerID == 11) {
    NAME = "JOY";
    ID = "11";
    if (finger.confidence >= 60) {
      Serial.print("Attendance Marked for ");
      Serial.println(NAME);
      lcd.clear();
      lcd.print("Halo ");
      lcd.print("Nigga");
      makeIFTTTRequest("Pengembalian");
    }
  }
  finger.fingerID = 0;
}

uint8_t getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -1;
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);
  return finger.fingerID;
}

void makeIFTTTRequest(String action) {
  WiFiClient client;
  if (!client.connect(server, 80)) {
    Serial.println("Failed to connect...");
    return;
  }

  String jsonObject = String("{\"value1\":\"") + NAME + "\",\"value2\":\"" + ID + "\",\"value3\":\"" + action + "\"}";
  client.println(String("POST ") + resource + " HTTP/1.1");
  client.println("Host: maker.ifttt.com");
  client.println("Connection: close");
  client.println("Content-Type: application/json");
  client.print("Content-Length: ");
  client.println(jsonObject.length());
  client.println();
  client.println(jsonObject);

  while (client.available()) {
    Serial.write(client.read());
  }

  client.stop();
}
