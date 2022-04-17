/*
 * USED LIBRARY BELOW: 
 * https://github.com/GAMELASTER/ArduinoGamepad
 */
//including a library
#include "Gamepad.h"

// Install FastLED library using Library manager
#include <FastLED.h>

#define NUM_LEDS 10
// Define the array of leds
CRGBArray<NUM_LEDS> leds;//[NUM_LEDS];

// Arduino pins
#define STRIP_PIN 0     // PIN for sending data to LED strip
#define INIT_BRIGHTNESS 64

#define TEMPERATURE_1 Tungsten100W
#define TEMPERATURE_2 OvercastSky

// How many seconds to show each temperature before switching
#define DISPLAYTIME 20
// How many seconds to show black between switches
#define BLACKTIME   3


#define BUTTON_10 1
#define BUTTON_8 4
#define BUTTON_7 12
#define BUTTON_6 9
#define BUTTON_5 6
#define BUTTON_4 8
#define BUTTON_3 10
#define BUTTON_2 11

#define BUTTON_EXT_1 A0
#define ENC_PRS A4

#define ENC_A A3
#define ENC_B A5

#define SW_A 2
#define SW_B 7
#define SW_C 5
#define SW_D 3
//#define SW_PRS 13

enum all_modes {
  off,
  solid_white,
  solid_red,
  solid_blue,
  solid_purple,
  moving_bars,
  fire,
  rainbow_loop,
  solid_green,
  solid_yellow,
  bars,
};

volatile all_modes run_mode = solid_white;  // LED effects mode setting
long ttime = 0;                     // the last time the output pin was toggled
long debounce = 200;                // the debounce time
boolean on = false;
int start = 0;
uint8_t hue = 0;
uint8_t ihue=0;
uint8_t cntFire = 0;
void changeMode(void); 


unsigned char butdat = 0;     // Button data
unsigned char prvdat = 0;     // Previous button data
unsigned char button = 0;     // Button data store
unsigned char prvbut = 0;     // Previous button data
unsigned char presdb = 0;     // Pressed button
unsigned char vdcnt = 0;      // Valid data counter
unsigned char lvdcnt = 0;     // Long valid data counter


unsigned char butdat_pb = 0;     // Button data
unsigned char prvdat_pb = 0;     // Previous button data
unsigned char button_pb = 0;     // Button data store
unsigned char prvbut_pb = 0;     // Previous button data
unsigned char presdb_pb = 0;     // Pressed button
unsigned char vdcnt_pb = 0;      // Valid data counter
unsigned char lvdcnt_pb = 0;     // Long valid data counter


unsigned char butdat_pd = 0;     // Button data
unsigned char prvdat_pd = 0;     // Previous button data
unsigned char button_pd = 0;     // Button data store
unsigned char prvbut_pd = 0;     // Previous button data
unsigned char presdb_pd = 0;     // Pressed button
unsigned char vdcnt_pd = 0;      // Valid data counter
unsigned char lvdcnt_pd = 0;     // Long valid data counter

int val;
int encoder0Pos = 0;
int encoder0PinALast = HIGH;
int n = HIGH;

//Initializing a Gamepad
Gamepad gp;

void read_encoder(void) {
  n = digitalRead(ENC_A);
  if ((encoder0PinALast == HIGH) && (n == LOW)) 
  {
    if (digitalRead(ENC_B) == HIGH) {
      encoder0Pos--;
    } 
    else 
    {
      encoder0Pos++;
    }
    if(encoder0Pos > 20)
    {
      encoder0Pos = 20;
    }
    if(encoder0Pos < 1)
    {
      encoder0Pos = 1;
    }
    
    FastLED.setBrightness(encoder0Pos*10);
    
    Serial.print (encoder0Pos);
    Serial.print ("\n");
  }
  encoder0PinALast = n;
}

void init_led(void)
{

  // LED strip
  FastLED.addLeds<NEOPIXEL, STRIP_PIN>(leds, NUM_LEDS).setCorrection( TypicalSMD5050 );
  FastLED.setBrightness(INIT_BRIGHTNESS);
  
}


