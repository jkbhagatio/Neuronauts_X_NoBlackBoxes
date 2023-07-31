/*
Send robootcamp for a walk.
*/

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
}

void loop() {
    //digitalWrite(LED_BUILTIN, HIGH); // status LED is always on
    /*
    // Test going in each direction at varying speeds.
    float speed_hi = 0.9;
    float speed_lo = 0.6;
    char *dirs[] = {"l0", "lf", "lb", "r0", "rf", "rb", "f0", "b0"};
    float speeds[] = {0.9, 0.6};
    int n_speeds = (int) sizeof(speeds) / sizeof(speeds[0]);
    int n_dirs = (int) (sizeof(dirs) / sizeof(dirs[0]));

    for (int i_dir = 0; i_dir < n_dirs; ++i_dir) {
        for (int i_speed = 0; i_speed < n_speeds; ++i_speed) {
            t = millis();
            set_dir(dirs[i_dir], speeds[i_speed], PWM_LEFT, PWM_RIGHT,
                    LEFT_LOG1, LEFT_LOG2, RIGHT_LOG1, RIGHT_LOG2);
            while ((millis() - t) < 5000);
            Serial.print("inner i_speed:");
            Serial.println(i_speed);
            Serial.print("inner i_dir:");
            Serial.println(i_dir);
        }
        ++i_dir;
        Serial.print("outer i_dir:");
        Serial.println(i_dir);
    }
    */
    walk();
}

// Set speed of robootcamp via pwm pins.
// Inputs: 'pwm_left' and 'pwm_right': pin numbers for pwm pins
//         'speed_left' and 'speed_right': speed as a fraction 0-1 (pwm duty cycle)                            
void set_speed(int pwm_left, int pwm_right, float speed_left, float speed_right) {
    analogWrite(pwm_left, (int) round(speed_left * PWM_RES));
    analogWrite(pwm_right, (int) round(speed_right * PWM_RES));
}

// Set direction of robootcamp via h-bridge logic pins.
// Inputs: 'dir': direction heading
//         'speed': speed of turn (motor pwm duty cycle) as a fraction 0-1
//         'dur': duration to keep this headiing (in ms)
void set_dir(String dir, float speed, int pwm_left, int pwm_right,
             int left_log1, int left_log2, int right_log1, int right_log2) {
    // For each possible direction, set logic pins appropriately:
    dir.toLowerCase();  // ensure `dir` is lowercase
    if (dir == "l0")  { // pure left
        set_speed(pwm_left, pwm_right, speed / 2, speed);
        // Go backward on left motor at a low speed, and forward on right motor at a 
        // high speed.
        digitalWrite(left_log1, LOW);
        digitalWrite(left_log2, HIGH);
        digitalWrite(right_log1, HIGH);
        digitalWrite(right_log2, LOW);
    }
    else if (dir == "lf")  { // left forward
        set_speed(pwm_left, pwm_right, speed / 2, speed);
        // Go forward on left motor at a low speed, and forward on right motor at a 
        // high speed.
        digitalWrite(left_log1, HIGH);
        digitalWrite(left_log2, LOW);
        digitalWrite(right_log1, HIGH);
        digitalWrite(right_log2, LOW);  
    }
    else if (dir == "lb")  { // left backward
        set_speed(pwm_left, pwm_right, speed / 2, speed);
        // Go backward on left motor at a low speed, and backward on right motor at a 
        // high speed.
        digitalWrite(left_log1, LOW);
        digitalWrite(left_log2, HIGH);
        digitalWrite(right_log1, LOW);
        digitalWrite(right_log2, HIGH);  
    }
    else if (dir == "r0")  { // pure right
        set_speed(pwm_left, pwm_right, speed, speed / 2);
        // Go forward on left motor at a high speed, and backward on right motor at a 
        // low speed.
        digitalWrite(left_log1, HIGH);
        digitalWrite(left_log2, LOW);
        digitalWrite(right_log1, LOW);
        digitalWrite(right_log2, HIGH);  
    }
    else if (dir == "rf")  { // right forward
        set_speed(pwm_left, pwm_right, speed, speed / 2);
        // Go forward on left motor at a high speed, and forward on right motor at a 
        // low speed.
        digitalWrite(left_log1, HIGH);
        digitalWrite(left_log2, LOW);
        digitalWrite(right_log1, HIGH);
        digitalWrite(right_log2, LOW);  
    }
    else if (dir == "rb")  { // right backward
        set_speed(pwm_left, pwm_right, speed, speed / 2);
        // Go backward on left motor at a high speed, and backward on right motor at a 
        // low speed.
        digitalWrite(left_log1, LOW);
        digitalWrite(left_log2, HIGH);
        digitalWrite(right_log1, LOW);
        digitalWrite(right_log2, HIGH);  
    }
    else if (dir == "f0")  { // pure forward
        set_speed(pwm_left, pwm_right, speed, speed);
        // Go forward on left and right motors at same speed.
        digitalWrite(left_log1, HIGH);
        digitalWrite(left_log2, LOW);
        digitalWrite(right_log1, HIGH);
        digitalWrite(right_log2, LOW);
    }
    else if (dir == "b0")  { // pure backward
        set_speed(pwm_left, pwm_right, speed, speed);
        // Go backward on left and right motors at same speed.
        digitalWrite(left_log1, LOW);
        digitalWrite(left_log2, HIGH);
        digitalWrite(right_log1, LOW);
        digitalWrite(right_log2, HIGH);
    }
    else if (dir == "00")  { // brake.
        set_speed(pwm_left, pwm_right, 0, 0);
    }
}

// Walk in a certain direction for a random number of seconds
void walk() {
    long t = millis();
    char *dirs[] = {"lf", "lb", "rf", "rb", "f0", "b0"};
    char *turns[] = {"r0", "b0"};
    float speeds[] = {0.9, 0.8, 0.7};
    String dir = dirs[random(0, 5)];
    String turn = turns[random(0,1)];
    float spd = speeds[random(0, 2)];
    int dir_dur = random(3000, 7000);
    int turn_dur = random(1000, 4000);
    set_dir(dir, spd, PWM_LEFT, PWM_RIGHT,
            LEFT_LOG1, LEFT_LOG2, RIGHT_LOG1, RIGHT_LOG2);
    while ((millis() - t) < dir_dur);
    t = millis();
    set_dir(turn, spd, PWM_LEFT, PWM_RIGHT,
            LEFT_LOG1, LEFT_LOG2, RIGHT_LOG1, RIGHT_LOG2);
    while ((millis() - t) < turn_dur);
}

// 
void get_dir() {
  
}

// If false, reset timer; if true, back up, 
// turn left or right by some amount, and continue forward again.
bool is_stuck() {

}
}
