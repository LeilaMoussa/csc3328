/* Project Lab5b
 * Description: Expose a function, the hardware Interrupt Service Routine, to Particle Cloud
                with the same functionality as the Lab4c, adding the possibility of invoking the function
                using Particle's API, like from an HTML web page.
 * Author: Leila Moussa
 * Date: November 14, 2022
 */

# define LED1 D0                    // External LED.
# define LED0 D7                    // Onboard LED.
# define BTN1 A1                    // The button which will toggle flashEnabled.
# define ADC A5                     // This is the potentiometer.
# define FLASH_RATE 200             // The rate at which LED0 will flash.

volatile bool flashEnabled = false; // Global variable controling whether the LED are on or off,
                                    // volatile so the compiler does not optimize away statement that toggle its value.
int adcVal = 0;

Timer timer(FLASH_RATE, flashLEDs); // A timer object that executes the function flashLEDs() every 200 ms

int cloudToggleFlashEnabled(String cmd) {   // To expose a function to the cloud, it must have this signature:
                                            // a string of arguments as arguments and an integer as a return value
                                            // String is a class defined by the DeviceOS.
    flashEnabled = !flashEnabled;           // Perform the same logic as toggleFlashEnabled below
    return 0;                               // Exit code 0 indicates success. For more complicated/fallible logic
                                            // other values may be relevant.
}

void toggleFlashEnabled() {         // The ISR corresponding to the BTN1 event.
    flashEnabled = !flashEnabled;   // Toggle flashEnabled.
}

void flashLEDs() {                              // The timer's ISR.
    if (flashEnabled) {
        digitalWrite(LED0, !digitalRead(LED0)); // Write the opposite value of the LED, effectively turning it on/off
                                                // every time the timer goes off.
    } else {
        digitalWrite(LED1, LOW);                // If flashEnabled is false, both LEDs should be turned off anyway.
        digitalWrite(LED0, LOW);
    }
}

void setup() { 
    pinMode(LED1, OUTPUT);                              // Both LEDs get output from the photon.
    pinMode(LED0, OUTPUT);
    pinMode(BTN1, INPUT_PULLDOWN);                      // The button supplies input to the photon and is active high.
    attachInterrupt(BTN1, toggleFlashEnabled, RISING);  // This interrupt triggers toggleFlashEnabled() whenever a rising edge
                                                        // is detected in the BTN1 signal.
    timer.start();                                      // In the beginning of the program, start the timer, so it begins executing
                                                        // its corresponding ISR at every set interval right away.
                                                        
    Particle.variable("adcValue", adcVal);                  // Register the variable `adcVal` with the name "adcValue" with Particle Cloud.
                                                            // We can inspect the value of the variable in (almost) real time
                                                            // in Particle Console.
    Particle.function("toggleFlashFlag", cloudToggleFlashEnabled);  // Similarly register the function `cloudToggleFlashEnabled` to be able to call
                                                                    // using HTTP requests for example.
                                                                    // Must ensure that this name matches that in the URL to be called.
                                                                    // Sample response:
                                                                    // {"id":"55004a001051373331333230","name":"photon13","connected":true,"return_value":0}
}

void loop() {
    adcVal = analogRead(ADC);                           // To control the brightness of LED1, first read the analog value
                                                        // from the potentiometer.
    int pwmPulseWidth = adcVal / 16;                    // Scale down that value from the range [0-4095] to [0-255],
                                                        // which ends up being almost a division by 16.
    if (flashEnabled) {                                 // While LED0 flashes,
        analogWrite(LED1, pwmPulseWidth);               // LED1 is turned on continuously, but with its brightness controled.
    }
}
