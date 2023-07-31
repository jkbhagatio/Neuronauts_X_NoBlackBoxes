/* Test PID control for speed. 
 *  
 * Algo:
 * 0) Set *set_point* (desired speed: rpm), *error_tolerance* (rpm), and *PID* gains
 * 1) In a loop, do the following:
 * a) Sense *process_variable* (current speed: rpm: via encoder)
 * b) Compute *error*: *set_point* - *process_variable*
 * c) If *error* is within *error_tolerance*, do nothing, else compute *PID_err_sig*
 * and *PID_scaling_x*, and multiply *controller* (PWM) by *PID_scaling_x*)
 * 
 * Notes for manually setting PID gains:
 * 0) Set all gains to zero.
   1) Increase the P gain until the response to a disturbance is steady oscillation.
   2) Increase the D gain until the the oscillations go away 
   (i.e. it's critically damped).
   3) Repeat steps 2 and 3 until increasing the D gain does not stop the oscillations.
   4) Set P and D to the last stable values.
   5) Increase the I gain until it brings you to the setpoint with the number of 
   oscillations desired (normally zero but a quicker response can be had if you 
   don't mind a couple oscillations of overshoot).
 */

// Todo: 1) logistic scaling function to use; 2) setting `k_p`, k_i`, and `k_d`; 
// 3) debugging at each step of `loop`, and then after 2, and 3 iterations (use counter)

#include <math.h>

// Desired speed (set point, in RPM).
float l_set_pt = 75;
float r_set_pt = 75;
// Theoretical measured max RPM (empirical was ~90).
float MAX_RPM = 100;
// Measured rpm values for each motor / wheel.
float l_rpm;
float r_rpm;
// Error values (SP - PV) for each motor.
float l_err;
float r_err;
float last_l_err = 0;
float last_r_err = 0;
// Error tolerance in RPM.
float ERR_TOL = 1;
// How often to compute the error (ms).
int ERR_CALC_DUR = 500;
// Input pin numbers for Hall sensors.
int R_ROT_ENCA = 2;
int L_ROT_ENCA = 3;
// Encoder resolution (number of pulses per revolution) from datasheet.
int ENC_RES = 1400;
// PWM pins to control speed.
int PWM_LEFT = 10;
int PWM_RIGHT = 6;
// Initial PWM vals.
int l_pwm_val = 255;
int r_pwm_val = 255;
// Bit resolution for PWM duty cycle.
int PWM_RES = 255;
// Minimum PWM for motion (tested empirically).
int MIN_PWM = 80;
// H-bridge V logic pins to control individual motor direction:
// Hi to '1' and Lo to '2' is forward motion, and vice versa.
int LEFT_LOG1 = 12;
int LEFT_LOG2 = 11;
int RIGHT_LOG1 = 7;
int RIGHT_LOG2 = 8;
// Placeholders for the encoders' current states and previous states.
int r_rot_encA_state;
int r_rot_encA_prev_state;
int r_rot_encB_state;
int r_rot_encB_prev_state;
int l_rot_encA_state;
int l_rot_encA_prev_state;
int l_rot_encB_state;
int l_rot_encB_prev_state;
// Placeholders for Hall sensors' pulse numbers, and pointers to these.
unsigned long l_encA_pulse_n = 0;
unsigned long l_encA_pulse_n_loc = 0;
unsigned long *p_l_encA_pulse_n;
unsigned long r_encA_pulse_n = 0;
unsigned long r_encA_pulse_n_loc = 0;
unsigned long *p_r_encA_pulse_n;
// Output of the control function.
float pid_err_sig;
// Output of the scaling function.
float pid_scale_x;
// Max amount to scale process variable; used by scaling function.
float MAX_SCALE = 2;
// PID gains and error terms (set via trial and error).
float k_p = 0.08;
float k_i = 0;
float k_d = 0;
float p_err_sig;
float i_err_sig;
float d_err_sig;
float e = 2.71828;
float exp_out;
// Error timer (gets reset every time (abs(SP - PV) <= ERR_TOL)
unsigned long l_err_t;
unsigned long last_l_err_t;
unsigned long r_err_t;
unsigned long last_r_err_t;
int err_ct = 0;

