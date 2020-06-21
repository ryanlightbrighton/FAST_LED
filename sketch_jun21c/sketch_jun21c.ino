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

    // http://fastled.io/tools/paletteknife/
*/

/*
    Includes
*/

#include <FastLED.h>
#include <Prandom.h>

/*
    Defines, vars & strip arrays
*/

Prandom R;

#define COLOR_ORDER GRB
CRGB leds1[10];
CRGB leds2[10];
CRGB leds3[10];
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

struct lights {
    long nextTime;
    int index;
    long interval;
    CRGB* ptr;
    int numLEDS;
    CRGB ptrArr[10];
};

struct lights myLights1 = {0, 0, randomTimer(minT, maxT), &leds1[0], 10, &leds1};
struct lights myLights2 = {0, 0, randomTimer(minT, maxT), &leds2[0], 10, &leds2};
struct lights myLights3 = {0, 0, randomTimer(minT, maxT), &leds3[0], 10, &leds3};

lights myArr[] = {myLights1, myLights2, myLights3};

/*
    Setup sets up the first strip and runs scheduler for all other strips
*/

void setup() {
    Serial.begin(9600);
    delay(3000);
    debug();
    FastLED.setBrightness(brightness);
    FastLED.addLeds<WS2812B, 13, COLOR_ORDER>(leds1, myLights1.numLEDS);
    FastLED.addLeds<WS2812B, 33, COLOR_ORDER>(leds2, myLights2.numLEDS);
    FastLED.addLeds<WS2812B, 53, COLOR_ORDER>(leds3, myLights3.numLEDS);
}

/*
    Main loop (only first strip handled here)
*/
uint8_t gHue = 0;   // rotating "base color" used by many of the patterns
int choice = 0;
void loop() {
    showfps();
    if (Serial.available()) {
        String dataIn = Serial.readString();
        if (dataIn.startsWith("1")) {
            /*
                data needs to be sent in format 2<numb>
            */
            String stringMinTime = dataIn.substring(dataIn.indexOf("1") + 1, dataIn.length());
            choice = stringMinTime.toInt();
            adjusted = true;
        } else if (dataIn.startsWith("2")) {
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
    if (choice == 0) {
        for (int i = 0; i < 3; i++) {
            createPattern(&myArr[i]);
        }
    } else if (choice == 1) {
        for (int i = 0; i < 3; i++) {
            createPattern3(&myArr[i]);
        }
    } else if (choice == 2) {
        for (int i = 0; i < 3; i++) {
            createPattern2(&myArr[i]);
        }
    } else if (choice == 3) {
        for (int i = 0; i < 3; i++) {
            confetti(&myArr[i]);
        }
    } else if (choice == 4) {
        for (int i = 0; i < 3; i++) {
            bpm(&myArr[i]);
        }
    } else {
        for (int i = 0; i < 3; i++) {
            juggle(&myArr[i]);
        }
    }
    EVERY_N_MILLISECONDS(20000) {
        choice++;
        choice =mod(choice, 6);
    }
    EVERY_N_MILLISECONDS(20) { 
        gHue++;
    }
    FastLED.show();
}

void bpm(struct lights* thisLight) {
    //if (millis() >= thisLight->nextTime) {
        // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
        int num_leds = thisLight->numLEDS;
        CRGB* ptr = thisLight->ptr;
        uint8_t BeatsPerMinute = 62;
        CRGBPalette16 palette = PartyColors_p;
        uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
        for( int i = 0; i < num_leds; i++) { //9948
            ptr[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
        }
        for (int i = 0; i < num_leds; i++) {
            ptr[i].fadeToBlackBy(240);
        }
        //thisLight->nextTime = 200 + millis();
    //}
}

void juggle(struct lights* thisLight) {
    if (millis() >= thisLight->nextTime) {
        CRGBPalette16 palette = PartyColors_p; // new
        int num_leds = thisLight->numLEDS;
        CRGB* ptr = thisLight->ptr;
        byte dothue = 0;
        uint8_t beat = beatsin8( 62, 64, 255); // new
        for( int i = 0; i < 8; i++) {
            ptr[beatsin16( i+7, 0, num_leds-1 )] /*|= CHSV(dothue, 200, 255);*/ = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
            dothue += 32;
        }
        for (int i = 0; i < num_leds; i++) {
            ptr[i].fadeToBlackBy(70);
        }
        thisLight->nextTime = 20 + millis();
    }
}

void confetti(struct lights* thisLight){
    if (millis() >= thisLight->nextTime) {
        int num_leds = thisLight->numLEDS;
        CRGB* ptr = thisLight->ptr;
        // random colored speckles that blink in and fade smoothly
        //fadeToBlackBy( ptrArr, num_leds, 10);
        for (int i = 0; i < num_leds; i++) {
            ptr[i].fadeToBlackBy(100);
        }
        int pos = random16(num_leds);
        ptr[pos] += CHSV( gHue + random8(64), 200, 255);
        thisLight->nextTime = 200 + millis();
    }
}

void createPattern(struct lights* thisLight) {
    if (millis() >= thisLight->nextTime) {   
        // Turn all LEDs off
        CRGB* ptr = thisLight->ptr;
        int num_leds = thisLight->numLEDS;
        for (int i = 0; i < num_leds; i++) {
            ptr[i].fadeToBlackBy(170);
        }
        int idx = thisLight->index;
        // Turn on an LED and make a trail
        int hue = map(idx,0,num_leds,0,255);
        ptr[idx] = CHSV(hue, 255, 255);
        thisLight->nextTime = thisLight->interval + millis();
        idx = idx + 1;
        idx = mod(idx,num_leds);
        thisLight->index = idx;
        if (thisLight->index == 0) {
            thisLight->interval = randomTimer(minT, maxT);
        }
    }
}

void createPattern2(struct lights* thisLight) {
    if (millis() >= thisLight->nextTime) {   
        // Turn all LEDs off
        CRGB* ptr = thisLight->ptr;
        int num_leds = thisLight->numLEDS;
        for (int i = 0; i < num_leds; i++) {
            ptr[i].fadeToBlackBy(100);
        }
        //int idx = 0;
        // Turn on an LED and make a trail
        int idx = map(sin16(millis()*32),-32768,32767,0,num_leds);
        int hue = map(idx,0,num_leds,0,255);
        ptr[idx] = CHSV(hue, 255, 255);
        thisLight->nextTime = 50 + millis();
    }
}

void createPattern3(struct lights* thisLight) {
    if (millis() >= thisLight->nextTime) {   
        // Turn all LEDs off
        CRGB* ptr = thisLight->ptr;
        int num_leds = thisLight->numLEDS;
        for (int i = 0; i < num_leds; i++) {
            ptr[i].fadeToBlackBy(170);
        }
        int idx = thisLight->index;
        // Turn on an LED and make a trail
        if (thisLight->interval < 50) {
            ptr[idx] = CRGB(255, 255, 255);
        } else if (thisLight->interval < 100) {
            ptr[idx] = CRGB(255, 128, 213);
        } else if (thisLight->interval < 150) {
            ptr[idx] = CRGB(153, 0, 230);
        } else {
            ptr[idx] = CRGB(42, 0, 128);
        }
        thisLight->nextTime = thisLight->interval + millis();
        idx = idx + 1;
        idx = mod(idx,num_leds);
        thisLight->index = idx;
        if (thisLight->index == 0) {
            thisLight->interval = randomTimer(minT, maxT);
        }
    }
}
