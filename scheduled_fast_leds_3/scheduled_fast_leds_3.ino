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
int maxT = 200;
int brightness = 50;
// vars for showfps
long currentMillis = 0;
long lastMillis = 0;
long loops = 0;

struct color_class {
  int red;
  int green;
  int blue;
};

struct color_class mainColors = {42, 0, 128};
struct color_class mainColors2 = {153, 0, 230};
struct color_class mainColors3 = {255, 128, 213};
struct color_class altColors = {255, 255, 255};

/*
    Function to generate a random number between 0 and the argument passed in
*/

long randomTimer(int minTime, int maxTime) {
    // read value of an unused pin and gen random value from it
    //srand(analogRead(41));
    //long tr = max(minTime, rand() % maxTime);
    //randomSeed(analogRead(41));
    //long tr = random(minTime, maxTime);
    long tr = R.randrange(minTime, maxTime);
    //Serial.println("Rand value: " + String(tr));
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
            ptr[i] = CRGB(0, 0, 0);
        }
        //FastLED.show();
        struct color_class obj;
        if (delayCycle < 50) {
            obj = altColors;
        } else if (delayCycle < 100) {
            obj = mainColors3;
        } else if (delayCycle < 150) {
            obj = mainColors2;
        } else {
            obj = mainColors;
        }
        // Turn on an LED and make a trail
        ptr[j] = CRGB(obj.red, obj.green, obj.blue);
        //ptr[mod(j - 1, NUM_LEDS_PER_STRIP)] = CRGB(max(obj.red / 2, 1), max(obj.green / 2, 1), max(obj.blue / 2, 1));
        //ptr[mod(j - 2, NUM_LEDS_PER_STRIP)] = CRGB(max(obj.red / 6, 1), max(obj.green / 6, 1), max(obj.blue / 6, 1));
        //ptr[mod(j - 3, NUM_LEDS_PER_STRIP)] = CRGB(max(obj.red / 12, 1), max(obj.green / 12, 1), max(obj.blue / 12, 1));
        //ptr[mod(j - 4, NUM_LEDS_PER_STRIP)] = CRGB(max(obj.red / 15, 1), max(obj.green / 15, 1), max(obj.blue / 15, 1));
        ptr[mod(j - 1, NUM_LEDS_PER_STRIP)] = CRGB(obj.red, obj.green, obj.blue);
        ptr[mod(j - 1, NUM_LEDS_PER_STRIP)].maximizeBrightness(70);
        ptr[mod(j - 2, NUM_LEDS_PER_STRIP)] = CRGB(obj.red, obj.green, obj.blue);
        ptr[mod(j - 2, NUM_LEDS_PER_STRIP)].maximizeBrightness(50);
        ptr[mod(j - 3, NUM_LEDS_PER_STRIP)] = CRGB(obj.red, obj.green, obj.blue);
        ptr[mod(j - 3, NUM_LEDS_PER_STRIP)].maximizeBrightness(25);
        ptr[mod(j - 4, NUM_LEDS_PER_STRIP)] = CRGB(obj.red, obj.green, obj.blue);
        ptr[mod(j - 4, NUM_LEDS_PER_STRIP)].maximizeBrightness(12);
        FastLED.show();
        delay(delayCycle);
    }
}

void debug() {
    Serial.println("/*************/");
    Serial.println("Colors: R: " + String(mainColors.red) + " G: " + String(mainColors.green) + " B: " + String(mainColors.blue));
    Serial.println("MinTime: " + String(minT));
    Serial.println("MaxTime: " + String(maxT));
    Serial.println("Brightness: " + String(brightness));
}

void showfps() {
    currentMillis=millis();               // Determine frames per second
    loops++;
    if(currentMillis - lastMillis > 1000){
        Serial.println(loops);
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
        if (dataIn.startsWith("1")) {
            /*
                data needs to be sent in format 1R<numb>G<numb>B<numb>E
            */
            String stringR = dataIn.substring(dataIn.indexOf("R") + 1, dataIn.indexOf("G"));
            mainColors.red = stringR.toInt();
            String stringG = dataIn.substring(dataIn.indexOf("G") + 1, dataIn.indexOf("B"));
            mainColors.green = stringG.toInt();
            String stringB = dataIn.substring(dataIn.indexOf("B") + 1, dataIn.indexOf("E"));
            mainColors.blue = stringB.toInt();
        } else if (dataIn.startsWith("2")) {
            /*
                data needs to be sent in format 2<numb>
            */
            String stringMinTime = dataIn.substring(dataIn.indexOf("2") + 1, dataIn.length());
            minT = min(stringMinTime.toInt(),maxT);
        } else if (dataIn.startsWith("3")) {
            /*
                data needs to be sent in format 3<numb>
            */
            String stringMaxTime = dataIn.substring(dataIn.indexOf("3") + 1, dataIn.length());
            maxT = max(stringMaxTime.toInt(),minT);
        } else if (dataIn.startsWith("4")) {
            /*
                data needs to be sent in format 3<numb>
            */
            String stringBrightness = dataIn.substring(dataIn.indexOf("4") + 1, dataIn.length());
            brightness = stringBrightness.toInt();
            FastLED.setBrightness(brightness); // between 0 and 255
        }
        /*
            debug
        */
        debug();
    }
    createPattern(&leds12[0], randomTimer(minT, maxT));
}
