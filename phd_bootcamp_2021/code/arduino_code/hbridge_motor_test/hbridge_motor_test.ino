/*
Motor via H-bridge via arduino nano. 
*/


int speedPin = 3; // H-bridge enable pin for speed control
int motor1APin = 8; // H-bridge leg 1
int motor2APin = 7; // H-bridge leg 2
int speed_value_motor1; // value for motor speed

void setup() {
    // set digital i/o pins as outputs:
    pinMode(speedPin, OUTPUT);
    pinMode(motor1APin, OUTPUT);
    pinMode(motor2APin, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    digitalWrite(LED_BUILTIN, HIGH); // status LED is always on

   // put motor in forward motion
   digitalWrite(motor1APin, LOW);
   digitalWrite(motor2APin, HIGH);

   // control the speed 0 - 255
   speed_value_motor1 = 127; // half speed
   analogWrite(speedPin, speed_value_motor1); // PWM controlled speed
}