void setup() {

  init_led();
  //initializing inputs
  pinMode(BUTTON_2,  INPUT_PULLUP);
  pinMode(BUTTON_3,  INPUT_PULLUP);
  pinMode(BUTTON_4,  INPUT_PULLUP);
  pinMode(BUTTON_5,  INPUT_PULLUP);
  pinMode(BUTTON_6,  INPUT_PULLUP);
  pinMode(BUTTON_7,  INPUT_PULLUP);
  pinMode(BUTTON_8,  INPUT_PULLUP);  
  pinMode(BUTTON_10,  INPUT_PULLUP);    
  pinMode(BUTTON_EXT_1,  INPUT_PULLUP);    
  pinMode(ENC_PRS,  INPUT_PULLUP);    

  pinMode(SW_A,  INPUT_PULLUP);   
  pinMode(SW_B,  INPUT_PULLUP);     
  pinMode(SW_C,  INPUT_PULLUP);   
  pinMode(SW_D,  INPUT_PULLUP);   
  //pinMode(SW_PRS,  INPUT_PULLUP);

      
  DDRD &= ~(1<<PD5);    //Configure PORTD pin 5 as an input  --- BUTTON 9
  PORTD |= (1<<PD5);    //Activate pull-ups in PORTD pin 5

  DDRB &= ~(1<<PB0);    //Configure PORTB pin 0 as an input  --- BUTTON 1
  PORTB |= (1<<PB0);    //Activate pull-ups in PORTB pin 0

  DDRB &= ~(1<<PB2);    //Configure PORTB pin 2 as an input  --- BUTTON EXT 2
  PORTB |= (1<<PB2);    //Activate pull-ups in PORTB pin 2

  pinMode (ENC_A, INPUT);
  pinMode (ENC_B, INPUT);
  Serial.begin(9600);
}

// Make whole LED strip in single color
void runSingleColor(CRGB color) {
  FastLED.showColor(color);
}


// "Fire" effect, makes LED strip bright Yellow/Red with some
// flickering color deviations every now and than
void runFire() {
  int r = 255;
  int g = r - 40;
  int b = 40;
  ++cntFire; 
  if(cntFire >= 20) // 200ms Delay
  {
    cntFire = 0;
    memset(leds, 0, NUM_LEDS * 3);
    if (run_mode != fire) return;
    for (int x = 0; x < NUM_LEDS; x++) {
      int flicker = random(0, 150);
      int r1 = r - flicker;
      int g1 = g - flicker;
      int b1 = b - flicker;
      if (g1 < 0) g1 = 0;
      if (r1 < 0) r1 = 0;
      if (b1 < 0) b1 = 0;
      leds[x].r = r1;
      leds[x].g = g1;
      leds[x].b = b1;
    }
    FastLED.show();
  }
}


// Cycle rainbow colors on whole strip
void runRainbowCycle() 
{
  // draw a generic, no-name rainbow
  static uint8_t starthue = 0;
  fill_rainbow( leds + 5, NUM_LEDS - 5, --starthue, 20);

  // Choose which 'color temperature' profile to enable.
  uint8_t secs = (millis() / 1000) % (DISPLAYTIME * 2);
  if( secs < DISPLAYTIME) {
    FastLED.setTemperature( TEMPERATURE_1 ); // first temperature
    leds[0] = TEMPERATURE_1; // show indicator pixel
  } else {
    FastLED.setTemperature( TEMPERATURE_2 ); // second temperature
    leds[0] = TEMPERATURE_2; // show indicator pixel
  }

  // Black out the LEDs for a few secnds between color changes
  // to let the eyes and brains adjust
  if( (secs % DISPLAYTIME) < BLACKTIME) {
    memset8( leds, 0, NUM_LEDS * sizeof(CRGB));
  }
  
  FastLED.show();
}

// Loop rainbow colors on whole strip
void runRainbowLoop() {
  for (int i = 0 ; i < NUM_LEDS; i++ ) {
    leds[i] = CHSV((hue + i) % 256, 255, 255);
  }
  FastLED.show();
  hue++;
  if (hue > 255) 
    hue = 0;
}

