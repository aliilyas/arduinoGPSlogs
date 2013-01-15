#include <SD.h>
#include <SoftwareSerial.h>

#define PMTK_SET_NMEA_UPDATE_1HZ     "$PMTK220,1000*1F"
#define PMTK_SET_NMEA_UPDATE_5HZ     "$PMTK220,200*2C"
#define PMTK_SET_NMEA_UPDATE_10HZ    "$PMTK220,100*2F"
#define PMTK_SET_NMEA_OUTPUT_RMCONLY "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"
#define PMTK_SET_NMEA_OUTPUT_ALLDATA "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28"

#define rxPin 2
#define txPin 3

SoftwareSerial mySerial(rxPin, txPin);
File myFile;

int ledPin = 8; 
void setup()
{

  /*LED*/
  pinMode(ledPin, OUTPUT); 
  digitalWrite(ledPin, HIGH);
  /*END LED*/

  /*SD*/
  pinMode(10, OUTPUT);
  /*END SD*/

  Serial.begin(57600);
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  mySerial.begin(9600);
  mySerial.println(PMTK_SET_NMEA_OUTPUT_ALLDATA);
  mySerial.println(PMTK_SET_NMEA_UPDATE_1HZ);

  Serial.println("Initializing SD card...");
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    return;
  }

  Serial.println("initialization done.");

  Serial.println("File created.");
}
int i = 0;
String stringOne = "";  
char below;
void loop()
{
  // Serial.println("inside loop.");
  int c;
  if (mySerial.available()) {
    c = (int)mySerial.read();

    //    Serial.print((char)c);
    stringOne+=String((char)c);
    if((char)c=='\n'){
      //myFile = SD.open("tesa.txt", FILE_WRITE);
      //myFile.println(stringOne);
      //myFile.close();
      Serial.print(stringOne);
      stringOne="";
    }
  }
  if (Serial.available()) {
    mySerial.print((char)Serial.read());
  }


  // stringOne+=qwe;
  // Serial.println(stringOne);

  // if (myFile) {

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

