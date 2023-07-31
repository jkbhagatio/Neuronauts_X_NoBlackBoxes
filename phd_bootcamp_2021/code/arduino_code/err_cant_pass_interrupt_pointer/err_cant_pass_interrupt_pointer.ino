/*
 * Shows a bug where a pointer linked to an interrupt pin doesn't properly update.
 */

// Define input pin numbers for Hall sensors.
int r_rot_encA = 2;
int l_rot_encA = 3;
// Set placeholders for Hall sensors' pulse numbers.
int l_encA_pulse_n = 0;
int *p_l_encA_pulse_n;
int r_encA_pulse_n = 0;
int *p_r_encA_pulse_n;
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
    // Set pin modes.
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
    // Run motor: show that pointer to interrupt doesn't update.
    run_motor(PWM_RIGHT, PWM_RES, RIGHT_LOG1, RIGHT_LOG2, *p_r_encA_pulse_n);
}

// Interrupt callback.
void increment_l() {
    ++*p_l_encA_pulse_n;
}

void increment_r() {
    ++*p_r_encA_pulse_n;
}

void run_motor(int pwm_pin, int pwm_res, int log1_pin, int log2_pin,
                int pulse_val) {
    // Start motor at 100% pwm duty cycle.
    analogWrite(pwm_pin, pwm_res);
    digitalWrite(log1_pin, HIGH);
    digitalWrite(log2_pin, LOW);
    delay(2000);
    Serial.print("local arg pulse val: ");
    Serial.println(pulse_val);
    Serial.print("global arg pulse val: ");
    Serial.println(*p_r_encA_pulse_n);
    analogWrite(pwm_pin, 0);
    delay(2000);
}
