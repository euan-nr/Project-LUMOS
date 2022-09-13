#include <Adafruit_LSM303DLH_Mag.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_NeoPixel.h>

int led = 7;

//#define PIN 8
#define PIN 6 //external LED
// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 60
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(1, 8, NEO_GRB + NEO_KHZ800);

/*TODO: Clean up code, add neopixel changes as well as LED support, fine tune ranges*/

const long intervalLow = 10;
const long intervalMed = 1000;
const long intervalHigh = 10000;
unsigned long previousMillis = 0; 
   
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);


// Variables will change:
int ledState = LOW;             // ledState used to set the LED
const long interval = 1000;           // interval at which to blink (milliseconds)

/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetailsLux(void)
{
  sensor_t sensor;
  tsl.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" lux");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" lux");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" lux");  
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void displaySensorDetailsAcc(void)
{
  sensor_t sensor;
  accel.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" m/s^2");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" m/s^2");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" m/s^2");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(100);
}


/**************************************************************************/
/*
    Configures the gain and integration time for the TSL2561
*/
/**************************************************************************/
void configureSensorLux(void)
{
  /* You can also manually set the gain or enable auto-gain support */
  // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  // tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
  
  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  /* Update these values depending on what you've set above! */  
  Serial.println("------------------------------------");
  Serial.print  ("Gain:         "); Serial.println("Auto");
  Serial.print  ("Timing:       "); Serial.println("13 ms");
  Serial.println("------------------------------------");
}

void setup(void) 
{
  // initialize the digital pin as an output (Light sensor)
  pinMode(led, OUTPUT);  
  Serial.begin(9600);
  Serial.println("Light Sensor Test"); Serial.println("");
  
  /* Initialise the sensor */
  //use tsl.begin() to default to Wire, 
  //tsl.begin(&Wire2) directs api to use Wire2, etc.
  if(!tsl.begin())
  {
    /* There was a problem detecting the TSL2561 ... check your connections */
    Serial.print("No light sensor detected!");
    while(1);
  }
  displaySensorDetailsLux();
  configureSensorLux();
  Serial.println("");
  #ifndef ESP8266
  while (!Serial);     // will pause Zero, Leonardo, etc until serial console opens
  #endif
  Serial.begin(9600);
  Serial.println("Accelerometer Test"); Serial.println("");
  if(!accel.begin())
  {
    Serial.println("No accelerometer detected!");
    while(1);
  }

  displaySensorDetailsAcc();
  
  pinMode(led, OUTPUT);  /*initialise LED*/
  Serial.begin(9600);
  Serial.println("Light Sensor Test"); Serial.println("");
  
  strip.begin(); /*Neopixel*/
  strip.setBrightness(255);
  strip.show(); // Initialize all pixels to 'off'
  strip1.begin(); /*Neopixel*/
  strip1.setBrightness(50);
  strip1.show(); // Initialize all pixels to 'off'
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
      strip1.setPixelColor(i, c);
      strip1.show();
      delay(wait);
  }
}