// Moving bars
void runMovingBars() {
    CHSV color = CHSV(hue, 255, 255);
    memset(leds, 0x00, NUM_LEDS * 3);
    for (int i = (0 - start); i < (NUM_LEDS - start); i++ ) {
      if (run_mode != moving_bars) return;
      if (i % 25 == 0) on = !on;
      if (on) leds[i + start] = color;
    }
    FastLED.show();
    start++;
    if (start == 250) {
      start = 0;
      on = !on;
    }
    hue++;
    if (hue > 255) hue = 0;
}

void changeMode(void) 
{
  
    if (run_mode == off)
    {
      run_mode = solid_white;
    }
    else if(run_mode == solid_white)
    {
      run_mode = solid_red;
    }
    else if(run_mode == solid_red)
    {
      run_mode = solid_blue;
    }
    else if(run_mode == solid_blue)
    {
      run_mode = solid_purple;
    }  
    else if(run_mode == solid_purple)
    {
      run_mode = solid_green;
    }   
    else if(run_mode == solid_green)
    {
      run_mode = solid_yellow;
    }
    else if(run_mode == solid_yellow)
    {
      run_mode = moving_bars;
    }   
    else if(run_mode == moving_bars)
    {
      run_mode = rainbow_loop;
    } 
    else if(run_mode == rainbow_loop)
    {
      run_mode = fire;
      Serial.print("mode_fire\n");
    }        
    else if(run_mode == fire)
    {
      run_mode = off;
      Serial.print("modeoff\n");
    }                     
    else
    {
      run_mode = off;
    }
    Serial.print("bastı\n");
}

void enc_prs(void)
{
  changeMode();
}
void enc_rls(void)
{
  //UNS
}


void sw_c_prs(void)
{
  gp.setButtonState(1, true);
}
void sw_c_rls(void)
{
  gp.setButtonState(1, false);
}



void sw_push_prs(void)
{
  //UNS
}
void sw_push_rls(void)
{
  //UNS
}



void sw_b_prs(void)
{
  gp.setButtonState(2, true);
}

void sw_b_rls(void)
{
  gp.setButtonState(2, false);
}


void button_ex_1_prs(void)
{
  gp.setButtonState(3, true);
}
void button_ex_1_rls(void)
{
  gp.setButtonState(3, false);
}


/*=====================================================================
        *********** BUTTON READ ************      
=====================================================================*/
void butread_pc_pe_pf(void)  // SW_PUSH_CANCEL
{
  /*
   * PB0 : Button_1
   * PB2 : Button_EX_2
   * PB4 : Button_4
   * PB5 : Button_6
   * PB6 : Button_3
   * PB7 : Button_2
   * 
   * PC7 : SW_PUSH --UNS
   * PC6 : SW_C
   * 
   * PD0 : SW_D
   * PD1 : SW_A
   * PD3 : Button_10
   * PD4 : Button_8
   * PD5 : Button_9
   * PD6 : Button_7
   * PD7 : Button_5
   * 
   * PE6 : SW_B
   * 
   * PF1 : ENC_PRS
   * PF7 : Button_EX_1
   */
  butdat = PINC | 0xBF;    //SW_PUSH KULLANILSA 0x3F
  butdat = butdat >> 2;
  butdat = butdat | 0xC0;
  butdat = butdat & (PINE | 0xBF);
  butdat = butdat & (PINF | 0x7D);  

  /*
   * 0  -- NONE
   * 1  -- ENC_PRS
   * 2  -- NONE
   * 3  -- NONE
   * 4  -- SW_C
   * 5  -- SW_PUSH -- UNUSED
   * 6  -- SW_B
   * 7  -- Button_EX_1
   * 
   */
    
  if (butdat==prvdat)
  {
    vdcnt++;
    if (button != butdat)     //Long button control
    {
      if (vdcnt == 5)     //5 * 10ms = 50ms debounce
      {
        vdcnt = 0;
        prvbut = button;
        button = butdat;
        presdb = (butdat ^ prvbut) & prvbut;
        if (presdb != 0)
        {
          if (presdb == 0x80)       
          {
            button_ex_1_prs();        
          }
          if (presdb == 0x40)     
          {
            sw_b_prs();
          }
          if (presdb == 0x20)     
          {
            //sw_push_prs();    
          }
          if (presdb == 0x10)          
          {
            sw_c_prs();
          }
          if (presdb == 0x08)           
          {
           //UPS
          }
          if (presdb == 0x04)     
          {
           //UPS
          }
          if (presdb == 0x02)  
          {
            enc_prs();
          }          
          if (presdb == 0x01)  
          {
            //UPS
          }             
        }
      }
    }
  }
  else
  {
    vdcnt = 0;
    prvdat = butdat;
    enc_rls();
    sw_c_rls();
    sw_b_rls();
    button_ex_1_rls();
  }
}


