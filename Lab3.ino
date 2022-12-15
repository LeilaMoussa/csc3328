/* Project Lab3
 * Description: Control a line-following robot using a Reflective Light Sensor and an H-Bridge to
                drive 2 motors at varying directions and speeds
 * Author: Leila Moussa
 * Date: October 26, 2022
 * Answer to question 3.1.2: The motorwould spin, because A would turn CW and B CCW at the same speed.
 */

# define PWMA D1    // The speed of motor A
# define PWMB D2    // The speed of motor B
# define In1A D3    // Input 1 to motor A, as each motor accepts two In signals
# define In2A D4    // Input 2 to motor A
# define In1B D5    // Motor B is the same
# define In2B D6
# define RLS A4     // An external Reflective Light Sensor below the robot
# define ADC A5     // The photon's Analog to Digital Converter
# define LED0 D7    // The photon's onboard LED
# define LED1 D0    // An external LED on the breadboard

// Some useful constants
# define LEFT -1
# define RIGHT 1
# define CW 1       // Clockwise
# define CCW -1     // Counter clockwise

# define BASE_SPEED 50  // The initial speed of both motors
# define DIFF_SPEED 20  // The difference to be added/substracted from the speeds to turn the motor
# define MAX_PWM 255    // The value of PWM pulse widths vary between 0 and 255

void setInput(int motor, int direction, int absSpeed) { // Set the inputs and PWM of either motor
    int in1, in2;
    if (motor == LEFT) {                                // Motor A is the left one
        in1 = In1A;
        in2 = In2A;
        analogWrite(PWMA, int(MAX_PWM * absSpeed / 100));// Write the value of the pulse width scaled to the range as the speed of the left motor
    } else {                                            // Motor B is the right one
        in1 = In1B;                                     // Similarly, set both its inputs and its speed
        in2 = In2B;
        analogWrite(PWMB, int(MAX_PWM * absSpeed / 100));
    }
    if (direction == CW) {                              // Forward (Clockwise) motion
        digitalWrite(in1, HIGH);                        // From the H-Bridge data sheet, Clockwise motion entails IN1=HIGH,IN2=LOW
        digitalWrite(in2, LOW);
    } else {                                            // Backward (Counter clockwise) motion
        digitalWrite(in1, LOW);                         // From the data sheet, swap those input values for Counter clockwise motion
        digitalWrite(in2, HIGH);
    }
}

void drive(int leftSpeed, int rightSpeed) {                     // Given speeds (velocities) of both motors, determine directions and set H-Bridge inputs accordingly
    setInput(LEFT, leftSpeed >= 0 ? CW : CCW, abs(leftSpeed));  // A negative speed produces Clockwise motion and vice-versa,
    setInput(RIGHT, rightSpeed >= 0 ? CW : CCW, abs(rightSpeed));// while the absolute value of the velocity is the PWM, controling the actual speed of the motor
}

void setup() {
    pinMode(PWMA, OUTPUT);      // All 6 following inputs to the H-Bridge are output from the photon
    pinMode(PWMB, OUTPUT);
    pinMode(In1A, OUTPUT);
    pinMode(In2A, OUTPUT);
    pinMode(In1B, OUTPUT);
    pinMode(In2B, OUTPUT);
    pinMode(RLS, INPUT_PULLUP); // The relfective light sensor at the bottom of the line-following robot is active low
                                // NOTE: After having run the robot, I realize analog input does not require setting pinMode --
                                // I assume the code would still be correct if this pinMode is omitted.
    pinMode(LED0, OUTPUT);      // The LEDs used for debugging are connected to output pins of the photon
    pinMode(LED1, OUTPUT);
}

void loop() {
    int rlsVal = analogRead(RLS);   // At each iteration, get a reading from the RLS, indicating how bright it is
    int adcVal = analogRead(ADC);   // Also determine sensitivity to the light from threshold set by potentiometer in that iteration
    int leftSpeed = BASE_SPEED;     // By default, both right and left speeds are set to a constant base speed, expressed as a percentage
    int rightSpeed = BASE_SPEED;
    if (rlsVal < adcVal) {          // An RLS value lower than the threshold happens when there is a lot of light,
                                    // meaning the robot is diverting from the line towards the white space
                                    // To correct its course, we must turn it towards the left
        digitalWrite(LED1, HIGH);   // For debugging, a left turn is associated with turning on the external LED,
        digitalWrite(LED0, LOW);    // while making sure the onboard LED is off.
        rightSpeed += DIFF_SPEED;   // A left turn needs is achieved by a higher right speed and lower left speed,
        leftSpeed -= DIFF_SPEED;    // so we increment and decrement them respectively by a constant differential speed, also a percentage
    } else {                        // In contrast, a high enough RLS value indicates a barrier (not enough light), namely the black line
        digitalWrite(LED1, LOW);    // We debug the right turn by swapping which LEDs are turned on
        digitalWrite(LED0, HIGH);
        rightSpeed -= DIFF_SPEED;   // Similarly, to turn right, left speed must exceed the right speed
        leftSpeed += DIFF_SPEED;
    }
    drive(leftSpeed, rightSpeed);   // Having computed the speeds, we can set the three inputs for each motor's H-Bridge
}
