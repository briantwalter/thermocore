//
// thermocore-alpha     Thermocore heating control ALPHA
// version              0.0.1
// author               Brian Walter @briantwalter
// description          Spark.io core control of my furnace, 
//                      dehumidifier, and electronic air filter 

// includes
#include "Adafruit_DHT/Adafruit_DHT.h"

// defines
#define pinFan 1		// 24 VAC relay internal to furnace
#define pinHeat 2		// 24 VAC relay internal to furnace
#define pinFilter 3		// 110 AC relay internal to furnace
#define pinHumidifier 4		// 110 AC relay external to furnace
#define dhtPinUpstairs 5	// DTH11 in upstairs location
#define dhtPinDownstairs 6	// DTH11 in downstairs location
#define dhtType DHT11		// Type of DHT <DHT11|DTH22>

// desired settings
int myState = 0;	// system 0=off, 1=on
int myTemp = 0;		// degrees fahrenheit
int myHumid = 0;	// percent humidity

// sensor readings
int upTemp = 0; 	// upstairs temperature
int upHumid = 0;	// upstairs humidity
int dnTemp = 0; 	// downstairs temperature
int dnHumid = 0;	// downstairs humidity

// create DTH sensor handlers
DHT dhtUpstairs(dhtPinUpstairs, dhtType);
DHT dhtDownstairs(dhtPinDownstairs, dhtType);

// runs once
void setup() {
  // expose cloud variables
  Spark.variable("mystate", &myState, INT);
  Spark.variable("mytemp", &myTemp, INT);
  Spark.variable("myhumid", &myHumid, INT);
  Spark.variable("uptemp", &upTemp, INT);
  Spark.variable("uphumid", &upHumid, INT);
  Spark.variable("dntemp", &dnTemp, INT);
  Spark.variable("dnhumid", &dnHumid, INT);
  // expose cloud functions
  Spark.function("tempcall", tempCall);
  Spark.function("syscall", sysCall);
  //Spark.function("appcall", appCall);
  // start sensor readings
  dhtUpstairs.begin();
  dhtDownstairs.begin();
  // setup controls
  ctrlFan("setup");
  ctrlHeat("setup");
  ctrlFilter("setup");
  ctrlHumidifier("setup");
}

// runs forever
void loop() {
}

// FUNCTIONS
//// PRIMATIVES START
// get current temperature for given location
int getTemp(String location) {
  if (location == "upstairs") {
    upTemp = dhtUpstairs.getTempFarenheit();
    return upTemp;
  }
  if (location == "downstairs") {
    dnTemp = dhtDownstairs.getTempFarenheit();
    return dnTemp;
  }
  else {
    return -1;
  }
}
// get current humidity for given location
int getHumid(String location) {
  if (location == "upstairs") {
    upHumid = dhtUpstairs.getHumidity();
    return upHumid;
  }
  if (location == "downstairs") {
    dnHumid = dhtDownstairs.getHumidity();
    return dnHumid;
  }
  else {
    return -1;
  }
}
// control for furnace fan
int ctrlFan(String action) {
  if (action == "setup") {
    pinMode(pinFan, OUTPUT);
    digitalWrite(pinFan, LOW);
    return 0;
  }
  if (action == "on") {
    digitalWrite(pinFan, HIGH);
    return 0;
  }
  if (action == "off") {
    digitalWrite(pinFan, LOW);
    return 0;
  }
  if (action == "status") {
    int value = 0;
    value = digitalRead(pinFan);
    return value;
  }
  else {
    return -1;
  }
}
// control for furnace heat
int ctrlHeat(String action) {
  if (action == "setup") {
    pinMode(pinHeat, OUTPUT);
    digitalWrite(pinHeat, LOW);
    return 0;
  }
  if (action == "on") {
    digitalWrite(pinHeat, HIGH);
    return 0;
  }
  if (action == "off") {
    digitalWrite(pinHeat, LOW);
    return 0;
  }
  if (action == "status") {
    int value = 0;
    value = digitalRead(pinHeat);
    return value;
  }
  else {
    return -1;
  }
}
// control for electornic air filter
int ctrlFilter(String action) {
  if (action == "setup") {
    pinMode(pinFilter, OUTPUT);
    digitalWrite(pinFilter, LOW);
    return 0;
  }
  if (action == "on") {
    digitalWrite(pinFilter, HIGH);
    return 0;
  }
  if (action == "off") {
    digitalWrite(pinFilter, LOW);
    return 0;
  }
  if (action == "status") {
    int value = 0;
    value = digitalRead(pinFilter);
    return value;
  }
  else {
    return -1;
  }
}
// control for external dehumidifier
int ctrlHumidifier(String action) {
  if (action == "setup") {
    pinMode(pinHumidifier, OUTPUT);
    digitalWrite(pinHumidifier, LOW);
    return 0;
  }
  if (action == "on") {
    digitalWrite(pinHumidifier, HIGH);
    return 0;
  }
  if (action == "off") {
    digitalWrite(pinHumidifier, LOW);
    return 0;
  }
  if (action == "status") {
    int value = 0;
    value = digitalRead(pinHumidifier);
    return value;
  }
  else {
    return -1;
  }
}
//// PRIMATIVES END
//// TEMP CALL START
int tempCall(String temp) {
  myTemp = temp.toInt();
  return 0;
}
//// TEMP CALL END
//// SYSTEM CALL START
int sysCall(String action) {
  if ( action == "off" || action == "on" || action == "status" ) {
    int valFan = ctrlFan(action);
    int valHeat = ctrlHeat(action);
    int valFilter = ctrlFilter(action);
    int valHumidifier = ctrlHumidifier(action);
    if ( action == "status" ) {
      int rc = (( valFan * 1000 ) + ( valHeat * 100 ) + ( valFilter * 10 ) + valHumidifier );
      return rc;
    }
    if ( valFan != 0 || valHeat != 0 || valFilter != 0 || valHumidifier != 0 ) {
      return -1;
    }
    else {
      return 0;
    }
  }
  if ( action == "diag" ) {
    int sleep = 2000;
    ctrlFan("on");
    delay(sleep);
    ctrlFan("off");
    delay(sleep);
    ctrlHeat("on");
    delay(sleep);
    ctrlHeat("off");
    delay(sleep);
    ctrlFilter("on");
    delay(sleep);
    ctrlFilter("off");
    delay(sleep);
    ctrlHumidifier("on");
    delay(sleep);
    ctrlHumidifier("off");
    return 0;
  }
  else {
    return -1;
  }
}
//// SYSTEM CALL END
//// APPLICATION CALL START
int appCall(String action) {
}
//// APPLICAITON CALL END