void setup() {
    Serial.begin(57600);
    // Set pointers to Hall sensor pulse counters.
    p_l_encA_pulse_n = &l_encA_pulse_n;
    p_r_encA_pulse_n = &r_encA_pulse_n;
    // Set Hall Sensor input pins as interrupts.
    pinMode(L_ROT_ENCA, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(L_ROT_ENCA), increment_l, CHANGE);
    pinMode(R_ROT_ENCA, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(R_ROT_ENCA), increment_r, CHANGE);
    // Set PWM and H-bridge motor logic pins as outputs.
    pinMode(PWM_LEFT, OUTPUT);
    pinMode(PWM_RIGHT, OUTPUT);
    pinMode(LEFT_LOG1, OUTPUT);
    pinMode(LEFT_LOG2, OUTPUT);
    pinMode(RIGHT_LOG1, OUTPUT);
    pinMode(RIGHT_LOG2, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    // Set initial speed and direction.
    analogWrite(PWM_RIGHT, r_pwm_val);
    analogWrite(PWM_LEFT, l_pwm_val);
    digitalWrite(LEFT_LOG1, HIGH);
    digitalWrite(LEFT_LOG2, LOW);
    digitalWrite(RIGHT_LOG1, HIGH);
    digitalWrite(RIGHT_LOG2, LOW);
    // Set error timer.
    last_l_err_t = millis();
}

void loop() {
    // a) Sense *process_variable* (current speed: rpm: via encoder)
    // b) Compute *error*: *set_point* - *process_variable*
    // c) If *error* > *error_tolerance*, compute *pid_err_sig* and *pid_scale_x*,
    // and update *controller* (PWM) by multipying by *pid_scale_x*.

     get_rpm();
     get_error();
     Serial.print("l_rpm: "); Serial.println(l_rpm);
     Serial.print("r_rpm: "); Serial.println(r_rpm);
     Serial.print("l_pwm_val: "); Serial.println(l_pwm_val);
     Serial.print("r_pwm_val: "); Serial.println(r_pwm_val);
     Serial.print("l_err: "); Serial.println(l_err);
     Serial.print("r_err: "); Serial.println(r_err);
     
//     // Run PID control if error > error tolerance.
//     if (abs(l_err) > ERR_TOL) {
//         // Get current error time.
//         l_err_t = millis() - last_l_err_t;
//         run_pid_control('l');
//     } else {
//         // Reset error timer.
//         last_l_err_t = millis();
//     }
//     if (abs(r_err) > ERR_TOL) {
//         // Get current error time.
//         r_err_t = millis() - last_r_err_t;
//         run_pid_control('r');
//         // change analogWrite(PWM_RIGHT, x);
//         ++err_ct;
//         Serial.print("err_ct: "); Serial.println(err_ct);
//     } else {
//         // Reset error timer.
//         last_r_err_t = millis();
//     }
//     // Set last errors.
//     last_l_err = l_err;
//     last_r_err = r_err;
}

// Interrupt callbacks.
void increment_l() {
    ++*p_l_encA_pulse_n;
}

void increment_r() {
    ++*p_r_encA_pulse_n;
}

void get_rpm() {
    // Initialize timer.
    unsigned int t = millis();
    // Reset pointer values.
    *p_l_encA_pulse_n = 0;
    *p_r_encA_pulse_n = 0;
    // Pulse counts will update during `ERR_CALC_DUR`.
    while ((millis() - t) < ERR_CALC_DUR);
    // Convert pulse counts to rpm: (pps -> ppm -> rpm).
    l_rpm = (float) (
        ((unsigned long) l_encA_pulse_n * (1000.0 / ERR_CALC_DUR)) * 60 / ENC_RES);
    r_rpm = (float) (
        ((unsigned long) r_encA_pulse_n * (1000.0 / ERR_CALC_DUR)) * 60 / ENC_RES);
}

void get_error() {
    l_err = l_set_pt - l_rpm;
    r_err = r_set_pt - r_rpm;
}

// Computes control function output, runs it through a scaling function, and scales
// process variable by this output.
void run_pid_control(char side) {
    unsigned int pwm_pin, pwm_val, err_t, new_pwm_val;
    float err, last_err;
    if (side == 'l') {
        pwm_pin = PWM_LEFT;
        pwm_val = l_pwm_val;
        err = l_err;
        err_t = l_err_t;
        last_err = last_l_err;
    } else if (side == 'r') {
        pwm_pin = PWM_RIGHT;
        pwm_val = r_pwm_val;
        err = r_err;
        err_t = r_err_t;
        last_err = last_r_err;
    }
    p_err_sig = k_p * err;
    i_err_sig = k_i * err_t * err / 1000;  // /1000 to conver `err_t` to seconds
    d_err_sig = k_d * (err - last_err);
    pid_err_sig = p_err_sig + i_err_sig + d_err_sig;
    scale_err_term(pid_err_sig);  // outputs `pid_scale_x`
    new_pwm_val = (int) (pwm_val * pid_scale_x);
    // Ensure pwm val is within limits.
    if (new_pwm_val > PWM_RES) {
        new_pwm_val = PWM_RES; }
    if (new_pwm_val < MIN_PWM) {
        new_pwm_val = MIN_PWM; }
    // Update PWM.
    analogWrite(pwm_pin, new_pwm_val);
    if (side == 'l') {
        l_pwm_val = new_pwm_val;
        Serial.print("new l_pwm_val: "); Serial.println(new_pwm_val);
    } else if (side == 'r') {
        r_pwm_val = new_pwm_val;
        Serial.print("new r_pwm_val: "); Serial.println(new_pwm_val);
    }
}

// Computes scaling term from control function output.
void scale_err_term(float err_sig) {
    pid_scale_x = MAX_SCALE / (1 + exp(-pid_err_sig));
}
