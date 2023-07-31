/*
 * Uses rotary encoder with two Hall effect sensors to get speed and direction.
 * 
 * Tests showed:
 * 1) Braking (via Hi-Hi or Lo-Lo to H-bridge logic pins) doesn't slow down motor any
 * faster than just cutting power (setting PWM to 0)
 * 2) Attaching an interrupt works about 4-5x better at detecting pulses than 
 * `digitalRead` (max sampling rate of `digitalRead` seems to be about 2.5e4 Hz)
 * (frequency of encoder = MAX_RPM / 60 * ENC_RES = 15000/60*1400 = 3.5e4 Hz)
 */

// Define input pin numbers for Hall sensors.
int r_rot_encA = 2;
int l_rot_encA = 3;
// Set placeholders for the encoders' current states and previous states.
int r_rot_encA_state;
int r_rot_encA_prev_state;
int r_rot_encB_state;
int r_rot_encB_prev_state;
// Set placeholders for Hall sensors' pulse numbers.
int l_encA_pulse_n = 0;
int *p_l_encA_pulse_n;
int r_encA_pulse_n = 0;
int r_encA_pulse_n_init = 0;
int *p_r_encA_pulse_n;
// Encoder resolution (number of pulses per revolution) from datasheet
int ENC_RES = 1400;
// PWM pins to control speed.
int PWM_LEFT = 10;
int PWM_RIGHT = 6;
int PWM_RES = 255;  // # bit resolution for pwm duty cycle
// H-bridge V logic pins to control individual motor direction:
// Hi to '1' and Lo to '2' is forward motion, and vice versa.
int LEFT_LOG1 = 12;
int LEFT_LOG2 = 11;
int RIGHT_LOG1 = 7;
int RIGHT_LOG2 = 8;

int rpm;

void setup() { 

    // Set pin modes.
    //pinMode(r_rot_encA, INPUT);
    // Use interrupt to detect pin state change.
    pinMode(l_rot_encA, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(l_rot_encA), increment_l, CHANGE);
    pinMode(r_rot_encA, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(r_rot_encA), increment_r, CHANGE);
    pinMode(PWM_LEFT, OUTPUT);
    pinMode(PWM_RIGHT, OUTPUT);
    pinMode(LEFT_LOG1, OUTPUT);
    pinMode(LEFT_LOG2, OUTPUT);
    pinMode(RIGHT_LOG1, OUTPUT);
    pinMode(RIGHT_LOG2, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
   
    Serial.begin (57600);
    p_l_encA_pulse_n = &l_encA_pulse_n;
    p_r_encA_pulse_n = &r_encA_pulse_n;
}
 
void loop() {

      // Compute n_encoder_pulses via `digitalRead()`.
//    // Read the current state of the sensors.
//    r_rot_encA_state = digitalRead(r_rot_encA);
//    r_rot_encB_state = digitalRead(r_rot_encB);
//    // If the previous and current state of sensor 'A' are different, 
//    // a pulse has occurred.
//    if (r_rot_encA_state != r_rot_encA_prev_state){     
//        // If the state of sensor 'B' is different than sensor 'A', the encoder is
//        // rotating ccw, else cw.
//        if (digitalRead(r_rot_encB_state) != r_rot_encA_state) { 
//            //
//        } else {
//            // 
//        }
//        ++encA_pulse_n;
//        Serial.print("pulse_n: ");
//        Serial.println(encA_pulse_n);
//        r_rot_encA_prev_state = r_rot_encA_state;
//    }

    // Test sampling frequency of `digitalRead`:
//    unsigned long start=micros();
//    digitalRead(2);
//    digitalRead(3);
//    digitalRead(4);
//    digitalRead(5);
//    digitalRead(6);
//    digitalRead(7);
//    digitalRead(8);
//    digitalRead(10);
//    digitalRead(11);
//    digitalRead(12);
//    unsigned long end=micros();
//    Serial.println((float) 1 / ((end-start) / 10.0));
//    while(true);

    // Compute n_encoder_pulses for one revolution via an interrupt.
//    if (encB_pulse_n > ENC_RES) {
//        //digitalWrite(LEFT_LOG1, HIGH);
//        //digitalWrite(LEFT_LOG2, HIGH);
//        analogWrite(PWM_LEFT, 0);
//        Serial.println(encB_pulse_n);
//    } else {
//        analogWrite(PWM_LEFT, 255);
//        digitalWrite(LEFT_LOG1, HIGH);
//        digitalWrite(LEFT_LOG2, LOW);
//    }

    // Get max rpm for a motor connected via an h-bridge ic.
    rpm = get_max_rpm(PWM_RIGHT, PWM_RES, RIGHT_LOG1, RIGHT_LOG2, *p_r_encA_pulse_n,
                      ENC_RES);
    //Serial.println(rpm * 2);
    //Serial.println(r_encA_pulse_n);
    //Serial.println(*p_r_encA_pulse_n);
    //print_p_int(*p_r_encA_pulse_n);
    //while(true);
}

void print_p_int(int p_int) {
    Serial.print("'print_p_int' val: ");
    Serial.println(p_int);
}

// Interrupt callbacks.
void increment_l() {
    ++*p_l_encA_pulse_n;
}

void increment_r() {
    ++*p_r_encA_pulse_n;
}

int get_max_rpm(int pwm_pin, int pwm_res, int log1_pin, int log2_pin,
                int pulse_val, int enc_res) {
    int rev_n = 0;
    // Start timer.
    unsigned long t = millis();
    // Start motor at 100% pwm duty cycle.
    analogWrite(pwm_pin, pwm_res);
    digitalWrite(log1_pin, HIGH);
    digitalWrite(log2_pin, LOW);
//    // Assumes there is an interrupt callback which increments `pulse_val` 
//    // (this should be declared in `setup()`)
//    while ((millis() - t) < 2000) {
//        Serial.println(pulse_val); 
//        //Serial.println(pulse_val);
//        if (pulse_val > enc_res) {
//        //if (l_encA_pulse_n > enc_res) {
//            ++rev_n;
//            pulse_val = 0;
//        }
//    }
    delay(2000);
    Serial.println(pulse_val);
    Serial.println(*p_r_encA_pulse_n);
    analogWrite(pwm_pin, 0);
    while(true);
    return rev_n;
}
