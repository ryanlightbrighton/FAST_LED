/*
    Author: Ryan Light

    Description:  lights up a pattern for multiple strips, with each strip having it's own random timer.
    Each strip is running in parallel, through the use of sheduler library

    https://arduino.stackexchange.com/questions/286/how-can-i-create-multiple-running-threads

    useful links:
    https://www.cutplasticsheeting.co.uk/mirrored-sheeting/two-way-acrylic-mirror
    https://www.cutplasticsheeting.co.uk/mirror-discs/acrylic-mirror-discs/
    https://www.screwfix.com/p/manrose-100mm-round-pipe-0-35m/15872
    https://www.youtube.com/watch?v=FBTtdssTVaU&list=LLgbnOEsFsPBtnf1hdx6_G4g&index=2&t=0s
    https://www.etsy.com/listing/636273983/infinity-mirror-led-light-up-party-dress?ref=shop_home_active_5

    Also, to change indentation edit "editor.tabs.size" in C:\Users\Username\AppData\Local\Arduino15\preferences.txt
*/

/*
    Includes
*/

#include <Scheduler.h>
#include <FastLED.h>
#include <Prandom.h>

/*
    Defines, vars & strip arrays
*/

Prandom R;

#define COLOR_ORDER GRB
#define NUM_LEDS_PER_STRIP 10
CRGB leds11[NUM_LEDS_PER_STRIP];
CRGB leds12[NUM_LEDS_PER_STRIP];
CRGB leds13[NUM_LEDS_PER_STRIP];
int minT = 20;
int maxT = 100;
int brightness = 50;
// vars for showfps
long currentMillis = 0;
long lastMillis = 0;
long loops = 0;
bool adjusted = false;

/*
    Function to generate a random number between 0 and the argument passed in
*/

long randomTimer(int minTime, int maxTime) {
    long tr = R.randrange(minTime, maxTime);
    return tr;
}

/*
    Function to return modulo of a number (even if negative)
*/

int mod(int x, int n) {
    return (x % n + n) % n;
}

/*
    Function to create a pattern for a strip.
*/

void createPattern(CRGB* ptr, long delayCycle) {
    for (int j = 0; j < NUM_LEDS_PER_STRIP; j++) {
        // Turn all LEDs off
        for (int i = 0; i < NUM_LEDS_PER_STRIP; i++) {
            ptr[i].fadeToBlackBy(170);
        }
        // Turn on an LED and make a trail
        int hue = map(j,0,NUM_LEDS_PER_STRIP,0,255);
        //int hue = map(delayCycle,0,maxT,0,255);
        ptr[j] = CHSV(hue, 255, 255);
        FastLED.show();
        if (! adjusted) {
            delay(delayCycle);
        } else {
            adjusted = false;
            FastLED.clear();
        }
    }
}

void debug() {
    Serial.println("/*************/");
    Serial.println("MinTime: " + String(minT));
    Serial.println("MaxTime: " + String(maxT));
    Serial.println("Brightness: " + String(brightness));
}

void showfps() {
    currentMillis=millis();               // Determine frames per second
    loops++;
    if(currentMillis - lastMillis > 1000){
        Serial.println("FPS: " + String(loops));
        lastMillis = currentMillis;
        loops = 0;
    }
}

/*
    Setups and loops for each additional strip
*/

void setup1() {
    FastLED.addLeds<WS2812B, 13, COLOR_ORDER>(leds13, NUM_LEDS_PER_STRIP);
}

void loop1() {
    createPattern(&leds13[0], randomTimer(minT, maxT));
}

void setup2() {
    FastLED.addLeds<WS2812B, 33, COLOR_ORDER>(leds11, NUM_LEDS_PER_STRIP);
}

void loop2() {
    createPattern(&leds11[0], randomTimer(minT, maxT));
}

/*
    Setup sets up the first strip and runs scheduler for all other strips
*/

void setup() {
    Serial.begin(9600);
    delay(3000);
    debug();
    FastLED.setBrightness(brightness);
    FastLED.addLeds<WS2812B, 53, COLOR_ORDER>(leds12, NUM_LEDS_PER_STRIP);
    Scheduler.start(setup1, loop1);
    Scheduler.start(setup2, loop2);
}

/*
    Main loop (only first strip handled here)
*/

void loop() {
    //showfps();
    if (Serial.available()) {
        String dataIn = Serial.readString();
        if (dataIn.startsWith("2")) {
            /*
                data needs to be sent in format 2<numb>
            */
            String stringMinTime = dataIn.substring(dataIn.indexOf("2") + 1, dataIn.length());
            minT = min(stringMinTime.toInt(),maxT);
            adjusted = true;
        } else if (dataIn.startsWith("3")) {
            /*
                data needs to be sent in format 3<numb>
            */
            String stringMaxTime = dataIn.substring(dataIn.indexOf("3") + 1, dataIn.length());
            maxT = max(stringMaxTime.toInt(),minT);
            adjusted = true;
        } else if (dataIn.startsWith("4")) {
            /*
                data needs to be sent in format 3<numb>
            */
            String stringBrightness = dataIn.substring(dataIn.indexOf("4") + 1, dataIn.length());
            brightness = stringBrightness.toInt();
            FastLED.setBrightness(brightness); // between 0 and 255
            adjusted = true;
        }
        /*
            debug
        */
        debug();
    }
    createPattern(&leds12[0], randomTimer(minT, maxT));
}
