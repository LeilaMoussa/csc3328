/* Project Lab1a 
 * Description:  Make onboard LED blink
 * Author: Leila Moussa
 * Date: September 20, 2022
 */
# define LED0 D7                    // now we can call pin D7 by the more meaningful name ‘LED0’
# define FLASH_RATE 200             // easily change LED's flash-rate
void setup() { 
    pinMode(LED0, OUTPUT);          // sets LED0 pin D7 mode to Output 
}  
void loop()  { 
    digitalWrite(LED0, HIGH); // turn LED0 on 
    delay(FLASH_RATE);        // wait 1/5 second 
    digitalWrite(LED0, LOW);  // turn LED0 off 
    delay(FLASH_RATE);        // wait 1/5 second
}
