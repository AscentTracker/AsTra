/*

FIX
-i 10 setting for idle poweroff
-b 0 setting for mute
-9c+ grade?

20.8.2014

 TODO:
 -smaller analogDebounce
 -smaller getSettings
 -use tinyFAT, writeLn -> write (no long string buffer)?

Version history:

0.24
 -log current temperature and temp_altitude based on it (long ascents tend to end higher than they started; why?) 

0.23
 -auto shutoff after idle

1.01
 -voltage display at startup, grade 4+


*/


// Poweroff if idle after 10 minutes
#include <avr/sleep.h>


#include <ctype.h>
#include <SD.h>

#define SHOWFREEMEM 0
#define HWLCD 1
#define HWRTC 1
#define TEMPDEBUG 0

byte HWSD=1;

#define buttonPin A1
#define piezoPin 2
#define chipSelect 4


#include <Wire.h>
#include <MS561101BA.h>
#if HWRTC
#include <RTClib.h>
#endif

#if HWLCD
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
// LiquidCrystal LCD(RS, Enable, D4, D5, D6, D7)

LiquidCrystal LCD(9, 10, 5, 6, 7, 8);

#endif

#include <AnalogDebounce.h>
  AnalogDebounce Buttons(buttonPin,ButtonPush);  // Analog Input 0, 

#define B_UP 2
#define B_DOWN 3
#define B_START 1
#define B_MENU 4


#define S_IDLE 1
#define S_PRECLIMBING 2
#define S_CLIMBING 3
#define S_POSTCLIMBING 4
#define S_SHOW 5


// ?,4,4+,5a,5a+,...9c+
#define MAXGRADE 32

RTC_DS1307 RTC; // define the Real Time Clock object
DateTime now;


#define MOVAVG_SIZE 32

MS561101BA baro = MS561101BA();

float movavg_buff[MOVAVG_SIZE];
int movavg_i=0;

const float sea_press = 1013.25;
float press, temperature;

// Timestamps for time and date
char stime[9]; // add space for string terminators! 
char sdate[11];


// the logging file
File logfile;


// Filename template
char filename[] = "YYMMDD00.CSV";


float avg_altitude;
float raw_altitude;
float zero;
float lastaltitude;
// float rate;
float kalman_altitude=0.0;
float maxalt=0.0;

uint32_t mnow=0;
uint32_t lastprint=0;
uint32_t lastsync=0;
uint32_t starttime=0;
uint32_t endtime=0;
uint32_t start_stop_autotimer=0;
uint32_t zerotime=0;
uint32_t INTERVAL=250;
// auto poweroff after 10 minutes with no button activity
uint32_t lastbuttontime=0;
#define IDLEPOWEROFF 600000 

char str[6];
byte grade=3; // start with 5a
byte beep=2; // 0=silent, 1=startup & error, 2=all
byte mute=0; // set to 1 if button pressed at boot
byte failed=0; // failed to ascend to the top
byte autostart=1; // enable autostart/stop
byte button=0;
byte initloop=0;
byte state=S_IDLE;
byte displayvolts=0;

byte numclimbers=0;
char **climbername=NULL;
byte climber=0;


/***************************************************************************************/
/* sensitivity / data filtering / n filter parameters                             */
/* The Kalman Filter is being used to remove noise from the MS5611 sensor data. It can */
/* be adjusted by changing the value Q (process Noise) and R (Sensor Noise).           */
/* Best pactise is to only change R, as both parametrers influence each other.         */
/* (less Q requires more R to compensate and vica versa)                               */
/***************************************************************************************/
#define KALMAN_Q 0.05 // 0.05 do not tamper with this value unless you are looking for adventure ;-)
#define KALMAN_R 80  // default:300   change this value if you want to adjust the default sensitivity!
                      // this will only be used if PIN_PPM is NOT defined
                      //  50 = fast but lot of errors (good for sensor testing...but quite a lot of noise)
                      // 300 = medium speed and less false indications (nice medium setting with 
                      //       a silence window (indoor)of -0.2 to +0.2)
                      // 1000 = conservative setting ;-)
                      // 500 = still useable maybe for a slow flyer?
                      // 1000 and q=0.001 is it moving at all?? in stable airpressure you can measure 
                      //      the height of your furniture with this setting. but do not use it for flying ;-)
                      // .. everything inbetween in relation to these values.

