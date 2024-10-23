//------------------ SET 307 or 308 Mode on remote ----------------
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRutils.h>

#define buzzer_pin 25

const uint16_t kRecvPin = 13;

IRrecv irrecv(kRecvPin);

decode_results results;

bool acPower = false;
int temperatureSetpoint = 0;  // Default temperature
String acMode = "";
String fanSpeed = "";
bool swing = false;

void Decoder_Remote(uint64_t rawData) {
  if ((rawData >> 40) == 0xC0) {
    acPower = 0;
  } else {
    acPower = 1;
  }
  uint8_t tempLSB = (rawData >> 8) & 0xFF;
  temperatureSetpoint = tempLSB - 92;

  uint8_t Mode = (rawData >> 36) & 0xF;

  switch (Mode) {

    case 1:
      acMode = "HEAT";
      break;
    case 2:
      acMode = "COOL";
      break;
    case 3:
      acMode = "DROP";
      break;
    case 4:
      acMode = "AUTO";
      break;
    case 5:
      acMode = "DRY";
      break;
    default:
      acMode = "Unknown";
      break;
  }

  switch ((rawData >> 32) & 0x0F) {  // 2 bits for fan speed
    case 0b00:
      fanSpeed = "Auto";
      break;
    case 0b01:
      fanSpeed = "High";
      break;
    case 0b10:
      fanSpeed = "Low";
      break;
    case 0b11:
      fanSpeed = "Medium";
      break;
    default:
      fanSpeed = "Unknown";
      break;
  }
}
void setup() {
  Serial.begin(115200);
  irrecv.enableIRIn();  // Start the receiver
  pinMode(buzzer_pin, OUTPUT);
  digitalWrite(buzzer_pin, HIGH);
  delay(500);
  digitalWrite(buzzer_pin, LOW);
  delay(500);
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.println();
    Serial.println("---------------------------------------");
    Serial.println();
    Serial.println(resultToHumanReadableBasic(&results));
    Serial.println(results.value, BIN);

    if (results.bits == 48 && results.decode_type == KELON) {
      Decoder_Remote(results.value);
    }

    // Print the updated state
    digitalWrite(buzzer_pin, HIGH);
    delay(250);
    digitalWrite(buzzer_pin, LOW);
    delay(200);
    Serial.print("AC Power: ");
    Serial.println(acPower ? "On" : "Off");
    Serial.print("Temperature Setpoint: ");
    Serial.println(temperatureSetpoint);
    Serial.print("AC Mode: ");
    Serial.println(acMode);
    Serial.print("Fan Speed: ");
    Serial.println(fanSpeed);
    Serial.print("Swing: ");
    Serial.println(swing ? "On" : "Off");

    Serial.println();
    Serial.println("---------------------------------------");

    // Restart the receiver
    irrecv.resume();
  }
}