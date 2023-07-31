/*
 * Why doesn't this work: 
 * 1) Create an int, `a`, and int pointer, `*p_a`, and in `void setup()` assign the
 * pointer to the int: `p_a = &a`;`
 * 2) Create a function, `void print_p_int(int *p_int)`, that prints the value `*p_int`
 * points to.
 * 3) In `void loop()` increment the value `*p_a` points to, and call `print_p_int(*p_a)`:
 * why doesn't `print_p_int` behave as expected? -> because `print_p_int()` shouldn't take
 * in a pointer, it should take in the value the pointer points to.
 */

/*
int a = 0;
int *p_a;

void setup() {
  Serial.begin(57600);
  p_a = &a;
}

void loop() {
    ++*p_a;
    print_p_int(*p_a);
    Serial.print("loop val: ");
    Serial.println(a);
    delay(1000);
}

void print_p_int(int p_int) {
    Serial.print("'print_p_int' val: ");
    Serial.println(p_int);
}
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
int l_encA_pulse_n_init = 0;
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

void setup() {
    Serial.begin(57600);
    pinMode(l_rot_encA, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(l_rot_encA), increment_l, CHANGE);
    pinMode(r_rot_encA, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(r_rot_encA), increment_r, CHANGE);
    p_l_encA_pulse_n = &l_encA_pulse_n;
    p_r_encA_pulse_n = &r_encA_pulse_n;
}

void loop() {
    if (r_encA_pulse_n != r_encA_pulse_n_init) {
        Serial.print("loop val: ");
        Serial.println(r_encA_pulse_n);
        print_p_int(*p_r_encA_pulse_n);
        r_encA_pulse_n_init = r_encA_pulse_n;
    }
    //delay(2000);
    //Serial.println(r_encA_pulse_n);
    //Serial.println(r_encA_pulse_n_init);
}

// Interrupt callbacks.
void increment_l() {
    ++*p_l_encA_pulse_n;
}

void increment_r() {
    ++*p_r_encA_pulse_n;
}

void print_p_int(int p_int) {
    Serial.print("'print_p_int' val: ");
    Serial.println(p_int);
}
