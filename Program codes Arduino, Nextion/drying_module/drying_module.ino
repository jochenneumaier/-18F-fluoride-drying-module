#include <Nextion.h>          // not included in the library manager, have to be downloaded manually:  https://github.com/bborncr/nextion
#include <SoftwareSerial.h>   // Software Serial Port (included in Arduino IDE "no download")
#include <OneWire.h>          // V 2.3.7 used, included in the library manager, see also: https://www.pjrc.com/teensy/td_libs_OneWire.html
#include <DallasTemperature.h>  //V3.9.0 used, included in the library manager, see also: https://github.com/milesburton/Arduino-Temperature-Control-Library
SoftwareSerial nextion(2, 3);// Nextion (TX, RX) via software serial
Nextion myNextion(nextion, 9600); // Setup of Nextion library with name NextionLCD at 9600bps

#define ONE_WIRE_BUS 10 //Data from Temp Sensor
OneWire oneWire (ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float temp = 0;
unsigned long sendtemp = 0;
int settemp = 20;
byte heating = 0;
#define heating1 8
#define valve1 4
#define valve2 5
#define valve3 6
#define valve4 7
#define heating1 8
#define heating2 9

byte tempmenu = 0;
byte dryingstatus = 0;

String message;   // received message for Nextion LCD

void setup()
{
  pinMode(valve1, OUTPUT);
  pinMode(valve2, OUTPUT);
  pinMode(valve3, OUTPUT);
  pinMode(valve4, OUTPUT);
  pinMode(heating1, OUTPUT);
  pinMode(heating2, OUTPUT);

  digitalWrite(valve1, HIGH); // 0V
  digitalWrite(valve2, HIGH); // 0V
  digitalWrite(valve3, HIGH); // 0V
  digitalWrite(valve4, HIGH); // 0V
  digitalWrite(heating1, HIGH); //0V
  digitalWrite(heating2, HIGH); //0V

  myNextion.init();
  nextion.begin(9600); // open the software serial port at 9600 bps:
  sensors.begin();
}

void loop() {

  //------------------------------------------------------------------------------------------
  //  buttons action
  //------------------------------------------------------------------------------------------

  message = myNextion.listen(); //check for message
  if (message != "") { // if a message is received...

    //---------------switch valve 1-----------------

    if (message == "65 2 2 1 ffff ffff ffff") {
      digitalWrite(valve1, LOW); // 5V
      delay (200);
      myNextion.setComponentValue("custom.valve1state", 1);
      myNextion.sendCommand("vis valve1nc,1");
    }
    if (message == "65 2 8 1 ffff ffff ffff") {
      digitalWrite(valve1, HIGH); // 0V
      delay (200);
      myNextion.setComponentValue("custom.valve1state", 0);
      myNextion.sendCommand("vis valve1nc,0");
    }

    //---------------switch valve 2-----------------
    if (message == "65 2 3 1 ffff ffff ffff") {
      digitalWrite(valve2, LOW); // 5V
      delay (200);
      myNextion.setComponentValue("custom.valve2state", 1);
      myNextion.sendCommand("vis valve2nc,1");
    }
    if (message == "65 2 9 1 ffff ffff ffff") {
      digitalWrite(valve2, HIGH); // 5V
      delay (200);
      myNextion.setComponentValue("custom.valve2state", 0);
      myNextion.sendCommand("vis valve2nc,0");
    }

    //---------------switch valve 3-----------------

    if (message == "65 2 c 1 ffff ffff ffff") {
      digitalWrite(valve3, LOW); // 5V
      delay (200);
      myNextion.setComponentValue("custom.valve3state", 1);
      myNextion.sendCommand("vis valve3nc,1");
    }
    if (message == "65 2 e 1 ffff ffff ffff") {
      digitalWrite(valve3, HIGH); // 0V
      delay (200);
      myNextion.setComponentValue("custom.valve3state", 0);
      myNextion.sendCommand("vis valve3nc,0");
    }

    //---------------switch valve 4-----------------

    if (message == "65 2 d 1 ffff ffff ffff") {
      digitalWrite(valve4, LOW); // 5V
      delay (200);
      myNextion.setComponentValue("custom.valve4state", 1);
      myNextion.sendCommand("vis valve4nc,1");
    }
    if (message == "65 2 f 1 ffff ffff ffff") {
      digitalWrite(valve4, HIGH); // 0V
      delay (200);
      myNextion.setComponentValue("custom.valve4state", 0);
      myNextion.sendCommand("vis valve4nc,0");
    }

    //---------------drying mode start conditions-----------------

    if (message == "65 0 1 1 ffff ffff ffff") {
      digitalWrite(valve1, LOW); // 5V
      digitalWrite(valve2, HIGH); // 0V
      digitalWrite(valve3, LOW); // 5V
      digitalWrite(valve4, LOW); // 5V
      delay (200);
    }

    //---------------drying mode switch button-----------------

    if (message == "65 1 5 1 ffff ffff ffff") {
      if (dryingstatus == 0) {
        digitalWrite(valve1, HIGH);
        digitalWrite(valve2, LOW);
        digitalWrite(valve3, HIGH);
        digitalWrite(valve4, HIGH);
        delay (200);
        dryingstatus = 1;
        myNextion.setComponentValue("drying.valvedrystate", 1);
        myNextion.sendCommand("vis q3,1");
      }
      else {
        digitalWrite(valve1, LOW); // 5V
        digitalWrite(valve2, HIGH); // 0V
        digitalWrite(valve3, LOW); // 5V
        digitalWrite(valve4, LOW); // 5V
        delay (200);
        dryingstatus = 0;
        myNextion.setComponentValue("drying.valvedrystate", 0);
        myNextion.sendCommand("vis q3,0");
      }
    }
    //------------ set Temp menu ----------------

    if ((message == "65 1 6 1 ffff ffff ffff") || (message == "65 2 4 1 ffff ffff ffff")) {    //drying temp
      tempmenu = 1;
    }

    do {
      message = myNextion.listen(); //check for message
      if (message == "65 3 2 1 ffff ffff ffff") {
        tempmenu = 0;
        sendtemp = 0;
        delay(200);
        settemp = myNextion.getComponentValue("temp.settemp");
        myNextion.setComponentText("drying.t2", String(settemp));
        myNextion.setComponentText("custom.t1", String(settemp));
      }
      if (message == "65 3 1 1 ffff ffff ffff") {
        tempmenu = 0;
        sendtemp = 0;
      }
    } while (tempmenu == 1);

  } //message

  // -------------- send Temp to Nextion from Sensor (in a text field)-------------------

  if (millis() - sendtemp >= 1000) {
    sendtemp = millis();
    sensors.requestTemperatures();
    temp = sensors.getTempCByIndex(0);
    nextion.print("custom.temp.txt=\"");
    nextion.print(temp, 1);         // 1 decimal place
    nextion.print("\"");
    nextion.write(0xff);
    nextion.write(0xff);
    nextion.write(0xff);
    nextion.print("drying.t1.txt=\"");
    nextion.print(temp, 0);         // 0 decimal place
    nextion.print("\"");
    nextion.write(0xff);
    nextion.write(0xff);
    nextion.write(0xff);
  }

  // -------------- heating-------------------

  if (temp < (settemp - 1)) {
    heating = 1;
  }
  if (temp > (settemp - 1)) {
    heating = 0;
  }
  if (heating == 1) {
    digitalWrite(heating1, LOW);  //LOW = heating on
  }
  if (heating == 0) {
    digitalWrite(heating1, HIGH);  //HIGH = heating off
  }
} // loop
