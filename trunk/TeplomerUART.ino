// A0 - DALLAS temperature sensors

#define verbose

#include <OneWire.h>
#define ONE_WIRE_BUS A0
OneWire onewire(ONE_WIRE_BUS); // pin for onewire DALLAS bus
#define dallasMinimal //-956 Bytes
#ifdef dallasMinimal
#include <DallasTemperatureMinimal.h>
DallasTemperatureMinimal dsSensors(&onewire);
#else
#include <DallasTemperature.h>
DallasTemperature dsSensors(&onewire);
#endif
DeviceAddress tempDeviceAddress;
#ifndef NUMBER_OF_DEVICES
#define NUMBER_OF_DEVICES 15
#endif
DeviceAddress tempDeviceAddresses[NUMBER_OF_DEVICES];
//int  resolution = 12;
unsigned int numberOfDevices; // Number of temperature devices found
unsigned long lastDsMeasStartTime;
bool dsMeasStarted=false;
float sensor[NUMBER_OF_DEVICES];
unsigned int const dsMeassureInterval=750; //inteval between meassurements
unsigned long lastMeasTime=0;

#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11); // RX, TX

float versionSW=0.1;
char versionSWString[] = "TeplomerUART v"; //SW name & version

void dsInit(void);

void setup() {
  #ifdef verbose
  Serial.begin(115200);
  Serial.println(versionSW);
  #endif
	
	mySerial.begin(4800);
  mySerial.println("Hello, world?");

  dsInit();
}
 
void loop() {
  if (!dsMeasStarted) {
    //start sampling
    dsMeasStarted=true;
    dsSensors.requestTemperatures(); 
    //digitalWrite(13,HIGH);
    lastDsMeasStartTime = millis();
  }
  else if (dsMeasStarted && (millis() - lastDsMeasStartTime>dsMeassureInterval)) {
    dsMeasStarted=false;
    //digitalWrite(13,LOW);
    //saving temperatures into variables
    for (byte i=0;i<numberOfDevices; i++) {
      float tempTemp=-126;
      for (byte j=0;j<10;j++) { //try to read temperature ten times
        //tempTemp = dsSensors.getTempCByIndex(i);
        tempTemp = dsSensors.getTempC(tempDeviceAddresses[i]);
        if (tempTemp>=-55) {
          break;
        }
      }
      sensor[i] = tempTemp;
			
	
			for (byte j=0; j<8; j++) {
				if (tempDeviceAddresses[i][j]<16) {
					mySerial.print(0,HEX);
					Serial.print(0,HEX);
				}
					mySerial.print(tempDeviceAddresses[i][j],HEX);
					Serial.print(tempDeviceAddresses[i][j],HEX);
			}
			
      Serial.print("-");
			mySerial.print("-");
      Serial.println(sensor[i]);
			mySerial.println(sensor[i]);
			
    } 
  }
}

void dsInit(void) {
  dsSensors.begin();
  numberOfDevices = dsSensors.getDeviceCount();

  #ifdef verbose
  Serial.print("Sensor(s):");
  Serial.println(numberOfDevices);
  #endif

  // Loop through each device, print out address
  for (byte i=0;i<numberOfDevices; i++) {
      // Search the wire for address
    if (dsSensors.getAddress(tempDeviceAddress, i)) {
      memcpy(tempDeviceAddresses[i],tempDeviceAddress,8);
    }
  }
  #ifndef dallasMinimal
  dsSensors.setResolution(12);
  dsSensors.setWaitForConversion(false);
  #endif

}

