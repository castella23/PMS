#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 10

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;

byte expectedUID[] = {0x83, 0x84, 0x89, 0xF5}; // Update this UID as needed

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("==== RFID PARKING PAYMENT ====");
  Serial.println("Place your card near the reader...");

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  if (!verifyUID()) {
    Serial.println("❌ UID mismatch. Access denied.");
    haltCard();
    return;
  }

  String carPlate = readBlock(2, "Car Plate");
  String balanceStr = readBlock(4, "Balance");
  long currentBalance = balanceStr.toInt();

  Serial.println();
  Serial.println("===== Card Info =====");
  Serial.print("🚗 Car Plate : ");
  Serial.println(carPlate);
  Serial.print("💰 Balance    : ");
  Serial.println(currentBalance);
  Serial.println("=====================");
  
  // Send to Python
  Serial.print("DATA:");
  Serial.print(carPlate);
  Serial.print(",");
  Serial.println(currentBalance);

  // Wait for Python CHARGE
  while (Serial.available() == 0) {}
  String chargeMsg = Serial.readStringUntil('\n');
  if (!chargeMsg.startsWith("CHARGE:")) {
    Serial.println("❌ Invalid CHARGE format.");
    haltCard();
    return;
  }

  long charge = chargeMsg.substring(7).toInt();
  long newBalance = currentBalance - charge;

  if (newBalance < 0) {
    Serial.println("❌ Insufficient funds!");
    haltCard();
    return;
  }

  // Write new balance
  if (writeBlock(4, String(newBalance))) {
    Serial.println("DONE"); // signal to Python
    Serial.println("===== Payment Summary =====");
    Serial.print("Plate        : "); Serial.println(carPlate);
    Serial.print("Charge       : "); Serial.println(charge);
    Serial.print("Old Balance  : "); Serial.println(currentBalance);
    Serial.print("New Balance  : "); Serial.println(newBalance);
    Serial.println("============================");
  }

  haltCard();
  delay(2000);
}

// Verify if UID matches expected
bool verifyUID() {
  if (rfid.uid.size != sizeof(expectedUID)) return false;
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] != expectedUID[i]) return false;
  }
  return true;
}

// Read from RFID block
String readBlock(byte blockNum, String label) {
  byte buffer[18];
  byte size = sizeof(buffer);

  status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(rfid.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("❌ Auth failed ("); Serial.print(label); Serial.println(")");
    return "[Auth Fail]";
  }

  status = rfid.MIFARE_Read(blockNum, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("❌ Read failed ("); Serial.print(label); Serial.println(")");
    return "[Read Fail]";
  }

  String data = "";
  for (byte i = 0; i < 16; i++) {
    if (buffer[i] != ' ') data += (char)buffer[i];
  }
  data.trim();
  return data;
}

// Write to RFID block
bool writeBlock(byte blockNum, String data) {
  byte buffer[16];
  for (byte i = 0; i < 16; i++) {
    buffer[i] = (i < data.length()) ? data[i] : ' ';
  }

  status = rfid.MIFARE_Write(blockNum, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("❌ Write failed: ");
    Serial.println(rfid.GetStatusCodeName(status));
    return false;
  }
  return true;
}

// End communication with card
void haltCard() {
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
