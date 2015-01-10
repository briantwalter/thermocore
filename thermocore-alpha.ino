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
  Spark.variable("uptemp", &upTemp, INT);
  Spark.variable("uphumid", &upHumid, INT);
  Spark.variable("dntemp", &dnTemp, INT);
  Spark.variable("dnhumid", &dnHumid, INT);
  // expose cloud functions
  Spark.function("settemp", setTemp);
  Spark.function("syscall", sysCall);
  // start sensor readings
  dhtUpstairs.begin();
  dhtDownstairs.begin();
  // setup controls
  pinMode(pinFan, OUTPUT);
  digitalWrite(pinFan, LOW);
  pinMode(pinHeat, OUTPUT);
  digitalWrite(pinHeat, LOW);
  pinMode(pinFilter, OUTPUT);
  digitalWrite(pinFilter, LOW);
  pinMode(pinHumidifier, OUTPUT);
  digitalWrite(pinHumidifier, LOW);
}

// runs forever
void loop() {
  // populate sensor variables
  upTemp = sysCall("getTempUpstairs");
  upHumid = sysCall("getHumidUpstairs");
  dnTemp = sysCall("getTempDownstairs");
  dnHumid = sysCall("getHumidDownstairs");
  // check to make sure the state is enabled and temperature is non-zero
  if ( myState == 1 && myTemp != 0 ) {
    int actualTemp = (( upTemp + dnTemp ) / 2 ); // average up and down
    int heatStatus = sysCall("getHeatStatus");
    if ( actualTemp < ( myTemp - 2 )) {
      if ( heatStatus == 0 ) {
        sysCall("setHeatOn");
      }
    }
    if ( actualTemp >= ( myTemp - 2 )) {
      if ( heatStatus == 1 ) {
        sysCall("setHeatOff");
      }
    }
  }
  delay(2000); // need wait 2 seconds in between sensor readings
}

// functions
//// PRIMATIVES DO NOT CALL THESE USE sysCall(action)
// control for furnace fan
int ctrlFan(String action) {
  if (action == "on") {
    digitalWrite(pinFan, HIGH);
    return 0;
  }
  if (action == "off") {
    digitalWrite(pinFan, LOW);
    return 0;
  }
  if (action == "status") {
    return digitalRead(pinFan);
  }
  else {
    return -1;
  }
}
// control for furnace heat
int ctrlHeat(String action) {
  if (action == "on") {
    digitalWrite(pinHeat, HIGH);
    return 0;
  }
  if (action == "off") {
    digitalWrite(pinHeat, LOW);
    return 0;
  }
  if (action == "status") {
    return digitalRead(pinHeat);
  }
  else {
    return -1;
  }
}
// control for electornic air filter
int ctrlFilter(String action) {
  if (action == "on") {
    digitalWrite(pinFilter, HIGH);
    return 0;
  }
  if (action == "off") {
    digitalWrite(pinFilter, LOW);
    return 0;
  }
  if (action == "status") {
    return digitalRead(pinFilter);
  }
  else {
    return -1;
  }
}
// control for external dehumidifier
int ctrlHumidifier(String action) {
  if (action == "on") {
    digitalWrite(pinHumidifier, HIGH);
    return 0;
  }
  if (action == "off") {
    digitalWrite(pinHumidifier, LOW);
    return 0;
  }
  if (action == "status") {
    return digitalRead(pinHumidifier);
  }
  else {
    return -1;
  }
}
//// PRIMATIVES END
//// SET TEMP CALL START
int setTemp(String temp) {
  myTemp = temp.toInt();
  return 0;
}
//// SET TEMP CALL END
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
  if ( action == "getTempDownstairs") {
    return dhtDownstairs.getTempFarenheit();
  }
  if ( action == "getTempUpstairs") {
    return dhtUpstairs.getTempFarenheit();
  }
  if ( action == "getHumidDownstairs") {
    return dhtDownstairs.getHumidity();
  }
  if ( action == "getHumidUpstairs") {
    return dhtUpstairs.getHumidity();
  }
  if ( action == "setStateOn") {
    myState = 1;
    return 0;
  }
  if ( action == "setStateOff") {
    myState = 0;
    return 0;
  }
  if ( action == "setFanOn") {
    return ctrlFan("on");
  }
  if ( action == "setFanOff") {
    return ctrlFan("off");
  }
  if ( action == "getFanStatus") {
    return ctrlFan("status");
  }
  if ( action == "setHeatOn") {
    return ctrlHeat("on");
  }
  if ( action == "setHeatOff") {
    return ctrlHeat("off");
  }
  if ( action == "getHeatStatus") {
    return ctrlHeat("status");
  }
  if ( action == "setFilterOn") {
    return ctrlFilter("on");
  }
  if ( action == "setFilterOff") {
    return ctrlFilter("off");
  }
  if ( action == "getFilterStatus") {
    return ctrlFilter("status");
  }
  if ( action == "setHumidifierOn") {
    return ctrlHumidifier("on");
  }
  if ( action == "setHumidifierOff") {
    return ctrlHumidifier("off");
  }
  if ( action == "getHumidifierStatus") {
    return ctrlHumidifier("status");
  }
  else {
    return -1;
  }
}
//// SYSTEM CALL END
