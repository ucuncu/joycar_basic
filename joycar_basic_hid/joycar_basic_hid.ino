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


#define SW_ENC_A A1
#define SW_ENC_B A2

#define SW_A 2
#define SW_B 7
#define SW_C 5
#define SW_D 3
#define SW_PRS 13

enum all_modes {
  off,
  solid_white,
  solid_red,
  solid_blue,
  solid_purple,
  fire,
  rainbow_loop,
  solid_green,
  solid_yellow,
  bars,
};

// BUTTON MAP

#define usb_but_1 0
#define usb_but_2 1
#define usb_but_3 2
#define usb_but_4 3
#define usb_but_5 4
#define usb_but_6 5
#define usb_but_7 6
#define usb_but_8 7
#define usb_but_9 8
#define usb_but_10 9
#define usb_but_ex_1 10
#define usb_but_ex_2 11
#define usb_sw_a 12
#define usb_sw_b 13
#define usb_sw_c 14
#define usb_sw_d 15
#define usb_sw_push 16
#define usb_sw_inc 17
#define usb_sw_dec 18


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

int encoder0Pos = 0;
int encoder0PinALast = HIGH;
int n = HIGH;

int sw_encoder_Pos = 0;
int sw_encoder_PinALast = HIGH;
int sw_n = HIGH;


volatile boolean f10ms = LOW;
volatile int counter = 0;

//Initializing a Gamepad
Gamepad gp;




// Timer1 interrupt
ISR (TIMER1_COMPA_vect) {
  f10ms = HIGH;
}


void setupTimer() {
  cli();
  initTimer1();
  sei();
}

void initTimer1() {
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 10000;
  TCCR1B = bit(WGM12) | bit(CS12)| bit(CS10);  // WGM12 => CTC(Clear Timer on Compare Match), CS12 & CS10  => prescaler 1/1024
  TIMSK1 = bit(OCIE1A);                        // OCIE1A => Timer1 compare match A interrupt
}


void setTimer1(float _time) {
  long cnt = 16000000 / 1024 * _time;  // cnt = clk / prescaler * time(s)
  if(cnt > 65535) {
    cnt = 65535;         // "timer1 16bit counter over."
  }
  OCR1A = cnt;           // Output Compare Register Timer1A
  TIMSK1 = bit(OCIE1A);
}

void stopTimer1(){
    TIMSK1 = 0;
}

/*
|  CS12  |  CS11  |  CS10  |  Description                       |
|:-------|:------:|:------:|:----------------------------------:|
|   0    |    0   |    0   |  No clock source(timer stop)       |
|   0    |    0   |    1   |  clk / 1                           |
|   0    |    1   |    0   |  clk / 8                           |
|   0    |    1   |    1   |  clk / 64                          |
|   1    |    0   |    0   |  clk / 256                         |
|   1    |    0   |    1   |  clk / 1024                        |
|   1    |    1   |    0   |  External clock source no T1 pin.  |
|   1    |    1   |    1   |  External clock source no T1 pin.  |
*/



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
    
  }
  encoder0PinALast = n;
}