void loop(void) 
{
  unsigned long currentMillis = millis();
  /* Get a new sensor event */ 
  sensors_event_t event,event2;
  tsl.getEvent(&event);
  accel.getEvent(&event2);

  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("X: "); Serial.print(event2.acceleration.x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(event2.acceleration.y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(event2.acceleration.z); Serial.print("  ");Serial.println("m/s^2 ");

  /* Note: You can also get the raw (non unified values) for */
  /* the last data sample as follows. The .getEvent call populates */
  /* the raw values used below. */
  //Serial.print("X Raw: "); Serial.print(accel.raw.x); Serial.print("  ");
  //Serial.print("Y Raw: "); Serial.print(accel.raw.y); Serial.print("  ");
  //Serial.print("Z Raw: "); Serial.print(accel.raw.z); Serial.println("");

  double storedVector = event2.acceleration.x*event2.acceleration.x;
  storedVector += event2.acceleration.y*event2.acceleration.y;
  storedVector += event2.acceleration.z*event2.acceleration.z;
  storedVector = sqrt(storedVector);
  Serial.print("Len: "); Serial.println(storedVector);

  /* Delay before the next sample */
  delay(100);

  accel.getEvent(&event2);
  double newVector = event2.acceleration.x*event2.acceleration.x;
  newVector += event2.acceleration.y*event2.acceleration.y;
  newVector += event2.acceleration.z*event2.acceleration.z;
  newVector = sqrt(newVector);
  Serial.print("New Len: "); Serial.println(newVector);
  Serial.print("Acceleration:");Serial.println(abs(newVector-storedVector));
 
  /* Display the results (light is measured in lux) */
  if (event.light)
  {
    Serial.print(event.light); Serial.println(" lux");
  }
  else
  {
    /* If event.light = 0 lux the sensor is probably saturated
       and no reliable data could be generated! */
    Serial.println("Sensor overload");
  }
  delay(250);
  
  if (event.light<=20 && (abs(newVector - storedVector)< 0.25))
  {
  colorWipe(strip.Color(255, 204, 204), 255); // A
  if (currentMillis - previousMillis >= intervalLow) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    // set the LED with the ledState of the variable:
    digitalWrite(led, ledState);
    }
  }   
    if (event.light<=20 && ((abs(newVector - storedVector)>= 0.25) && (abs(newVector - storedVector)<3)))
  {
  colorWipe(strip.Color(204,102,0), 100); // B
  if (currentMillis - previousMillis >= intervalLow) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    // set the LED with the ledState of the variable:
    digitalWrite(led, ledState);
    }
  }   
    if (event.light<=20 && (abs(newVector - storedVector)>= 3)) 
  {
  colorWipe(strip.Color(255,0,0), 50); // C
  if (currentMillis - previousMillis >= intervalLow) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    // set the LED with the ledState of the variable:
    digitalWrite(led, ledState);
    }
  }  
  if (event.light>20&& event.light<400 && (abs(newVector - storedVector)< 0.25))
  {
  colorWipe(strip.Color(255, 255, 0), 255); // D
  if (currentMillis - previousMillis >= intervalMed) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    // set the LED with the ledState of the variable:
    digitalWrite(led, ledState);
    }
  }   
  if (event.light>20&& event.light<400 && (abs(newVector - storedVector)>=0.5 && abs(newVector - storedVector)<3))
  {
    colorWipe(strip.Color(0, 255, 0), 100); // E   
    if (currentMillis - previousMillis >= intervalMed) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    // set the LED with the ledState of the variable:
    digitalWrite(led, ledState);
    }
  }
  if (event.light>20&& event.light<400 && (abs(newVector - storedVector)>= 3))
  {
    colorWipe(strip.Color(0, 102, 0), 50); // F    
    if (currentMillis - previousMillis >= intervalMed) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    // set the LED with the ledState of the variable:
    digitalWrite(led, ledState);
    }
  }
  if (event.light>=400 && (abs(newVector - storedVector)<0.25))
  {
  colorWipe(strip.Color(178, 102, 255), 255); // G
  if (currentMillis - previousMillis >= intervalHigh) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    // set the LED with the ledState of the variable:
    digitalWrite(led, ledState);
    }
  }
  if (event.light>=400 && (abs(newVector - storedVector)>=0.5 && abs(newVector - storedVector)<3))
  {
  colorWipe(strip.Color(0, 0, 255), 100); // H
  if (currentMillis - previousMillis >= intervalHigh) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    // set the LED with the ledState of the variable:
    digitalWrite(led, ledState);
    }
  }
  if (event.light>=400 && (abs(newVector - storedVector)>3))
  {
  colorWipe(strip.Color(76, 0, 153), 50); // I
  if (currentMillis - previousMillis >= intervalHigh) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    // set the LED with the ledState of the variable:
    digitalWrite(led, ledState);
    }
  }
}