void button_ex_2_prs(void)
{
  gp.setButtonState(4, true);
}
void button_ex_2_rls(void)
{
  gp.setButtonState(4, false);
}


void button_2_prs(void)
{
  gp.setButtonState(5, true);
}
void button_2_rls(void)
{
  gp.setButtonState(5, false);
}

void button_3_prs(void)
{
  gp.setButtonState(6, true);
}
void button_3_rls(void)
{
  gp.setButtonState(6, false);
}

void button_6_prs(void)
{
  gp.setButtonState(7, true);
}
void button_6_rls(void)
{
  gp.setButtonState(7, false);
}

void button_4_prs(void)
{
  gp.setButtonState(8, true);
}
void button_4_rls(void)
{
  gp.setButtonState(8, false);
}


void button_1_prs(void)
{
  gp.setButtonState(9, true);
}
void button_1_rls(void)
{
  gp.setButtonState(9, false);
}

/*=====================================================================
        *********** BUTTON READ ************      
=====================================================================*/
void butread_pb(void)
{
  /*
   * PB0 : Button_1
   * PB2 : Button_EX_2
   * PB4 : Button_4
   * PB5 : Button_6
   * PB6 : Button_3
   * PB7 : Button_2
   * 
   * PC7 : SW_PUSH
   * PC6 : SW_C
   * 
   * PD0 : SW_D
   * PD1 : SW_A
   * PD3 : Button_10
   * PD4 : Button_8
   * PD5 : Button_9
   * PD6 : Button_7
   * PD7 : Button_5
   * 
   * PE6 : SW_B
   * 
   * PF1 : ENC_PRS
   * PF7 : Button_EX_1
   */
  butdat_pb = PINB | 0x0A;

  /*
   * 0  -- Button_1
   * 1  -- NONE
   * 2  -- Button_EX_2
   * 3  -- NONE
   * 4  -- Button_4
   * 5  -- Button_6
   * 6  -- Button_3
   * 7  -- Button_2
   * 
   */
    
  if (butdat_pb==prvdat_pb)
  {
    vdcnt_pb++;
    if (button_pb != butdat_pb)     //Long button control
    {
      if (vdcnt_pb == 5)     //5 * 10ms = 50ms debounce
      {
        vdcnt_pb = 0;
        prvbut_pb = button_pb;
        button_pb = butdat_pb;
        presdb_pb = (butdat_pb ^ prvbut_pb) & prvbut_pb;
        if (presdb_pb != 0)
        {
          if (presdb_pb == 0x80)       
          {
            button_2_prs();        
          }
          if (presdb_pb == 0x40)     
          {
            button_3_prs();
          }
          if (presdb_pb == 0x20)     
          {
            button_6_prs();    
          }
          if (presdb_pb == 0x10)          
          {
            button_4_prs();
          }
          if (presdb_pb == 0x08)           
          {
           //UPS
          }
          if (presdb_pb == 0x04)     
          {
           button_ex_2_prs();
          }
          if (presdb_pb == 0x02)  
          {
            //UPS
          }          
          if (presdb_pb == 0x01)  
          {
            button_1_prs();
          }             
        }
      }
    }
  }
  else
  {
    vdcnt_pb = 0;
    prvdat_pb = butdat_pb;
    button_1_rls();
    button_2_rls();
    button_3_rls();
    button_4_rls();
    button_6_rls();
    button_ex_2_rls();
  }
}


void button_5_prs(void)
{
  gp.setButtonState(10, true);
}
void button_5_rls(void)
{
  gp.setButtonState(10, false);
}



void button_7_prs(void)
{
  gp.setButtonState(11, true);
}
void button_7_rls(void)
{
  gp.setButtonState(11, false);
}

