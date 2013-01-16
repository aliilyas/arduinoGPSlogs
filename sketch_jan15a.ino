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
/******************************************************/

/*****************  SETUP  ****************************/
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

  logMessage("Initializing SD card...");
  if (!SD.begin(10)) {
    logMessage("initialization failed!");
    return;
  }

  logMessage("initialization done.");
}
/**********************************************************/




/*************       LOOP   *******************************/
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
/***************************************************************/
//testtesttest

void logMessage(String s){
  Serial.println(s);
}

void composeAndOutput(String input){
  String output = "";
  String timestamp =  splitString(input,1);
  String date =  splitString(input,9);
  String longitude = splitString(input,5);
  String latitude = splitString(input,3);
  String gDirection = splitString(input,4)+"/"+splitString(input,6);
  String speedInKnots = splitString(input,7);
  String course = splitString(input,8);
  String variation = splitString(input,10);
  String delimiter = " | ";

  output = timestamp + ":"+date + delimiter + " Long : " + longitude +delimiter + " Lat : " +latitude +delimiter+ " Speed : " +speedInKnots; 

  myFile = SD.open("newa.txt", FILE_WRITE);
  myFile.println(output);
  myFile.close();
}

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



template<typename Data>
class Vector {
  size_t d_size; // Stores no. of actually stored objects
  size_t d_capacity; // Stores allocated capacity
  Data *d_data; // Stores data
public:
  Vector() : 
  d_size(0), d_capacity(0), d_data(0) {
  }; // Default constructor
  Vector(Vector const &other) : 
  d_size(other.d_size), d_capacity(other.d_capacity), d_data(0) {
    d_data = (Data *)malloc(d_capacity*sizeof(Data)); 
    memcpy(d_data, other.d_data, d_size*sizeof(Data)); 
  }; // Copy constuctor
  ~Vector() { 
    free(d_data); 
  }; // Destructor
  Vector &operator=(Vector const &other) { 
    free(d_data); 
    d_size = other.d_size; 
    d_capacity = other.d_capacity; 
    d_data = (Data *)malloc(d_capacity*sizeof(Data)); 
    memcpy(d_data, other.d_data, d_size*sizeof(Data)); 
    return *this; 
  }; // Needed for memory management
  void push_back(Data const &x) { 
    if (d_capacity == d_size) resize(); 
    d_data[d_size++] = x; 
  }; // Adds new value. If needed, allocates more space
  size_t size() const { 
    return d_size; 
  }; // Size getter
  Data const &operator[](size_t idx) const { 
    return d_data[idx]; 
  }; // Const getter
  Data &operator[](size_t idx) { 
    return d_data[idx]; 
  }; // Changeable getter
private:
  void resize() { 
    d_capacity = d_capacity ? d_capacity*2 : 1; 
    Data *newdata = (Data *)malloc(d_capacity*sizeof(Data)); 
    memcpy(newdata, d_data, d_size * sizeof(Data)); 
    free(d_data); 
    d_data = newdata; 
  };// Allocates double the old space
};


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

