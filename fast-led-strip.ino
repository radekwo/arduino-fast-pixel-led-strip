#include <FastLED.h>

#define LED_PIN     6
#define NUM_LEDS    100
#define BRIGHTNESS  255
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB
#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

CRGB leds[NUM_LEDS];
uint8_t r,g,b;

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN>(leds, NUM_LEDS);
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
//SimplePatternList gPatterns = { all_random_rgb, all_random_fade, color_wheel; color_carousel; };
SimplePatternList gPatterns = { sinelon, all_random_rgb, all_random_fade, color_wheel, color_carousel, };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND);

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void all_random_rgb() {
  for (int i = 0; i < NUM_LEDS; i++) {
    r = random(0, 256);
    g = random(0, 256);
    b = random(0, 256);
    leds[i] = CRGB(r, g, b);
  }
  FastLED.show(); // This sends the updated pixel color to the hardware.
  delay(1000); // Delay for a period of time (in milliseconds).
} //end efekt

void pixels_off(){
 for (int i=0; i < NUM_LEDS; i++) {
   leds[i] = CRGB::Black;
 }
 FastLED.show();
}

void all_random_fade() {
  //rgb fade in, out
  #define ilerazy 1

  float dimval_r[NUM_LEDS];
  float dimval_g[NUM_LEDS];
  float dimval_b[NUM_LEDS];

  //losujemy wartości R,G,B kolorów ( są to wartości max ) i obliczamy wartości DIMM
  //tak by w 255 krokach przejść od min do max
  for (int i = 0; i < NUM_LEDS; i++) {
    r = random(0, 256);
    g = random(0, 256);
    b = random(0, 256);
    dimval_r[i] = (float) r / 255;
    dimval_g[i] = (float) g / 255;
    dimval_b[i] = (float) b / 255;
  };

  //gasimy wszystko
  pixels_off();

  for (int ile = 0 ; ile < ilerazy ; ile ++) {
    //rozjasniamy
    for (int fade = 0; fade < 255; fade++){
      for (int i = 0; i < NUM_LEDS; i++) {
        r = (float) dimval_r[i] * fade;
        g = (float) dimval_g[i] * fade;
        b = (float) dimval_b[i] * fade;
        leds[i] = CRGB(r,g,b);
      }
      FastLED.show();
      delay(20);
    }
    //sciemniamy
    for (int fade = 255; fade > 0; fade--){
      for (int i = 0; i < NUM_LEDS; i++) {
        r = (float) dimval_r[i] * fade;
        g = (float) dimval_g[i] * fade;
        b = (float) dimval_b[i] * fade;
        leds[i] = CRGB(r,g,b);
      }
      FastLED.show();
      delay(20);
    }
  }
} // end

void roll_left() {
  CRGB buf;
  uint8_t lastpixel = NUM_LEDS-1;
  buf=leds[lastpixel];
  for (int i=lastpixel; i>0; i--){ leds[i]=leds[i-1]; }
  leds[0]=buf;
  FastLED.show();
}

void roll_right() {
  CRGB buf;
  uint8_t lastpixel = NUM_LEDS-1;
  buf=leds[0];
  for (int i=0; i<lastpixel; i++){ leds[i]=leds[i+1]; }
  leds[lastpixel]=buf;
  FastLED.show();
}

void color_wheel(){
  uint8_t hue;
  float dimm = 255 / NUM_LEDS;

  for (int i=0; i < NUM_LEDS; i++) {
    hue = (float) dimm * i;
    leds[i] = CHSV(hue, 255, 255);
  }
  FastLED.show();
}

void color_carousel() {
  #define maxcycle 10
  for ( int cycle=0; cycle < (maxcycle*NUM_LEDS) ; cycle++) {
      if (cycle < (maxcycle*NUM_LEDS)/2) {
        roll_left();
      } else {
      roll_right();
      }
      delay(25);
  }
}

void sinelon(){
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}