float kalman_q= KALMAN_Q; //process noise covariance
float kalman_r= KALMAN_R; //measurement noise covariance

// Kalman filter variables/parameters
float p=100; //estimation error covariance
float k=0; //kalman gain

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

#define S(a) Serial.print(a);
// As of IDE 1.1 no more space for serial debugging, pity
#define S(a) ;
#define L(a) S(a); logfile.print(a);


#include <avr/pgmspace.h>

// http://forum.arduino.cc/index.php?topic=110307.0

#define FS(x) (__FlashStringHelper*)(x)
#define pm PROGMEM

const char MyName[]        pm ={"AsTra V: "};
const char Version[]       pm ={"1.01"};
const char Battery[]       pm ={"Battery: "};
const char Spaces3[]       pm ={"   "};
const char Spaces4[]       pm ={"    "};
const char SpaceMSpace[]   pm ={" m "};
const char SpaceS[]        pm ={" s"};
const char sError[]        pm ={"Error: "};
const char mallocFailed[]  pm ={"malloc failed"};
const char autoPoweroff[]  pm ={"Idle -> Poweroff"};

// LCD positions Row 1 and 2, Left, Middle and Right columns
#define LCD_R1L 0,0
#define LCD_R1M 4,0
#define LCD_R1R 8,0
#define LCD_R2L 0,1
#define LCD_R2R 8,1

#define FR1 4000
#define BATTERYWARN 2.0 // still to be verified


float supplyvoltage=0.0;

void getVoltage () {        
   supplyvoltage = analogRead(A0);
   delay(10);
   supplyvoltage = analogRead(A0)*(5.0 / 1023);
}


// -------------------------------------------

void setup() {
  Serial.begin(9600);

  // mute if a button pressed while boot
  if (analogRead(buttonPin)>10) {
    mute=1;
    beep=0;    
  };

  pinMode(piezoPin,OUTPUT);

  // say hello
  buzz(piezoPin, 200, 100, 0);
    
 
  // Initialize the SD card  
  if (HWSD) {
    if (!SD.begin(chipSelect)) {
      HWSD=0;
    } else {
      getSettings();
    };
  };

  if (mute) beep=0;

  // No climbers in settings file, let Tarzan loose!
  if (numclimbers==0) {
    numclimbers=1;
    climbername = (char **)malloc (sizeof (char *) * numclimbers);
    if (!climbername) {
      error(mallocFailed);
    }
    climbername[0]=strdup("Tarzan");  
  }; 
  
  // Start with first climber  
  climber=0;  
  
  // get power supply voltage
  getVoltage();

#if HWLCD
  // set up the LCD's number of columns and rows: 
  LCD.begin(16, 2);
  
  // Print a message to the LCD.  
  LCD.setCursor(LCD_R1L);
  LCD.print(FS(MyName));
  // LCD.setCursor(LCD_R1R);
  // LCD.print(F(" "));
  LCD.print(FS(Version));
  // LCD.setCursor(LCD_R2R);
  // LCD.print(F(__TIME__));
  LCD.setCursor(LCD_R2L);
  LCD.print(FS(Battery));
  LCD.print(supplyvoltage);
  LCD.print(F(" V "));
#endif

  // Connect to RTC
  Wire.begin();
  
  if (!RTC.begin()) {
    error("RTC");
  }
 
  // Init MS5611 barometric sensor
  baro.init(MS561101BA_ADDR_CSB_LOW); 
  delay(100);
 
} // end of setup


float temp_altitude;
float temp0;

// =====================================================================

