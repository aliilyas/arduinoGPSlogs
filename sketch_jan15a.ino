#include <Stepper.h>
#include <SD.h>
#include <SoftwareSerial.h>

#define PMTK_SET_NMEA_UPDATE_1HZ     "$PMTK220,1000*1F"
#define PMTK_SET_NMEA_UPDATE_5HZ     "$PMTK220,200*2C"
#define PMTK_SET_NMEA_UPDATE_10HZ    "$PMTK220,100*2F"
#define PMTK_SET_NMEA_OUTPUT_RMCONLY "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"
#define PMTK_SET_NMEA_OUTPUT_ALLDATA "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28"

#define rxPin 2 // listen SD card
#define txPin 3 // echo SD card

/*****************  VARIABLES  ***********************/
SoftwareSerial gpsSerial(rxPin, txPin);// GPS serial 
File myFile;// data file on SD card
int ledPin = 8; // led (info only)
String gpsMessage = ""; 



void setup()
{

  /*LED*/
  pinMode(ledPin, OUTPUT); 
  digitalWrite(ledPin, HIGH);
  /*END LED*/

  /*SD*/
  pinMode(10, OUTPUT);
  /*END SD*/

  // SD serial line
  Serial.begin(57600);
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);

  // gps serial line
  gpsSerial.begin(9600);
  gpsSerial.println(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  gpsSerial.println(PMTK_SET_NMEA_UPDATE_1HZ);

  // Initialize SD card
  
  if (!SD.begin(10)) {
    logMessage("SD card initialization failed!");
    return;
  }
  logMessage("SD card initialization done.");
  // TODO make this strigs on LCD
}






void loop()
{
  int character;

  if (gpsSerial.available()) {
    character = (int)gpsSerial.read();// reads one by one char of all message from gps


    gpsMessage+=String((char)character);
    if(((int)character)==10){

      //logMessage(gpsMessage);
      String validity = splitString(gpsMessage,2);
      if(validity.equals("V")){// check if message is valid
        // change to "A", i cant receive gps in my workplace
        composeAndOutput(gpsMessage);
      }
      gpsMessage="";
    }
  }
  if (Serial.available()) {
    gpsSerial.print((char)Serial.read());// feed gps
  }

}


/*
 * Log message to Serial 
 */
void logMessage(String s){
  Serial.println(s);
}



/***********  NMEA message  *****************************************/

//$GPRMC[0]
//      [1]   220516     Time Stamp
//      [2]   A          validity - A-ok, V-invalid
//      [3]   5133.82    current Latitude
//      [4]   N          North/South
//      [5]   00042.24   current Longitude
//      [6]   W          East/West
//      [7]   173.8      Speed in knots
//      [8]   231.8      True course
//      [9]   130694     Date Stamp
//      [10]  004.2      Variation
//      [11]  W          East/West
//      [12]  *70        checksum

/*
 * Method creates log and writes this log into SD card 
 */
void composeAndOutput(String input){
  String output = "";
  String timestamp =  splitString(input,1);
  String date =  splitString(input,9);
  String longitude = splitString(input,5);
  String latitude = splitString(input,3);
  String gDirection = splitString(input,4)+"/"+splitString(input,6);
  String knots = splitString(input,7);
  String course = splitString(input,8);
  String variation = splitString(input,10);
  String delimiter = " | ";

  output = timestamp + ":"+date + delimiter + " Long : " + longitude +delimiter + " Lat : " +latitude +delimiter+ " Speed : " + knots; 

  myFile = SD.open("newa.txt", FILE_WRITE);
  myFile.println(output);
  myFile.close();
}

/*
 * Split string and get item in specified index
 */
String splitString(String s, int index){
  String rs='\0';
  int parserIndex = index;
  int parserCnt=0;
  int rFromIndex=0, rToIndex=-1;

  while(index>=parserCnt){
    rFromIndex = rToIndex+1;
    rToIndex = s.indexOf(',',rFromIndex);

    if(index == parserCnt){
      if(rToIndex == 0 || rToIndex == -1){
        return '\0';
      }
      return s.substring(rFromIndex,rToIndex);
    }
    else{
      parserCnt++;
    }

  }
  return rs;
}


/*
 * Converts knots to kilometers
 */
float knotToKilometers(float knot){
  return knot*1.852;
}




