// Include

#include <SPI.h>
#include <Mouse.h>
#include <Ultrasonic.h>
#include <SoftwareSerial.h>

// Define 

#define pino_trigger 4
#define pino_echo 5
#define array_size 25
#define limiar_time 3

// Initializations

Ultrasonic ultrasonic(pino_trigger, pino_echo);
SoftwareSerial SIM900(7, 8);

// Global variables

  unsigned int      n_stayed = 0;               // number of people who stayed in front of sensor
  unsigned int      n_passed = 0;               // number of people who passed in front of sensor
  unsigned int      times_array[array_size];    // array of times of people who stayed in front of sensor
  unsigned int      times_offset = 0;           // offset of times_array
  String            current_date;               // current date "YY/MM/DD,HH:MM:SS+TZ"
// Setup 

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  SIM900.begin(19200); // Arduino communicates with SIM900 GSM shield at a baud rate of 19200
  delay(20000); 
  current_date = get_date();
  
  //Serial.println("Lendo dados do sensor...");
}
////////////////////////////////////////////////////////////////
void loop() {
  // Variables
  unsigned int      obstruction_time;   // time that a person stayed in front of sensor
  float             sensor;             // sensor read in centimeters
  long              microsec;           // sensor temporization
  String            sms_message;
  unsigned int      i;

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
    if(obstruction_time < limiar_time){
      // Person passed through sensor
      //
      n_passed++;   
      //Serial.print(" Uma pessoa passou por aqui!\n"); 
    }else{
      // Person stayed for a while in front of sensor
      //
      n_stayed++;
      //Serial.print(" Uma pessoa ficou aqui por: ");
      //Serial.print(obstruction_time);
      //Serial.print(" segundos\n");
      times_array[times_offset] = obstruction_time;
      times_offset++;
      if(times_offset == array_size){
        // Array is full! Must send data before an other detection.
        times_offset = 0;
        sms_message = current_date;
        current_date = get_date();
        sms_message += "->"+ current_date;
        sms_message += "|F" + String(n_stayed);
        sms_message += "P" + String(n_passed);
        sms_message += ":";
        for( i = 0; i < array_size; i++ )
        {
          sms_message += String(times_array[i],HEX); // Send data in hexa to save space.
          if(i != (array_size - 1)){
            sms_message += ",";
          }  
        }
        sms_message += "*";
        //Serial.print(sms_message);
        sendSMS(sms_message);
        times_offset = 0;
        n_stayed = 0;
        n_passed = 0;
        
        //#TO DO: Send data every time vector is full  
      }
    } 
  }
  
  delay(500);// check new obstruction

}

String get_date()
{
  String            date;                       
  char              c;
  boolean           save_char = false;
  SIM900.print("AT+CLTS=1\r");
  delay(100);
  SIM900.print("AT+CCLK?\r");
  delay(100);
  save_char = false;
  while(SIM900.available())
  {
    c = SIM900.read();
    if(c == '\"')
    {
      save_char = !save_char;  
    }
    if((save_char == true)&&(c != '\"')){
      date += c;
    }   
    delay(100);
  }
  return date;
} 

void sendSMS(String s) {
  //Serial.print("Enviando SMS...\n");
  char trash; // clean available data
  // AT command to set SIM900 to SMS mode
  SIM900.print("AT+CMGF=1\r"); 
  delay(100);

  SIM900.println("AT + CMGS = \"+5511XXXXXXXXX\""); 
  delay(100);
  
  SIM900.println(s); 
  delay(100);

  // End AT command with a ^Z, ASCII code 26
  SIM900.println((char)26); 
  delay(100);
  SIM900.println();
  // Give module time to send SMS
  delay(5000); 
  while(SIM900.available())
  {
    trash = SIM900.read();
  }
  //Serial.print("Terminou de enviar!\n");
}