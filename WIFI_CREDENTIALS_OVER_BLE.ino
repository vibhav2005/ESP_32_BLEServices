// Include necessary libraries
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <WiFi.h>

// Define UUIDs for the service and characteristics
#define Service_id "00000002-0000-0000-FDFD-FDFDFDFDFDFD"
#define ssid_Char_id "1efd327d-7caa-4dad-90d8-d2b87dfc4882" 
#define pass_Char_id "18ad1902-ac57-488c-b5fe-c6c8b4041860"  

// Create necessary pointers
BLECharacteristic *pSsid;
BLECharacteristic *pPass;
BLEAdvertising *padvertising;

//declaring neccessary global variables and flags 
String ssid;
String pass;
static bool wifiConnected = false;
int i;

// Setup function for BLE initialization
void setup() {
  Serial.begin(115200); 
  Serial.println("BLE SERVICE BEGIN");

  pinMode(2, OUTPUT); // Indicator pin

  BLEDevice::init("ESP32 SERVICE"); // Initialize BLE

  // Create BLE server and service
  BLEServer *pserver = BLEDevice::createServer();
  BLEService *pservice = pserver->createService(Service_id);
  
  // Create characteristics for SSID and password
  pSsid = pservice->createCharacteristic(ssid_Char_id,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE);

  pPass = pservice->createCharacteristic(pass_Char_id,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE);
 
  // Start BLE service and advertising
  pservice->start();
  padvertising = BLEDevice::getAdvertising();
  padvertising->addServiceUUID(Service_id);
  padvertising->start();

  digitalWrite(2, HIGH); // Indicate that setup is complete
}


// Loop function
void loop() {
  wifi_begin(); // In loo we begin with WiFi connection process
}


// Function to handle WiFi connection process 

void wifi_begin() {
  
  if(wifiConnected == false){     //check if the wifi is connected or not through a flag if not then it will enter the condition to retrieve necessary credentials from the client

  //here's the basic logic of reading values from the client and converting it to const char* type since the begin condition does not take string as data type
  Serial.println("connecting to wifi...");
  i = 0;
  // Retrieve SSID and password from BLE characteristics
  ssid = pSsid->getValue().c_str();
  pass = pPass->getValue().c_str();
  
  // Check if SSID or password is empty as initially we created empty strings 
  if (ssid.isEmpty() || pass.isEmpty()) {
    Serial.println("No SSID or password set. Waiting for credentials...");
    delay(1000);
    return; // Exit function if SSID or password remains empty
  }

  // Attempt to connect to WiFi
  WiFi.begin(ssid.c_str(), pass.c_str());
  
  // Wait for WiFi connection
  //Also adding a condition i so that it dosen't remain in infinite loop if credentials are wrong
  while (WiFi.status() != WL_CONNECTED&&i<10) {
    delay(1000);
    i++;
    Serial.print(".");
    }
  }
  
  // Check if WiFi is connected
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    
  // Indicate WiFi connection success
    Serial.println("\nWiFi Connected");
    wifi_connected();
  }
  else {
    wifiConnected = false;
    digitalWrite(2, LOW); // Indicate WiFi connection failed
    Serial.println("\nWiFi Connection Failed");
  }
  
  padvertising->start(); // Restart BLE advertising
}

//created another function when wifi connects succesfully the led blinks and it dosent get out of the loop
void wifi_connected(){
  while(wifiConnected){
    digitalWrite(2,HIGH);
    delay(1000);
    digitalWrite(2,LOW);
    delay(1000);
  }
  
}