void sw_read_encoder(void) {
  sw_n = digitalRead(SW_ENC_A);
  if ((sw_encoder_PinALast == HIGH) && (sw_n == LOW)) 
  {  
    if (digitalRead(SW_ENC_B) == HIGH) {
        gp.setButtonState(18, true);
        delay(50);
        gp.setButtonState(18, false);
    } 
    else 
    {
        gp.setButtonState(17, true);
        delay(50);
        gp.setButtonState(17, false);
    }
  }
  if ((sw_encoder_PinALast == LOW) && (sw_n == HIGH)) 
  { 
    if (digitalRead(SW_ENC_B) == LOW) {
        gp.setButtonState(18, true);
        delay(50);
        gp.setButtonState(18, false);
    } 
    else 
    {
        gp.setButtonState(17, true);
        delay(50);
        gp.setButtonState(17, false);
    }
  }  
  sw_encoder_PinALast = sw_n;
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
  pinMode(SW_PRS,  INPUT_PULLUP);

      
  DDRD &= ~(1<<PD5);    //Configure PORTD pin 5 as an input  --- BUTTON 9
  PORTD |= (1<<PD5);    //Activate pull-ups in PORTD pin 5

  DDRB &= ~(1<<PB0);    //Configure PORTB pin 0 as an input  --- BUTTON 1
  PORTB |= (1<<PB0);    //Activate pull-ups in PORTB pin 0

  DDRB &= ~(1<<PB2);    //Configure PORTB pin 2 as an input  --- BUTTON EXT 2
  PORTB |= (1<<PB2);    //Activate pull-ups in PORTB pin 2

  pinMode(ENC_A, INPUT);
  pinMode(ENC_B, INPUT);

  pinMode(SW_ENC_A, INPUT);
  pinMode(SW_ENC_B, INPUT);


  setupTimer();
  setTimer1(0.01);

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
      run_mode = rainbow_loop;
    }   
    else if(run_mode == rainbow_loop)
    {
      run_mode = fire;
    }        
    else if(run_mode == fire)
    {
      run_mode = off;
    }                     
    else
    {
      run_mode = off;
    }
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
  gp.setButtonState(usb_sw_c, true);
}
void sw_c_rls(void)
{
  gp.setButtonState(usb_sw_c, false);
}



void sw_push_prs(void)
{
  gp.setButtonState(usb_sw_push, true);
}
void sw_push_rls(void)
{
  gp.setButtonState(usb_sw_push, false);
}



void sw_b_prs(void)
{
  gp.setButtonState(usb_sw_b, true);
}

void sw_b_rls(void)
{
  gp.setButtonState(usb_sw_b, false);
}


void button_ex_1_prs(void)
{
  gp.setButtonState(usb_but_ex_1, true);
}
void button_ex_1_rls(void)
{
  gp.setButtonState(usb_but_ex_1, false);
}

void sw_a_prs(void)
{
  gp.setButtonState(usb_sw_a, true);
}
void sw_a_rls(void)
{
  gp.setButtonState(usb_sw_a, false);
}

