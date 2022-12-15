/* CSC 3328 Final Project
 * Description: Burglar alarm system
 * Author: Leila Moussa
 * Date: December 15, 2022
 * Implemented features (in addition to base system)
 * i) 3 second delay
 * iii) remote arm/disarm
 * v) physical digital lock (I came up with my own key, hardcoded below)
 * vi) remote digital key
 * Note: I used a library for debouncing, which I don't think counts as feature ix.
         Heads up related to FSM_TICK: It seems that there's a bit of a small delay between entering the correct
        digital key and LED actually lighting up. Also, when I've tried entering the lock very quickly with a 
        slightly longer tick time of 1 second, it doesn't quite recognize it; so to be on the safe side,
        one should probably punch in the digital key a bit slowly.
 */
 
#include <Debounce.h>   // Particle Community Library for debouncing buttons, included in my project workspace.

# define BTN1 A1
# define BTN0 A2
# define LED1 D0
# define BUZZER A7
# define RLS A4
# define ADC A5     // I am using the Analog-Digital Converter to dim LED1

# define TONE_FREQ_PUSH 100         // The frequency in milliseconds of the tone sounded on each button push
# define TONE_FREQ_ALARM 7000       // The alarm tone frequency is more high-pitches
# define FSM_TICK 500               // The finite state machine for the overall system executes every 500 ms
# define INTRUDER_MIN_DUR 3000      // The intruder must be present for at least 3 seconds before the alarm tone sounds

enum state { OFF, ARMED, ALARM };   // The 3 different possible states of the overall system
static state currentState = OFF;    // By default, the system is off

const String digiKey = "1010";      // The digital key used for physically or remotely unlocking the system
enum seqState { A, B, C, D };       // The states of the lock FSM, respectively corresponding to the data: "", "1", "10", "101"
                                    // Given that the digital code is "1010"
static seqState seq = A;            // By default, the lock starts out with empty input
static int lockState = -1;          // Indicates whether the user is in the process of entering the digital key to arm or disarm the system
                                    // -1 means we are not in that process, and that's how we start
                                    // subsequently, the values 1, 2, 3 refer to A, B, C in seqState respectively

static volatile int btn1Sig = 0;    // The signal from BTN1
static volatile int btn0Sig = 0;    // The signal from BTN0
static volatile int rlsSig = 0;     // The signal from the RLS, 1 if something is detected, 0 otherwise
static volatile int tickFlag = 0;   // Indicates when it's time to run the system FSM

static unsigned long t0;            // Used for counting down 3s of intruder presence
static bool counting = false;       // Indicates whether we are in the process of counting down those 3 seconds

Timer fsmTimer(FSM_TICK, setTickFlag);  // Timer used to trigger the system FSM every FSM_TICK milliseconds
Debounce debouncer1 = Debounce();       // Debouncer objects, each for a button (BTN1, BTN0)
Debounce debouncer0 = Debounce();

void dimmedLight() {
    analogWrite(LED1, analogRead(ADC) / 16);    // This function is really just to make the LED light easier on my eyes :)
}

void setTickFlag() {
    tickFlag = 1;       // Timer ISR executed every FSM_TICK ms, to trigger the function fsm(), called in main loop
}

void setup() {
    pinMode(LED1, OUTPUT);                              // External LED and Buzzer receive signals from Photon
    pinMode(BUZZER, OUTPUT);
    
    debouncer1.attach(BTN1, INPUT_PULLDOWN);            // Attach debouncer objects to BTN1 and BTN0
    debouncer1.interval(20);                            // Specify debouncer interval in ms, indicating interval between 2 consecutive,
                                                        // distinct button presses
    debouncer0.attach(BTN0, INPUT_PULLDOWN);
    debouncer0.interval(20);
    
    pinMode(RLS, INPUT_PULLUP);                         // The RLS is active low
    attachInterrupt(RLS, rlsIsr, FALLING);              // Falling edge of the RLS signal indicates an intruder, to which an ISR is attached
    
    Particle.function("armSystem", remoteArm);          // Cloud functions to arm or disarm the system providing a password (digital key)
    Particle.function("disarmSystem", remoteDisarm);
    
    fsmTimer.start();                                   // Launch timer for system FSM
}

void setBtnFlag(volatile int* flag) {                   // This function sets some flag corresponding to a button press
    *flag = 1;                                          // which could be BTN1 or BTN0
    tone(BUZZER, TONE_FREQ_PUSH, 100);                  // and sounds the low frequency, shorter tone
}

void rlsIsr() {
    rlsSig = 1;                                         // ISR attached to RLS signal, sets the corresponding flag
}

int remoteArm(String cmd) {                             // Cloud function with appropriate signature
    if (cmd.compareTo(digiKey) == 0) {                  // Input is the password to be compared with the actual digital key, hardcoded above
        currentState = ARMED;                           // If match, arm the system -- the corresponding action will be executed
                                                        // on the next iteration of fsm(), as long as no other relevant signals interfere
        return 0;
    }
    return 1;                                           // return value of 1 if failure, 0 if success
}

int remoteDisarm(String cmd) {                          // Very similar function that turns the system off
    if (cmd.compareTo(digiKey) == 0) {
        currentState = OFF;
        return 0;
    }
    return 1;
}

