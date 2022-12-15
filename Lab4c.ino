/* Project Lab4c
 * Description: Flash onboard LED using a timer, while controling the brightness of the external LED
                with the potentiometer.
 * Author: Leila Moussa
 * Date: November 5, 2022
 */

# define LED1 D0                    // External LED.
# define LED0 D7                    // Onboard LED.
# define BTN1 A1                    // The button which will toggle flashEnabled.
# define ADC A5                     // This is the potentiometer.
# define FLASH_RATE 200             // The rate at which LED0 will flash.

volatile bool flashEnabled = false; // Global variable controling whether the LED are on or off,
                                    // volatile so the compiler does not optimize away statement that toggle its value.

Timer timer(FLASH_RATE, flashLEDs); // A timer object that executes the function flashLEDs() every 200 ms

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
}

void loop() {
    int adcVal = analogRead(ADC);                       // To control the brightness of LED1, first read the analog value
                                                        // from the potentiometer.
    int pwmPulseWidth = adcVal / 16;                    // Scale down that value from the range [0-4095] to [0-255],
                                                        // which ends up being almost a division by 16.
    if (flashEnabled) {                                 // While LED0 flashes,
        analogWrite(LED1, pwmPulseWidth);               // LED1 is turned on continuously, but with its brightness controled.
    }
}