void sw_d_prs(void)
{
  gp.setButtonState(usb_sw_d, true);
}
void sw_d_rls(void)
{
  gp.setButtonState(usb_sw_d, false);
}
/*=====================================================================
        *********** BUTTON READ ************      
=====================================================================*/
void butread_pc_pd_pe_pf(void)  // SW_PUSH_CANCEL
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
  butdat = PINC | 0x3F;
  butdat = butdat >> 2;
  butdat = butdat | 0xC0;
  butdat = butdat & (PINE | 0xBF);
  butdat = butdat & (PINF | 0x7D);  
  butdat = butdat & (((PIND | 0xFC) << 2) | 0xF3);
  /*
   * 0  -- NONE
   * 1  -- ENC_PRS
   * 2  -- SW_D
   * 3  -- SW_A
   * 4  -- SW_C
   * 5  -- SW_PUSH
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
          if (presdb & 0x80)       
          {
            button_ex_1_prs();        
          }
          if (presdb & 0x40)     
          {
            sw_b_prs();
          }
          if (presdb & 0x20)   // FIZIKSEK OLARAK SW PUSH PRESS BIRLIKTE BASIYOR       
          {
            if ((!(presdb & 0x40)) & (!(presdb & 0x10)) & (!(presdb & 0x08))  & (!(presdb & 0x04)) ) 
            { 
              sw_push_prs();   
            } 
          }
          if (presdb & 0x10)          
          {
            sw_c_prs();
          }
          if (presdb & 0x08)           
          {
           sw_a_prs();
          }
          if (presdb & 0x04)
          {
           sw_d_prs();
          }
          if (presdb & 0x02)  
          {
            enc_prs();
          }          
          if (presdb & 0x01)  
          {
            //UPS
          }             
        }
        else
        {
          sw_push_rls();
          sw_c_rls();
          sw_b_rls();
          sw_a_rls();
          sw_d_rls();
          button_ex_1_rls();
        }
      }
    }
  }
  else
  {
    vdcnt = 0;
    prvdat = butdat;
  }
}


void button_ex_2_prs(void)
{
  gp.setButtonState(usb_but_ex_2, true);
}
void button_ex_2_rls(void)
{
  gp.setButtonState(usb_but_ex_2, false);
}


void button_2_prs(void)
{
  gp.setButtonState(usb_but_2, true);
}
void button_2_rls(void)
{
  gp.setButtonState(usb_but_2, false);
}

void button_3_prs(void)
{
  gp.setButtonState(usb_but_3, true);
}
void button_3_rls(void)
{
  gp.setButtonState(usb_but_3, false);
}

void button_6_prs(void)
{
  gp.setButtonState(usb_but_6, true);
}
void button_6_rls(void)
{
  gp.setButtonState(usb_but_6, false);
}

void button_4_prs(void)
{
  gp.setButtonState(usb_but_4, true);
}
void button_4_rls(void)
{
  gp.setButtonState(usb_but_4, false);
}


void button_1_prs(void)
{
  gp.setButtonState(usb_but_1, true);
}
void button_1_rls(void)
{
  gp.setButtonState(usb_but_1, false);
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
          if (presdb_pb & 0x80)       
          {
            button_2_prs();        
          }
          if (presdb_pb & 0x40)     
          {
            button_3_prs();
          }
          if (presdb_pb & 0x20)     
          {
            button_6_prs();    
          }
          if (presdb_pb & 0x10)          
          {
            button_4_prs();
          }
          if (presdb_pb & 0x08)           
          {
           //UPS
          }
          if (presdb_pb & 0x04)     
          {
           button_ex_2_prs();
          }
          if (presdb_pb & 0x02)  
          {
            //UPS
          }          
          if (presdb_pb & 0x01)  
          {
            button_1_prs();
          }             
        }
        else
        {
          button_1_rls();
          button_2_rls();
          button_3_rls();
          button_4_rls();
          button_6_rls();
          button_ex_2_rls();
        }
      }
    }
  }
  else
  {
    vdcnt_pb = 0;
    prvdat_pb = butdat_pb;
  }
}


void button_5_prs(void)
{
  gp.setButtonState(usb_but_5, true);
}
void button_5_rls(void)
{
  gp.setButtonState(usb_but_5, false);
}



void button_7_prs(void)
{
  gp.setButtonState(usb_but_7, true);
}
void button_7_rls(void)
{
  gp.setButtonState(usb_but_7, false);
}

void button_8_prs(void)
{
  gp.setButtonState(usb_but_8, true);
}
void button_8_rls(void)
{
  gp.setButtonState(usb_but_8, false);
}

void button_9_prs(void)
{
  gp.setButtonState(usb_but_9, true);
}
void button_9_rls(void)
{
  gp.setButtonState(usb_but_9, false);
}



void button_10_prs(void)
{
  gp.setButtonState(usb_but_10, true);
}
void button_10_rls(void)
{
  gp.setButtonState(usb_but_10, false);
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
  butdat_pd = PIND | 0x07;

  /*
   * 0  -- NONE
   * 1  -- NONE
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
          if (presdb_pd & 0x80)       
          {
            button_5_prs();        
          }
          if (presdb_pd & 0x40)     
          {
            button_7_prs();
          }
          if (presdb_pd & 0x20)     
          {
            button_9_prs();    
          }
          if (presdb_pd & 0x10)          
          {
            button_8_prs();
          }
          if (presdb_pd & 0x08)           
          {
           button_10_prs();
          }
          if (presdb_pd & 0x04)     
          {
           // UPS
          }
          if (presdb_pd & 0x02)  
          {
            // UPS
          }          
          if (presdb_pd & 0x01)  
          {
            // UPS
          }             
        }
        else
        {
          button_5_rls();
          button_7_rls();
          button_8_rls();
          button_9_rls();
          button_10_rls();
        }
      }
    }
  }
  else
  {
    vdcnt_pd = 0;
    prvdat_pd = butdat_pd;
  }
}

void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } }

void loop() {
  if(f10ms)
  {
    f10ms = LOW; 
    butread_pc_pd_pe_pf();
    butread_pb();
    butread_pd(); 
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
      case fire: runFire(); break;
      default:
        off: runSingleColor(CRGB::Black); break;  
    }    
  }
 
  read_encoder();
  sw_read_encoder();  
}
