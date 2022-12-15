/* Project Lab1b
 * Description:  Make both the onboard LED and external LED blink at the same time
 * Author: Leila Moussa
 * Date: September 21, 2022
 */
# define LED0 D7                    // now we can call pin D7 by the more meaningful name 'LED0'
# define LED1 D0                    // and call pin D0 by 'LED1', to which it will be connected with a wire
# define FLASH_RATE 200             // easily change LED's flash-rate

void setup() { 
    pinMode(LED0, OUTPUT);          // sets LED0 pin D7 mode to Output
    pinMode(LED1, OUTPUT);          // pin D0 will also be an Output connected to LED1
}

void loop()  { 
    digitalWrite(LED0, HIGH);       // turn LED0 on
    digitalWrite(LED1, HIGH);       // turn LED1 on
    delay(FLASH_RATE);              // wait 1/5 second 
    digitalWrite(LED0, LOW);        // turn LED0 off 
    digitalWrite(LED1, LOW);        // turn LED1 off 
    delay(FLASH_RATE);              // wait another 1/5 second
}