void loop() {

  // check buttons first for best responsiveness
  Buttons.loopCheck();    

  // Must check for invalid values (NULL)    
  temperature=NULL; 
  while(temperature == NULL) {
    temperature = baro.getTemperature(MS561101BA_OSR_4096);
    // delay(20);
  }
  
  press=NULL;
  while (press == NULL) {
    press = baro.getPressure(MS561101BA_OSR_4096);
    // delay(20);
  };
  
  raw_altitude=getAltitude(press, temp0);

  // Update moving average
  pushAvg(press);
  press = getAvg(movavg_buff, MOVAVG_SIZE);
  avg_altitude=getAltitude(press, temp0);
  temp_altitude=getAltitude(press, temperature);
    
  // Update the prediction value
  p = p + kalman_q;

  // Update based on measurement
  k = p / (p + kalman_r);
  kalman_altitude = kalman_altitude + k * (raw_altitude - kalman_altitude);
  p = (1 - k) * p;

  // Get power supply voltage
  getVoltage();

// ============================================
 // Sensor init & avg buffer fillup delay 
// ============================================
    
 if (initloop < MOVAVG_SIZE) {
   temp0=temperature;
   initloop++;
   if (button) {
    mute=1;
    beep=0;
   }
   /* 
   LCD.setCursor(LCD_R2L);
   LCD.print(FS(Battery));
   LCD.print(supplyvoltage);
   LCD.print(F(" V "));
   */
   // LCD.write(255);
   delay(40);
   return;
 } else if (initloop == MOVAVG_SIZE) { // end of init looping
   initloop++;
    
#if HWLCD
    LCD.clear();
    LCD.setCursor(LCD_R1L);
    LCD.print(gradenum2name(grade,str));
    LCD.setCursor(LCD_R1R);
    LCD.print(climbername[climber]);
#endif

   return;
 };


    // millis() now  
    mnow=millis();

    // Fetch the time
    now = RTC.now();

    // Timestamps
    sprintf(stime, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
    sprintf(sdate, "%d-%02d-%02d",   now.year(), now.month(),  now.day());
 
// ============================================
//   BUTTONS
// ============================================

 switch (button) {  
   case B_START:
     buzz(piezoPin, FR1, 100, 100);
     switch (state) {
       case S_IDLE:     setZero(); state=S_PRECLIMBING; break;
       case S_CLIMBING: state=S_POSTCLIMBING; break;
       case S_SHOW:     setZero(); state=S_IDLE; break;
     };
     break;
   case B_MENU: // mark ascent aborted
     switch (state) {
       case S_CLIMBING: failed=true; button=0; state=S_POSTCLIMBING; break;
     };
     break;
 };
 
if (state==S_IDLE) {
 switch (button) {     
   case B_UP:
     if (grade<MAXGRADE) grade++;
#if HWLCD
    LCD.setCursor(LCD_R1L);
    LCD.print(gradenum2name(grade,str));
    LCD.print(FS(Spaces4));
#endif    
     break;

   case B_DOWN:
     if (grade>0) grade--;
#if HWLCD
    LCD.setCursor(LCD_R1L);
    LCD.print(gradenum2name(grade,str));
    LCD.print(FS(Spaces4));
#endif    
     break;

   case B_MENU:
     climber++;
     if (climber>=numclimbers) climber=0;
#if HWLCD
    LCD.setCursor(LCD_R1R);
    LCD.print(climbername[climber]);
    LCD.print(FS(Spaces4));
#endif         
     break;
  
 }; // end switch button
}; 

// -----------------------------------------------

switch (state) {
  case S_IDLE:
  
#if AUTOSTART  
    // start when above X meters for y seconds
    if (kalman_altitude-zero > 0.7) {
      if (start_stop_autotimer==0) {
        start_stop_autotimer=mnow;
      } else if (mnow-start_stop_autotimer > 2000) {
        // if (autostart) state=S_PRECLIMBING;
      };
    } else {
      start_stop_autotimer=0;
    };
#endif

   // set zero level when idle and one of these:
   //  -some button is pressed
   //  -10 seconds after boot
   //  -10 seconds after last re-set when after init and not waiting to climb over autostart level
    // if (((start_stop_autotimer==0) && (mnow > 10000)) && (button || (mnow < 10000) || (mnow-zerotime>10000))) {
    if ((button || (mnow < 10000) || 
          ((mnow > 10000)  && (mnow-zerotime>30000) && (start_stop_autotimer==0)))) {
     setZero();
     zerotime=mnow;
    };
    
    if (mnow-lastbuttontime > IDLEPOWEROFF) {
      buzz(piezoPin, 200, 100, 0);

      LCD.clear();
      LCD.setCursor(LCD_R1L);
      LCD.print(FS(autoPoweroff));
  
      set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
      sleep_enable();          // enables the sleep bit in the mcucr register
      sleep_mode();            // here the device is actually put to sleep!!
    };
    break;
    
  case S_CLIMBING:
#if AUTOSTART
    // stop when under X meters for z seconds
    if (kalman_altitude-zero < 1.0) {
      if (start_stop_autotimer==0) {
        start_stop_autotimer=mnow;
      } else if (mnow-start_stop_autotimer > 120000) {
        if (autostart) state=S_POSTCLIMBING;
      };
    } else {
      start_stop_autotimer=0;
    };    
#endif    
  break;
  
  case S_PRECLIMBING: // start climbing
    if (HWSD) {
      openLog(now.year(), now.month(), now.day());
      if (! logfile) {
        error("Create file");
      }
     };

    L(F("{ "));
    L(F("\"version\":\""));
    L(F("1.2"));
    L(F("\", \"file\":\""));
    L(filename);
    L(F("\", \"date\":\""));
    L(sdate);
    L(F("\", \"time\":\""));
    L(stime);
    L(F("\", \"epoch\":\""));
    L(now.unixtime());
    L(F("\", \"climber\":\""));
    L(climbername[climber]);
    L(F("\", \"grade\":\""));
    L(gradenum2name(grade,str));
    L(F("\", \"voltage\":\""));
    L(supplyvoltage);   
    L(F("\", \"zero\":\""));
    L(zero);
    L(F("\", \"temp\":\""));
    L(temp0);
    L(F("\" }\n"));
  
    starttime=endtime=lastsync=mnow;
    failed=false;
    state=S_CLIMBING;
    
     buzz(piezoPin, FR1, 100, 50);
     buzz(piezoPin, FR1, 100, 50);
     buzz(piezoPin, FR1, 100, 50);
    
#if HWLCD
    LCD.clear();
    LCD.setCursor(LCD_R1L);
    LCD.print(gradenum2name(grade,str));
    LCD.setCursor(LCD_R1R);
    LCD.print(climbername[climber]);
#endif
  break;
  
 case S_POSTCLIMBING:
    if (HWSD) {
       if (failed) {
         L("#FAILED\n");
       };
       logfile.flush();
       logfile.close();
    };
    buzz(piezoPin, FR1, 1000, 50);
    endtime=mnow;    
    state=S_SHOW;
    break;
  
}; // switch state

  if (button) {
    lastbuttontime=mnow;
    button=0; // debounce does not work? :)
  };

// ============================================
//   DISPLAY
// ============================================

  // update display only every INTERVAL  
  if (((mnow-lastprint)>INTERVAL)) {

#if TEMPDEBUG
    temp0_altitude=getAltitude(press, temp0);
    S(raw_altitude-zero);
    S(',');
    S(temp0_altitude-zero);
    S(',');
    S(avg_altitude-zero);
    S(',');
    S(kalman_altitude-zero);
    S('\n');
#endif    


    // Update display
#if HWLCD     
    if (state == S_CLIMBING) { // show altitude and elapsed seconds
      LCD.setCursor(LCD_R1M);
      if (HWSD) {
        LCD.print(F("GO!"));
      } else {
        LCD.print(F("!SD"));
      };
      LCD.setCursor(LCD_R2L);    
      LCD.print(kalman_altitude-zero,1);
      LCD.print(FS(SpaceMSpace));
      LCD.print(FS(Spaces4));
      // LCD.print(F("    "));
      LCD.setCursor(LCD_R2R);      
      LCD.print((mnow-starttime)/1000);
      LCD.print(FS(SpaceS));
    } else if (state == S_SHOW) { // show results
      LCD.setCursor(LCD_R1M);
      LCD.print(F("END"));
      LCD.setCursor(LCD_R2L);
      LCD.print(maxalt-zero,1);
      LCD.print(FS(SpaceMSpace));
      LCD.print(FS(Spaces4));
      LCD.setCursor(LCD_R2R);      
      LCD.print((endtime-starttime)/1000);
      LCD.print(FS(SpaceS));
      LCD.print(FS(Spaces4));

    } else { // state==IDLE show battery and time
      LCD.setCursor(LCD_R1M);
      LCD.print(FS(Spaces3));
      LCD.setCursor(LCD_R2L);
      if (supplyvoltage < BATTERYWARN) { // battery warning
        LCD.print('!');
        LCD.print(' ');
        displayvolts=1;
      };
      if (displayvolts) {
       LCD.print(supplyvoltage,1);
       LCD.print('V');
       LCD.print(' ');
      } else {
       LCD.print(kalman_altitude-zero,1);
       LCD.print(F(" m "));
      };
      LCD.setCursor(LCD_R2R);      
      LCD.print(stime);
    };
#endif 

// ============================================
// UPDATE LOG DATA
// ============================================

  // Update log and/or serial console
  if (state==S_CLIMBING) {
    L(mnow-starttime);
    L(',');
    L(raw_altitude-zero);
    L(',');
    L(avg_altitude-zero);
    L(',');
    L(kalman_altitude-zero);
    L(',');
    L(supplyvoltage);   
    L(',');
    L(temp_altitude-zero);
    L(',');
    L(temperature);
    L('\n');

    // sync every 15 seconds
    if (mnow-lastsync > 15000) {
      logfile.flush();
      lastsync=mnow;
 /*      
#if HWLCD
      LCD.setCursor(LCD_R2L);
      LCD.print("                ");
      LCD.setCursor(LCD_R1L);
      LCD.print(supplyvoltage);
      LCD.setCursor(5,1);
      LCD.print("SYNC ");
      LCD.print(maxalt-zero);
      delay(1000);
      LCD.setCursor(LCD_R1L);
      LCD.print("                ");
      // LCD.clear();
  #endif
 */  
    }; // sync
  }; // climbing
    
   lastprint=mnow;
   // lastaltitude=avg_altitude;
   maxalt=max(maxalt,kalman_altitude);
    
  };
  delay(20);
}


// ============================================
// -----------------------------------------------------


float getAltitude(float press, float temp) {
  //return (1.0f - pow(press/101325.0f, 0.190295f)) * 4433000.0f;
  return ((pow((sea_press / press), 1/5.257) - 1.0) * (temp + 273.15)) / 0.0065;
}

void pushAvg(float val) {
  movavg_buff[movavg_i] = val;
  movavg_i = (movavg_i + 1) % MOVAVG_SIZE;
}

float getAvg(float * buff, int size) {
  float sum = 0.0;
  for(int i=0; i<size; i++) {
    sum += buff[i];
  }
  return sum / size;
}

/*
*
* gradenum: 0=?, 1=4, 2=5a, 3=5a+, ...
* *buff=return value
*
*/

char *gradenum2name(byte gradenum, char *buff) {
   
    char append[2];
  
    if (gradenum<1) {
       strcpy(buff,"?");
       return buff;
    } 
    gradenum--;
    if (gradenum<1) {
       strcpy(buff,"4");
       return buff;
    } 
    gradenum--;
    if (gradenum<1) {
       strcpy(buff,"4+");
       return buff;
    } 
    gradenum--;
   
    byte grade=gradenum/6;
    byte subgrade=gradenum%6;
    byte letter=subgrade/2;
    byte plus=subgrade%2;
    
    itoa(5+grade,buff,10);
    append[0]=char(97+letter);
    append[1]='\0';
    strcat(buff,append);    
    append[0]=plus?'+':'\0';
    strcat(buff,append);
    
    // sprintf(buff,"%d%c%c",5+grade,char(97+letter),plus?'+':' ');
  
    return buff;
    
};


// --------------------

void error(const char *str)
{

#if HWLCD
  LCD.clear();
  LCD.setCursor(LCD_R1L);
  LCD.print(FS(sError));
  LCD.setCursor(LCD_R2L);
  LCD.print(str);
#endif


  while(1) {
    // buzz(piezoPin, 1200, 200, 2000);
    Serial.print(FS(sError));
    Serial.println(str);
    delay(3000);
  };
}


#if SHOWFREEMEM

// ----- Free RAM http://arduino.cc/playground/Code/AvailableMemory

int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

#endif


/*

//---------- Floating point value with one rounded decimal

char *f3d1tostr( float f, char *b)
{
  long int t = (f + 0.05) * 100; // round up 1st decimal up, convert to integer: 3.16 -> 321
  t=t/10; // chop off last digit: 32
  // always space for sign
  sprintf(b,"%c%2i.%i",(t < 0 ? '-' : ' '), (int)abs(t/10), (int)abs(t % 10));
  return b;
};

*/


// buzz - smaller implementation of tone()
//  http://www.faludi.com/examples/helpful-arduino-functions/

void buzz(int targetPin, long frequency, long length, int delayafter) {
  // return;
  if (!beep) return;
  // if (targetPin<0) targetPin=-targetPin;
  long delayValue = 1000000/frequency/2;
  // calculate the delay value between transitions
  //// 1 second's worth of microseconds, divided by the frequency,
  //// then split in half since there are two phases to each cycle

  long numCycles = frequency * length/ 1000;
  // calculate the number of cycles for proper timing
  //// multiply frequency, which is really cycles per second, by the
  //// number of seconds to get the total number of cycles to produce
  for (long i=0; i < numCycles; i++){ // for the calculated length of time...
    digitalWrite(targetPin,HIGH); // write the buzzer pin high to push out the diaphram
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(targetPin,LOW); // write the buzzer pin low to pull back the diaphram
    delayMicroseconds(delayValue); // wait again for the calculated delay value
  }

  // SS extension
  if (delayafter) delay(delayafter);
}

// ============================================================================

// Adopted from http://jurgen.gaeremyn.be/index.php/arduino/reading-configuration-file-from-an-sd-card.html 
// but using char arrays, not Strings (less memory waste for such a simple task)
// 
void getSettings()
{
 // Open the settings file for reading:
  File myFile = SD.open("SETTINGS.TXT");
  char character;
  char description = ' ';
  char value[15];
  byte i=0;
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      character = myFile.read();
      if (character == '#')         {
               // Comment - ignore this line
               while(character != '\n'){
                 character = myFile.read();
               };
      } else if (isalnum(character))      {  // Get a character as the description
        description=character;
      } else if (character ==' ')         {  // start checking the value for possible results
        character = myFile.read();
        
        switch (description) {
          case 'b':
            beep=character=-'0';
            break;
          case 'a':
            autostart=character=-'0';
            break;
          case 'n':
          case 'c':
          case 'k':
           i=0;
           do {
             value[i++]=character;
             character = myFile.read();
           } while(character != '\n');
           value[i++]='\0';           
           switch (description) {
             case 'n': numclimbers=atoi(value); break;
             case 'k': kalman_r=(float)atoi(value); break;
             case 'c':             
              if (numclimbers==0) numclimbers=1;
              if (climbername == NULL) {
                  climbername = (char **)malloc (sizeof (char *) * numclimbers);
                  if (!climbername) {
                    error(mallocFailed);
                  }
              };
              climbername[climber]=strdup(value);
              if (climber<numclimbers) climber++;
           };
           break;

          default:
            while(character != '\n')
              character = myFile.read();         
        };
        
        description = ' ';
      } else {
        // Ignore this character (could be space, tab, newline, carriage return or something else)
      }
    }
    // close the file:
    myFile.close();
}


