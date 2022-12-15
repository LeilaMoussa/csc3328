/* Project Lab4b
 * Description:  Toggle LED on/off using interrupts.
 * Author: Leila Moussa
 * Date: October 31, 2022
 * Comment: This solution makes the toggling behavior a little more responsive
            but it remains imperfect. I think the bouncing issue persists here, but since the overall behavior is better,
            there must be something else making the initial interrupt-free alternative from Lab4a perform incorrectly more often.
            Perhaps, something about the delay between pressing the button and it taking effect? Do interrupts react faster than polling?
 */

# define LED1 D0
# define BTN1 A1
# define FLASH_RATE 200

volatile bool flashEnabled = false;     // like in Lab4a, this variable determines whether the LED flashes or is turned off
                                        // it is volatile to avoid compiler optimizations which may omit changes to its value
                                        // it is global because its state must be kept between iterations of the loop() function

void toggleFlashEnabled() {             // this function is the Interrupt Service Routine (ISR)
    flashEnabled = !flashEnabled;       // it simply toggles the boolean value of flashEnabled
}

void setup() { 
    pinMode(LED1, OUTPUT);
    pinMode(BTN1, INPUT_PULLDOWN);
    attachInterrupt(BTN1, toggleFlashEnabled, RISING);  // a hardware interrupt is attached to the input at pin A1, which receives the button signal
                                                        // the interrupt is triggered whenever this signal observes a rising edge, i.e. from 0 to 1
                                                        // and calls the above function, toggleFlashEnabled, to handle it
}

void loop() {                       // the loop simply flashes the button or turns it off depending on the value of flashEnabled
    if (flashEnabled) {             // it does not have to worry about capturing the rising edge or keeping state
        digitalWrite(LED1, HIGH);   // the interrupt and its ISR take care of that
        delay(FLASH_RATE);
        digitalWrite(LED1, LOW);
        delay(FLASH_RATE);
    } else {
        digitalWrite(LED1, LOW);
    }
    
}
