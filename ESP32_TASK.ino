//including all the necessary BLE libraries.
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <BLE2901.h>

//defining UUID'S for the service and characteristics.
#define Service_id "00000002-0000-0000-FDFD-FDFDFDFDFDFD"
#define Temp_Char_id "1efd327d-7caa-4dad-90d8-d2b87dfc4882" 
#define Humidity_Char_id "18ad1902-ac57-488c-b5fe-c6c8b4041860"  

//creating necessary pointers.
BLECharacteristic *pTemp ;
BLECharacteristic *pHumidity;
BLEAdvertising *padvertising;

//Variable that act as demo sensor variables.
int Temp = 0;
int Humidity = 0; 

//all initializations done in this section.
void setup() {
  Serial.begin(115200); 
  Serial.println("BLE SERVICE BEGIN");
//setting an hardware indicator.
  pinMode(2,OUTPUT);

//initializing the BLE.
  BLEDevice::init("ESP32 SERVICE");

//creating a personal server named esp32 service and creating a custom service for it.
  BLEServer *pserver = BLEDevice::createServer();
  BLEService *pservice = pserver->createService(Service_id);
  
//creating characteristics for the service being provide by the server.
  pTemp = pservice->createCharacteristic(Temp_Char_id ,
                                         BLECharacteristic::PROPERTY_READ 
                                        |BLECharacteristic::PROPERTY_NOTIFY );

  pHumidity = pservice->createCharacteristic(Humidity_Char_id ,
                                             BLECharacteristic::PROPERTY_READ 
                                            |BLECharacteristic::PROPERTY_NOTIFY );
//sending the initial values to the charachteristics  
  pTemp->setValue((uint8_t*)&Temp , sizeof(float)); 
  pHumidity->setValue((uint8_t*)&Humidity , sizeof(float));

//adding a user and client descriptor for temperature characterstics.

  pTemp->addDescriptor(new BLE2902());//client desciptor is necessary to enable the notifications as they are not enabled default.
  BLE2901 *User_2901 ;
  User_2901 = new BLE2901();
  User_2901->setDescription("This section shows the Temperature characteristics");
  pTemp->addDescriptor(User_2901);

//adding a user and client descriptor for Humidity characterstics
  pHumidity->addDescriptor(new BLE2902());
  BLE2901 *User2_2901 ;
  User2_2901 = new BLE2901();
  User2_2901->setDescription("This section shows the Humidity characteristics");
  pHumidity->addDescriptor(User2_2901);
  
//starting the services after all initialisations.
  pservice->start();
  
//initialising the advertisement and starting it so that the service is seen by the client.
   padvertising = BLEDevice::getAdvertising();
   padvertising->addServiceUUID(Service_id);
   padvertising->start();
//Using it as an indicator to know that code executes perfectly till here.   
  digitalWrite(2,HIGH);

}

void loop() {
//updating the demo sensors value and keep notifying to get the continuos update on NRF Connect or on our client.
   pTemp->setValue(Temp); 
   pTemp->notify();
   Temp++;
   delay(1000);
   pHumidity->setValue(Humidity);
   pHumidity->notify();
   Humidity++;
   delay(1000);
//again start the advertising to get the continuous updates
   padvertising->start();

}