// ----------------------------------------------------

// Open log file --------------------

boolean openLog(int year, byte month, byte day)
{
  // static to rembember last filename between climbs; scannning SD card is s-l-o-w
  static byte c1 = 'A';
  static byte c2 = 'A';
  
  boolean found=false;
 // create a new file with unique name YYMMDD{AA..ZZ}
 for (; c1 <= 'Z'; c1++) {
  for (; c2 <= 'Z'; c2++) {
    sprintf(filename,"%02d%02d%02d%c%c.CSV", year-2000, month, day, c1,c2);
  // Serial.println(filename);
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      if (HWSD) {
        logfile = SD.open(filename, FILE_WRITE);
      } else {
        logfile=File();
      };
      found=true;
      break; // leave both loops
    };
   };
   if (found) break;
   c2='A';
  }

  return logfile;

};


// =====================================================================

// https://www.sparkfun.com/datasheets/LCD/HD44780.pdf

void setZero(void) {
      zero=maxalt=lastaltitude=kalman_altitude; // avg_altitude
      LCD.setCursor(LCD_R1M);
      LCD.print(' ');
      LCD.write(219); // 219 = square, 198=two lines, 95=_
}; 
  
// =====================================================================

// Callback function
void ButtonPush(byte Button) {
  button=Button;
}

