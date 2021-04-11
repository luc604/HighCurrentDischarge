#include <Arduino.h>
/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x64 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

float set_current = 0;
float set_voltage = 3.6;
String log_state = "YES";
int row_edit = 0;
int row_select = 0;

static uint8_t prevNextCode = 0;
static uint16_t store=0;
#define BUTN 7
#define ENC_A 8
#define ENC_B 9


int edit_coors(char coordinate, int line) {
  if (coordinate == 'x'){
    if (line <= 2) return 88;
    if (line == 3) return 35;
  }
  else {
    return 12 + line * 12;
  }
}

void drawSetupMenu() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 16);
  display.print("Set Voltage: ");
  display.print(set_voltage);
  display.println("V");
  display.setCursor(10, 28);
  display.print("Set Current: ");
  display.print(set_current);
  display.println("A");
  display.setCursor(10, 40);
  display.print(F("Log: "));
  display.println(log_state);
  display.setCursor(10, 52);
  display.println(F("Run"));

  if (row_select > 0) {
    display.setCursor(1, 4 + row_select * 12);
    display.print(F(">"));
    if (row_edit){
      int x = edit_coors('x', row_select);
      int y = edit_coors('y', row_select);
      display.drawLine(x, y, x+29, y, SSD1306_WHITE);
    }
  }
  display.display();      // Show initial text
}

int8_t read_rotary() {
  static int8_t rot_enc_table[] = {0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0};

  prevNextCode <<= 2;
  if (digitalRead(ENC_B)) prevNextCode |= 0x02;
  if (digitalRead(ENC_A)) prevNextCode |= 0x01;
  prevNextCode &= 0x0f;

   // If valid then store as 16 bit data.
   if  (rot_enc_table[prevNextCode] ) {
      store <<= 4;
      store |= prevNextCode;
      //if (store==0xd42b) return 1;
      //if (store==0xe817) return -1;
      if ((store&0xff)==0x2b) return -1;
      if ((store&0xff)==0x17) return 1;
   }
   return 0;
}

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // Clear the buffer
  display.clearDisplay();
  display.setTextSize(1);
  pinMode(ENC_A, INPUT);
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT);
  pinMode(ENC_B, INPUT_PULLUP);
  pinMode(BUTN, INPUT);
  pinMode(BUTN, INPUT_PULLUP);

}



void loop() {
  int update = 0;

   if( read_rotary() ) {

      if ( prevNextCode==0x0b) {
        if (row_edit){
          if(row_select==1) set_voltage = set_voltage + 0.05;
          if(row_select==2) set_current = set_current + 0.05;
          if(row_select==3) {
            if(log_state=="YES") log_state="NO";
            else log_state="YES";
          }
        } else {
         row_select++;
         if (row_select > 4) row_select = 0;
        }
      }

      if ( prevNextCode==0x07) {
        if (row_edit){
          if(row_select==1) set_voltage = set_voltage - 0.05;
          if(row_select==2) set_current = set_current - 0.05;
          if(row_select==3) {
            if(log_state=="YES") log_state="NO";
            else log_state="YES";
          }
        } else {
         row_select--;
         if (row_select < 0) row_select = 4;
        }
      }
      update = true;
   }
  if (!digitalRead(BUTN) && row_select < 4) {
    delay(300);
    if (row_edit==1) row_edit =0;
    else row_edit = 1;
    update = true;
  }
   if(update) drawSetupMenu();
}
