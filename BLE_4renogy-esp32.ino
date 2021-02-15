/*
    Bluetooth to serial converter for Renogy/srne solar controllers. Needs an rs232 to 3.3v TTL converter on serial2
    serial pinout on controller is TX RX GND GND 12v 12v
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;

#define NOTIFY_SERVICE_UUID      "0000FFF0-0000-1000-8000-00805F9B34FB"
#define NOTIFY_CHAR_UUID         "0000FFF1-0000-1000-8000-00805F9B34FB"

#define WRITE_SERVICE_UUID       "0000FFD0-0000-1000-8000-00805F9B34FB"
#define WRITE_CHAR_UUID_POLLING  "0000FFD1-0000-1000-8000-00805F9B34FB"
//#define WRITE_CHAR_UUID_COMMANDS "0000FFD1-0000-1000-8000-00805F9B34FB"



class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        for (int i = 0; i < rxValue.length(); i++)
          Serial2.print(rxValue[i]);
      }
    }
};


void setup() {
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  //Serial.begin(115200);

  // Create the BLE Device
  BLEDevice::init("gimmedata");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Services
  BLEService *pService = pServer->createService(NOTIFY_SERVICE_UUID);
 
  BLEService *pService2 = pServer->createService(WRITE_SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
                        NOTIFY_CHAR_UUID ,
                        BLECharacteristic::PROPERTY_NOTIFY
                      );

  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
      WRITE_CHAR_UUID_POLLING,
      BLECharacteristic::PROPERTY_WRITE
                                          );

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  
}

void loop() {

  if (deviceConnected) {
    if (Serial2.available()) {
      txValue = Serial2.read();
      pTxCharacteristic->setValue(&txValue, 1);
      pTxCharacteristic->notify();
      //txValue++;
      //delay(10); // bluetooth stack will go into congestion, if too many packets are sent
    }
  }

  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }
}
