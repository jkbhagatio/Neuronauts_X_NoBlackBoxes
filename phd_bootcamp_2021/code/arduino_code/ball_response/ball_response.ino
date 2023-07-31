/*
* This will be the arduino code that will listen to serial commands from raspberry pi
* to control robot in response to ball tracking (see `ball_tracker.py`). 
* Possible movements: forward, turn right and forward, turn left and forward, turn left in place.
*/

// PWM pins to control speed.
int PWM_LEFT = 10;
int PWM_RIGHT = 6;
int pwm_l_val = 255;
int pwm_r_val = 255;
int PWM_RES = 255;  // # bit resolution for pwm duty cycle
int PWM_MIN = 100;
int PWM_TOGGLE = 10;

// H-bridge V logic pins to control individual motor direction:
// Hi to '1' and Lo to '2' is forward motion, and vice versa.
int LEFT_LOG1 = 12;
int LEFT_LOG2 = 11;
int RIGHT_LOG1 = 7;
int RIGHT_LOG2 = 8;
long t = 0;  // timer

void setup() {
    Serial.begin(57600);  
    // Set digital i/o pins as outputs.
    pinMode(PWM_LEFT, OUTPUT);
    pinMode(PWM_RIGHT, OUTPUT);
    pinMode(LEFT_LOG1, OUTPUT);
    pinMode(LEFT_LOG2, OUTPUT);
    pinMode(RIGHT_LOG1, OUTPUT);
    pinMode(RIGHT_LOG2, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    analogWrite(PWM_LEFT, pwm_l_val);
    analogWrite(PWM_RIGHT, pwm_r_val);
}

void loop() {
    if (Serial.available() > 0) {
        char c = Serial.read();
        Serial.println(c);
        if (c == 'r') {
            forward_right();
        }
        if (c == 'l') {
            forward_left();
        }
        if (c == 'f') {
            forward();
        }
        if (c == 't') {
            turn_left();
        }
        if (c == 's') { // 's' = stop
            stopit();
        }
    }
}

void increase_speed() {
    pwm_l_val = pwm_l_val + PWM_TOGGLE;
    pwm_r_val = pwm_r_val + PWM_TOGGLE;
    // Set limit.
    if (pwm_l_val > PWM_RES) {pwm_l_val = PWM_RES;}
    if (pwm_r_val > PWM_RES) {pwm_r_val = PWM_RES;}
    analogWrite(PWM_LEFT, pwm_l_val);
    analogWrite(PWM_RIGHT, pwm_r_val);
}

void decrease_speed() {
    pwm_l_val = pwm_l_val - PWM_TOGGLE;
    pwm_r_val = pwm_r_val - PWM_TOGGLE;
    // Set limit.
    if (pwm_l_val < PWM_MIN) {pwm_l_val = PWM_MIN;}
    if (pwm_r_val < PWM_MIN) {pwm_r_val = PWM_MIN;}
    analogWrite(PWM_LEFT, pwm_l_val);
    analogWrite(PWM_RIGHT, pwm_r_val);
}

void forward() {
    analogWrite(PWM_RIGHT, PWM_RES);
    analogWrite(PWM_LEFT, PWM_RES);
    digitalWrite(LEFT_LOG1, HIGH);
    digitalWrite(LEFT_LOG2, LOW);
    digitalWrite(RIGHT_LOG1, HIGH);
    digitalWrite(RIGHT_LOG2, LOW);
}

void forward_right() {
    analogWrite(PWM_RIGHT, PWM_MIN);
    analogWrite(PWM_LEFT, PWM_RES);
    digitalWrite(LEFT_LOG1, HIGH);
    digitalWrite(LEFT_LOG2, LOW);
    digitalWrite(RIGHT_LOG1, HIGH);
    digitalWrite(RIGHT_LOG2, LOW);
}

void forward_left() {
    analogWrite(PWM_RIGHT, PWM_RES);
    analogWrite(PWM_LEFT, PWM_MIN);
    digitalWrite(LEFT_LOG1, HIGH);
    digitalWrite(LEFT_LOG2, LOW);
    digitalWrite(RIGHT_LOG1, HIGH);
    digitalWrite(RIGHT_LOG2, LOW);
}

void turn_right() {
    digitalWrite(LEFT_LOG1, HIGH);
    digitalWrite(LEFT_LOG2, LOW);
    digitalWrite(RIGHT_LOG1, LOW);
    digitalWrite(RIGHT_LOG2, HIGH);
}

void turn_left() {
    analogWrite(PWM_RIGHT, PWM_RES);
    analogWrite(PWM_LEFT, PWM_MIN);
    digitalWrite(LEFT_LOG1, LOW);
    digitalWrite(LEFT_LOG2, HIGH);
    digitalWrite(RIGHT_LOG1, HIGH);
    digitalWrite(RIGHT_LOG2, LOW);
}

void stopit() {
    digitalWrite(LEFT_LOG1, LOW);
    digitalWrite(LEFT_LOG2, LOW);
    digitalWrite(RIGHT_LOG1, LOW);
    digitalWrite(RIGHT_LOG2, LOW);
}
