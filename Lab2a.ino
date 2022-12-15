/* Project Lab2a
 * Description:  Control brightness of LED and speed of motor with potentiometer
                 using Analog to Digital Conversion and Pulse Width Modulation
 * Author: Leila Moussa
 * Date: September 30, 2022
 */

# include <math.h>                  // required to use pow()

# define ADC A5                     // pin A5 will be used as an Analog to Digital Converter
# define LED1 D0                    // the external LED will be connected to the output of pin D0
                                    // the on-board LED would not work because D7 does not have a timer/counter required for PWM

# define ADC_RESOLUTION 12          // the photon has a 12-bit converter
# define PWM_RESOLUTION 8           // pulse width is an 8-bit value
int SCALE;                          // the factor by which a 12-bit value will be scaled down to an 8-bit one

void setup() {
    pinMode(LED1, OUTPUT);                           // pin D0 outputs a signal to the external LED
    SCALE = pow(2, ADC_RESOLUTION - PWM_RESOLUTION); // an approximation of the factor by which
                                                     // ADC output will be scaled down to PWM input
}

void loop() {
    int adcVal = analogRead(ADC);                    // read a value in the range [0-4095] from the ADC
    int scaled_down = adcVal / SCALE;                // scale it down to the range [0-255]
    analogWrite(LED1, scaled_down);                  // the scaled down value is the pulse width written to the PWM
}