bool lockFsm() {                    // This FSM controls the sequence of BTN0 and BTN1 presses for the digital lock ("1010")
    bool z = false;                 // Output: 1 if unlocked, 0 otherwise
    switch (seq) {                  
        case A:                     // seq == A <==> input == ""
            if (btn1Sig) {          // expect a 1, otherwise, if 0, stay in state A
                seq = B;
                lockState = 1;      // lockState goes hand in hand with seq, mostly
                                    // i could have added one more seqState for pre-unlock
                                    // and used it an integer instead of a whole new variable...
            }
            break;
        case B:                     // seq == B <==> input == "1"
            if (btn0Sig) {          // expect a 0
                seq = C;
                lockState = 2;
            } else if (btn1Sig) {   // any mismatch takes you back to state A ==> non-overlapping pattern detection
                seq = A;
                lockState = 0;
            }
            break;
        case C:                     // seq == C <==> input == "10"
            if (btn1Sig) {          // expect a 1
                seq = D;
                lockState = 3;
            } else if (btn0Sig) {   // mismatch
                seq = A;
                lockState = 0;
            }
            break;
        case D:                     // seq == D <==> input == "101"
            if (btn0Sig) {          // finally, expect a 0
                z = true;           // output is true because we have unlocked the armed/disarmed state
                seq = A;
            } else if (btn1Sig) {   // mismatch
                seq = A;
                lockState = 0;
            }
            break;
    }
    btn1Sig = 0;                    // reset relevant flags, as we have already responded to them
    btn0Sig = 0;
    return z;                       // return whether the lock was unlocked or not
}

void fsm() {                                    // The FSM controlling the overall system state
    switch (currentState) {
        case OFF:
            // When OFF, we may respond to 2 signals:
            // a) BTN1 indicating the user's wish to transition to ARMED
            // b) BTN0 as used within the digital lock sequence
            // the following condition captures both of these
            // btn1Sig may refer to the initial press before starting to enter the digital key
            // lockState != -1 means that we're currently in the process of unlocking
            // The condition allows us to respond to both BTN1 and BTN0 signals, in 2 different contexts
            // The same applies to states ARMED and ALARM, when the key is required to disarm the system
            if (btn1Sig || lockState != -1) {
                if (lockState == -1) {          // If we have not started the unlocking process yet
                                                // That means btn1Sig == HIGH
                                                // indicating the user's intention to start unlocking
                    btn1Sig = 0;                // So discard that signal first
                    seq = A;                    // make sure the lock sequence is at state A (empty input)
                    lockState = 0;              // and the lockState moves up, starting from 0, then hopefully to 1, 2, then 3
                }
                else if (lockFsm()) {           // Otherwise, if we're already in the unlocking process
                                                // execute the lock FSM based on the current state of the lock sequence
                                                // and decide whether the input matches the key
                    currentState = ARMED;       // if it does, go ahead and arm the system
                    lockState = -1;             // and reset the state of the lock for next time
                }
            }
            break;
        case ARMED:
            // The following IF statement is very similar to the previous one -- above comments apply
            if (btn1Sig || lockState != -1) {
                if (lockState == -1) {
                    btn1Sig = 0;
                    seq = A;
                    lockState = 0;
                }
                else if (lockFsm()) {
                    currentState = OFF;                     // The only difference is that we're disarming instead of arming here
                    lockState = -1;
                }
            } else if (rlsSig) {                            // When ARMED, unconditionally respond to an intruder (no lock, that is)
                if (!counting) {                            // If we haven't already started counting up to the 3 seconds, start
                    counting = true;
                    t0 = millis();                          // take note of the milliseconds elapsed since program started
                }
                if (millis() - t0 >= INTRUDER_MIN_DUR) {    // Check (on subsequent iterations) whether 3000 ms have elapsed since we
                                                            // started counting
                    currentState = ALARM;                   // If so, finally move to ALARM state
                }
            }
            break;
        case ALARM:
            // Again, this IF statement is identical to the previous one
            if (btn1Sig || lockState != -1) {
                if (lockState == -1) {
                    btn1Sig = 0;
                    seq = A;
                    lockState = 0;
                }
                else if (lockFsm()) {
                    currentState = OFF;
                    lockState = -1;
                }
            } else if (!rlsSig) {       // If the intruder leaves, move back to ARMED state
                currentState = ARMED;
            }
            break;
    }

    switch (currentState) {
        case OFF:
            digitalWrite(LED1, LOW);                        // If OFF, turn off LED1 and buzzer, in case they were on
            noTone(BUZZER);
            break;
        case ARMED:
            dimmedLight();                                  // If ARMED, turn on light (using ADC)
            noTone(BUZZER);                                 // and make sure buzzer is off
            break;
        case ALARM:
            counting = false;                               // If we have arrived in ALARM state, stop counting up to the 3 seconds
            tone(BUZZER, TONE_FREQ_ALARM, 0);               // sound a continuous, high-pitched tone
                                                            // It only sounds as long as FSM_TICK (while intruder is still there) if it's not disarmed first
            Particle.publish("intruder-detected", NULL);    // Send an email through webhook
            break;
    }

    btn1Sig = 0;                                            // reset flags
    if (digitalRead(RLS) == LOW) return;                    // but only reset RLS flag if intruder has disappeared
    rlsSig = 0;
}

void loop() {
    debouncer1.update();            // As mentioned in Debouncer docs, update the debouncer object once per loop
    debouncer0.update();            // because Debouncer does not rely on interrupts
    if (debouncer1.rose()) {        // On rising edge of BTN1, set it
        setBtnFlag(&btn1Sig);
    }
    if (debouncer0.rose()) {        // same for BTN0
        setBtnFlag(&btn0Sig);
    }
    if (tickFlag) {                 // Every FSM_TICK, run fsm
        fsm();
        tickFlag = 0;               // remember to reset flag for next tick
    }
}
