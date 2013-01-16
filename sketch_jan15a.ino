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
      myFile = SD.open("1.txt", FILE_WRITE);
      myFile.println(gpsMessage);
      myFile.close();
      logMessage(gpsMessage);
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


