//Wiring
/*
          Türkçe

- **MQ-2 Gaz Sensörü**:
  - VCC pinini Deneyap Kart 1A'nın 5V pinine bağlayın.
  - GND pinini Deneyap Kart 1A'nın GND pinine bağlayın.
  - AOUT (Analog Çıkış) pinini, Deneyap Kart 1A'nın A0 pinine bağlayın.
- **Aktif Buzzer**:
  - Buzzerın + pinini Deneyap Kart 1A'nın D0 pinine bağlayın.
  - Buzzerın - pinini GND pinine bağlayın.
- **Deneyap Basınç Ölçer**: 
  - I2C haberleşme konnektörü üzerinden Deneyap Kart 1A'ya zincirleme (daisy-chain) bağlantı yaparak bağlayın.

          English

- **MQ-2 Gas Sensor**:
  - Connect the VCC pin to the 5V pin of the Deneyap Kart 1A.
  - Connect the GND pin to the GND pin of the Deneyap Kart 1A.
  - Connect the AOUT (Analog Output) pin to the A0 pin of the Deneyap Kart 1A.
- **Active Buzzer**:
  - Connect the + pin of the buzzer to the D0 pin of the Deneyap Kart 1A.
  - Connect the - pin of the buzzer to the GND pin.
- **Deneyap Pressure Measurer**: 
  - Connect in a daisy-chain configuration to Deneyap Kart 1A via the I2C communication connector.
*/

//Arduino IDE Code

#include <Deneyap_BasincOlcer.h>
#include <DeneyapTelegram.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

AtmosphericPressure BaroSensor;
DeneyapTelegram telegram;
WiFiClientSecure client;

// WiFi ağınızın adını giriniz. (Enter your WiFi SSID here.)
const char* ssid = "YOUR_WIFI_SSID";  

// WiFi şifrenizi giriniz. (Enter your WiFi password here.)
const char* password = "YOUR_WIFI_PASSWORD";  

// Telegram bot tokeninizi giriniz. (Enter your Telegram bot token here.)
#define BOTtoken "YOUR_BOT_TOKEN"  

// Telegram chat kimliğinizi giriniz. (Enter your Telegram chat ID here.)
#define CHAT_ID "YOUR_CHAT_ID" 

const float maxTemperature = 50.0;
const float minTemperature = -10.0;

const float maxPressure = 1600.0;
const float minPressure = 600.0;

const int maxGasAnalog = 2000;
const int minGasAnalog = 50;

#define buzzerPin D0
#define MQ2_ANALOG_PIN A0

void setup() {
  Serial.begin(115200);
  delay(1000);

  BaroSensor.begin(0x76);
  pinMode(buzzerPin, OUTPUT);

  client.setInsecure();
  Serial.print("WiFi'ye bağlanılıyor: (Connecting to WiFi: )");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("WiFi'ye bağlandı. (Connected to WiFi.)");
  Serial.print("IP adresi: (IP Address: )");
  Serial.println(WiFi.localIP());

  telegram.begin(BOTtoken);
  telegram.send(CHAT_ID, "Cihaz bağlandı. (Device connected.)");

  pinMode(buzzerPin, OUTPUT);
  pinMode(MQ2_ANALOG_PIN, INPUT);
}

void loop() {
  float pressure = BaroSensor.getPressure();
  float temperature = BaroSensor.getTemp();
  int gasAnalog = analogRead(MQ2_ANALOG_PIN);

  Serial.print("Basınç: (Pressure: )");
  Serial.println(pressure);
  Serial.print("Sıcaklık: (Temperature: )");
  Serial.println(temperature);
  Serial.print("Gaz Seviyesi: (Gas Level: )");
  Serial.println(gasAnalog);

  if (temperature > maxTemperature) {
    telegram.send(CHAT_ID, "Sıcaklık tehlikeli seviyeye ulaştı. (Temperature reached dangerous level.)");
    String tempMessage = "Sıcaklık (Temperature): " + String(temperature) + " °C";
    telegram.send(CHAT_ID, tempMessage);
    digitalWrite(buzzerPin, HIGH);
  } else if (temperature < minTemperature) {
    telegram.send(CHAT_ID, "Sıcaklık tehlikeli seviyeye düştü. (Temperature dropped to dangerous level.)");
    String tempMessage = "Sıcaklık (Temperature): " + String(temperature) + " °C";
    telegram.send(CHAT_ID, tempMessage);
    digitalWrite(buzzerPin, HIGH);
  }

  if (pressure > maxPressure) {
    telegram.send(CHAT_ID, "Basınç tehlikeli seviyeye ulaştı. (Pressure reached dangerous level.)");
    String pressureMessage = "Basınç (Pressure): " + String(pressure) + " hPa";
    telegram.send(CHAT_ID, pressureMessage);
    digitalWrite(buzzerPin, HIGH);
  } else if (pressure < minPressure) {
    telegram.send(CHAT_ID, "Basınç tehlikeli seviyeye düştü. (Pressure dropped to dangerous level.)");
    String pressureMessage = "Basınç (Pressure): " + String(pressure) + " hPa";
    telegram.send(CHAT_ID, pressureMessage);
    digitalWrite(buzzerPin, HIGH);
  }

  if (gasAnalog > maxGasAnalog) {
    telegram.send(CHAT_ID, "Gaz tehlikeli seviyeye ulaştı. (Gas reached dangerous level.)");
    String gasMessage = "Gaz seviyesi (Gas level): " + String(gasAnalog);
    telegram.send(CHAT_ID, gasMessage);
    digitalWrite(buzzerPin, HIGH);
  } else if (gasAnalog < minGasAnalog) {
    telegram.send(CHAT_ID, "Gaz tehlikeli seviyeye düştü. (Gas dropped to dangerous level.)");
    String gasMessage = "Gaz seviyesi (Gas level): " + String(gasAnalog);
    telegram.send(CHAT_ID, gasMessage);
    digitalWrite(buzzerPin, HIGH);
  }

  if (temperature >= minTemperature && temperature <= maxTemperature &&
      pressure >= minPressure && pressure <= maxPressure &&
      gasAnalog >= minGasAnalog && gasAnalog <= maxGasAnalog) {
    digitalWrite(buzzerPin, LOW);
  }

  delay(1000);
}