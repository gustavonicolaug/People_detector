// Include

#include <SPI.h>
#include <SD.h>
#include <Mouse.h>
#include <Ultrasonic.h>

// Define 

#define pino_trigger 4
#define pino_echo 5

// Initializations

Ultrasonic ultrasonic(pino_trigger, pino_echo);
const int chipSelect = 10;

// Setup 

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
/* SD card initialization:
 * 
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
*/
  Serial.println("Lendo dados do sensor...");
}
////////////////////////////////////////////////////////////////
void loop() {
  // Variables
  unsigned int      n_stayed = 0;       // number of people who stayed in front of sensor
  unsigned int      n_passed = 0;       // number of people who passed in front of sensor
  unsigned int      obstruction_time;   // time that a person stayed in front of sensor
  unsigned int      times_array[100];   // array of times of people who stayed in front of sensor
  unsigned int      times_offset = 0;   // offset of times_array
  float             sensor;             // sensor read in centimeters
  long              microsec;           // sensor temporization

  microsec = ultrasonic.timing();
  sensor = ultrasonic.convert(microsec, Ultrasonic::CM);//read sensor in cm.

  /*# TO DO: function to send data every 24 hours.
  if(MUDOU DE DIA){
    ENVIA DADOS VIA SMS;
    RESETA CONTADORES;  
  }
  */
  // Detect obstructions from less than 1m
  // 
  if(sensor < 100){
    obstruction_time = 1; 

    // Count time of obstruction in seconds.
    //
    while(sensor<100){
      delay(1000);
      microsec = ultrasonic.timing();
      sensor = ultrasonic.convert(microsec, Ultrasonic::CM);
      //Serial.print(sensor);
      obstruction_time++;
    }

    // Decide if person stayed or just passed through sensor based on obstruction_time.
    //
    if(obstruction_time < 3){
      // Person passed through sensor
      //
      n_passed++;   
      //Serial.print(" Uma pessoa passou por aqui!\n"); 
    }else{
      // Person stayed for a while in front of sensor
      //
      n_stayed++;
      /*Serial.print(" Uma pessoa ficou aqui por: ");
      Serial.print(obstruction_time);
      Serial.print(" segundos\n");*/
      times_array[times_offset] = obstruction_time;
      times_offset++;
      if(times_offset == 100){
        // Array is full! Must send data before an other detection.
        times_offset = 0;
        //#TO DO: Send data every time vector is full  
      }
    } 
  }
  
  delay(500);// check new obstruction

}