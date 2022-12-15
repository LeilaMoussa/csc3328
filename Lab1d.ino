/* Project Lab1d
 * Description:  Make both the onboard LED and external LED blink at the same time
 *               only if button 1 is pressed or if the RLS detects a presence
 * Author: Leila Moussa
 * Date: September 21, 2022
 */
# define LED0 D7                    // now we can call pin D7 by the more meaningful name 'LED0'
# define LED1 D0                    // and call pin D0 by 'LED1', to which it will be connected with a wire
# define BTN1 A1                    // Button 1 will be connected to the output of pin A1
# define RLS A4                     // Reflective Light Sensor is connected to pin A4
# define FLASH_RATE 200             // easily change LED's flash-rate

void setup() { 
    pinMode(LED0, OUTPUT);          // sets LED0 pin D7 mode to Output
    pinMode(LED1, OUTPUT);          // pin D0 will also be an Output connected to LED1
    pinMode(BTN1, INPUT_PULLDOWN);  // BTN1 is active high
    pinMode(RLS, INPUT_PULLUP);     // the RLS is active low
}

void loop()  {
    int flashEnabled = digitalRead(BTN1);   // flashEnabled is TRUE if Button 1 is pressed, FALSE otherwise
    int RLSval = digitalRead(RLS);          // RLSval is FALSE if light is detected
    if (flashEnabled || !RLSval) {          // only execute blinking logic if BTN1 is pressed OR if RLS detects light (is FALSE)
        digitalWrite(LED0, HIGH);           // turn LED0 on
        digitalWrite(LED1, HIGH);           // turn LED1 on
        delay(FLASH_RATE);                  // wait 1/5 second
        digitalWrite(LED0, LOW);            // turn LED0 off 
        digitalWrite(LED1, LOW);            // turn LED1 off 
        delay(FLASH_RATE);                  // wait another 1/5 second
    }
}
