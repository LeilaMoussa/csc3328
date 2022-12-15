/* Project Lab4a
 * Description:  Toggle LED on/off without interrupts at first.
 * Author: Leila Moussa
 * Date: October 31, 2022
 * Comment: Though correct in principle, this code does not always result in correct behavior from the LED.
            Often, when the button is pressed only briefly, the toggle does not work. When I hold the button down
            a little longer than that upon pressing it, it tends to behave correctly a little more consistently.
            One guess is that the button, when pressed briefly, may actually bounce and double click, the equivalent of two
            toggles at a very short interval. When pressed a little more slowly or carefully, it probably does not bounce.
 */

# define LED1 D0        // external LED1 connected to pin D0
# define BTN1 A1        // button connected to pin A1
# define FLASH_RATE 200 // the rate at which the LED will flash when flash is enabled

int btnPrevStatus = LOW;    // the previous status of the button, set on each iteration, initially LOW or off
bool flashEnabled = false;  // this boolean controls whether the LED will flash or not, based on the current and previous states of the button

void setup() { 
    pinMode(LED1, OUTPUT);          // the LED received output from the photon
    pinMode(BTN1, INPUT_PULLDOWN);  // the button provides a digital input to the photon and is active high
}

void loop() {
    int btnCurrStatus = digitalRead(BTN1);                  // get the current status of the button in this iteration: HIGH or LOW
    if (btnCurrStatus == HIGH && btnPrevStatus == LOW) {    // a rising edge, i.e. a previous status of 0 and a current status of 1,
        flashEnabled = !flashEnabled;                       // entails toggling the value of flashEnabled, to turn the LED on or off
    }
    btnPrevStatus = btnCurrStatus;                          // to keep state, set the previous status to the current status on each iteration
    if (flashEnabled) {
        digitalWrite(LED1, HIGH);                           // when flash is enabled due to a rising edge in the button signal, the LED flashes
        delay(FLASH_RATE);
        digitalWrite(LED1, LOW);
        delay(FLASH_RATE);
    } else {
        digitalWrite(LED1, LOW);                            // when flash is disabled, again due to a rising edge, the LED is turned off
    }
}
