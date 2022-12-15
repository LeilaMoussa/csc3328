/* Project Lab2b
 * Description: Control brightness of external LED, frequency of buzzer, and angular position of Servo motor
                with potentiometer using Analog to Digital Conversion and Pulse Width Modulation
 * Author: Leila Moussa
 * Date: September 30, 2022
 */

# include <math.h>                  // required to use pow()

# define ADC A5                     // pin A5 will be used as an Analog to Digital Converter
# define LED1 D0                    // the external LED will be connected to the output of pin D0
                                    // the on-board LED would not work because D7 does not have a timer/counter required for PWM
# define BUZZER A7                  // the buzzer is connected to the output of pin A7 (or WKP)
# define SERVO D2                   // the Servo motor is connected to the output of pin D2

# define ADC_RESOLUTION 12          // the photon has a 12-bit converter
# define PWM_RESOLUTION 8           // pulse width is an 8-bit value
# define MAX_TONE_FREQ 20000        // tone() takes a maximum frequency of 20kHz
# define TONE_FREQ_SHIFT 20         // tone() takes a minimum frequency of 20Hz,
                                    // so a shift is required to transform a minimum adcVal of 0 to a minimum of 20
# define MAX_SERVO_ANGLE 180        // the Servo motor takes an angle value in the range [0-180]

int ADC_TO_PWM_SCALE;               // the factor by which the analog output will be scaled down to the pulse width range
int ADC_TO_TONE_SCALE;              // the factor by which the analog output will be scaled up to the tone frequency range
int ADC_TO_SERVO_SCALE;             // the factor by which the analog output will be scaled down to the Servo angle range

Servo myServo;                      // object referencing Servo motor

void setup() {
    pinMode(LED1, OUTPUT);                                              // pin D0 outputs a signal to the external LED
    pinMode(BUZZER, OUTPUT);                                            // pin A7 outputs a signals to the buzzer
    ADC_TO_PWM_SCALE = pow(2, ADC_RESOLUTION - PWM_RESOLUTION);         // an approximation of the ADC to PWM scale-down factor
    ADC_TO_TONE_SCALE = (MAX_TONE_FREQ - TONE_FREQ_SHIFT) 
                        / pow(2, ADC_RESOLUTION) - 1;                   // factor to scale up to tone frequency before adding the shift
    myServo.attach(SERVO);                                              // attach the servo motor to the output of pin D2, also setting its pinMode
    ADC_TO_SERVO_SCALE = (pow(2, ADC_RESOLUTION) - 1) / MAX_SERVO_ANGLE;// factor to scale down to Servo angle
}

void loop() {
    int adcVal = analogRead(ADC);                               // read a value in the range [0-4095] from the ADC
    int pwmPulseWidth = adcVal / ADC_TO_PWM_SCALE;              // scale it down to the range [0-255]
    analogWrite(LED1, pwmPulseWidth);                           // the scaled down value is the pulse width written to the PWM
    int toneFreq = adcVal * ADC_TO_TONE_SCALE + TONE_FREQ_SHIFT;// scale the same value to the range [0-19980] then add 20Hz
    tone(BUZZER, toneFreq, 0);                                  // the scaled and shifted value is the tone frequency
                                                                // tone duration is 0 to create a continuous tone
    int servoAngle = adcVal / ADC_TO_SERVO_SCALE;               // scale the same ADC value to the range [0-180]
    myServo.write(servoAngle);                                  // writing the angular position to the Servo motor moves it the angle specified by turning the potentiometer
}