void button_8_prs(void)
{
  gp.setButtonState(12, true);
}
void button_8_rls(void)
{
  gp.setButtonState(12, false);
}

void button_9_prs(void)
{
  gp.setButtonState(13, true);
}
void button_9_rls(void)
{
  gp.setButtonState(13, false);
}



void button_10_prs(void)
{
  gp.setButtonState(14, true);
}
void button_10_rls(void)
{
  gp.setButtonState(14, false);
}

void sw_a_prs(void)
{
  gp.setButtonState(15, true);
}
void sw_a_rls(void)
{
  gp.setButtonState(15, false);
}

void sw_d_prs(void)
{
  gp.setButtonState(0, true);
}
void sw_d_rls(void)
{
  gp.setButtonState(0, false);
}

/*=====================================================================
        *********** BUTTON READ ************      
=====================================================================*/
void butread_pd(void)
{
  /*
   * PB0 : Button_1
   * PB2 : Button_EX_2
   * PB4 : Button_4
   * PB5 : Button_6
   * PB6 : Button_3
   * PB7 : Button_2
   * 
   * PC7 : SW_PUSH
   * PC6 : SW_C
   * 
   * PD0 : SW_D
   * PD1 : SW_A
   * PD3 : Button_10
   * PD4 : Button_8
   * PD5 : Button_9
   * PD6 : Button_7
   * PD7 : Button_5
   * 
   * PE6 : SW_B
   * 
   * PF1 : ENC_PRS
   * PF7 : Button_EX_1
   */
  butdat_pd = PIND | 0x04;

  /*
   * 0  -- SW_D
   * 1  -- SW_A
   * 2  -- NONE
   * 3  -- Button_10
   * 4  -- Button_8
   * 5  -- Button_9
   * 6  -- Button_7
   * 7  -- Button_5
   * 
   */
    
  if (butdat_pd==prvdat_pd)
  {
    vdcnt_pd++;
    if (button_pd != butdat_pd)     //Long button control
    {
      if (vdcnt_pd == 5)     //5 * 10ms = 50ms debounce
      {
        vdcnt_pd = 0;
        prvbut_pd = button_pd;
        button_pd = butdat_pd;
        presdb_pd = (butdat_pd ^ prvbut_pd) & prvbut_pd;
        if (presdb_pd != 0)
        {
          if (presdb_pd == 0x80)       
          {
            button_5_prs();        
          }
          if (presdb_pd == 0x40)     
          {
            button_7_prs();
          }
          if (presdb_pd == 0x20)     
          {
            button_9_prs();    
          }
          if (presdb_pd == 0x10)          
          {
            button_8_prs();
          }
          if (presdb_pd == 0x08)           
          {
           button_10_prs();
          }
          if (presdb_pd == 0x04)     
          {
           // UPS
          }
          if (presdb_pd == 0x02)  
          {
            sw_a_prs();
          }          
          if (presdb_pd == 0x01)  
          {
            sw_d_prs();
          }             
        }
      }
    }
  }
  else
  {
    vdcnt_pd = 0;
    prvdat_pd = butdat_pd;
    button_5_rls();
    button_7_rls();
    button_8_rls();
    button_9_rls();
    button_10_rls();
    sw_a_rls();
    sw_d_rls();
  }
}

void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } }

void loop() {

  butread_pc_pe_pf();
  butread_pb();
  butread_pd();  
  read_encoder();
  delay(10);
  switch (run_mode) 
  {
    case off: runSingleColor(CRGB::Black); break;
    case solid_white: runSingleColor(CRGB::White); break;
    case solid_red: runSingleColor(CRGB::Red); break;
    case solid_blue: runSingleColor(CRGB::Blue); break;
    case solid_purple: runSingleColor(CRGB::Purple); break;
    case solid_green: runSingleColor(CRGB::Green); break;
    case solid_yellow: runSingleColor(CRGB::Yellow); break;
    case rainbow_loop: runRainbowLoop(); break;
    case moving_bars: runMovingBars(); break;
    case fire: runFire(); break;
    default:
      off: runSingleColor(CRGB::Black); break;  
  }  
}
