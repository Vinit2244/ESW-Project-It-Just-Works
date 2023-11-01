#define left_trigger  16
#define left_echo     17

#define right_trigger 26
#define right_echo    27

#define center_trigger 32
#define center_echo    33

#define speed_of_sound 35000

#define STOP    0
#define FORWARD 1
#define RIGHT   2
#define LEFT    3

int status = 0;

int HIGH_T = 250; // in miliseconds
int LOW_T = 20; // in miliseconds

const int leftMotor[2] = { 4, 5 };
const int rightMotor[2] = { 13, 12 };
const int maxSpeed = 1000;

float danger = 30; // cms

void set_speed(int leftSpeed, int rightSpeed)
{
  int leftCheck = leftSpeed < 0;
  analogWrite(leftMotor[leftCheck], abs(leftSpeed));
  analogWrite(leftMotor[leftCheck ^ 1], 0);

  int rightCheck = rightSpeed < 0;
  analogWrite(rightMotor[rightCheck], abs(rightSpeed));
  analogWrite(rightMotor[rightCheck ^ 1], 0);
}

float take_reading(int trig_pin, int echo_pin)
{
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);

  int time = pulseIn(echo_pin, HIGH);
  float distance = ((float)time / 1000000) * speed_of_sound;  // in cms
  return (distance / 2);
}

void stop()
{
  if (status == STOP)
  {
    // do nothing
  }
  else
  {
    set_speed(0, 0);
  }
}

void go_left(int turn_time)
{
  if (status == LEFT)
  {
    // do nothing
  }
  else
  {
    set_speed(-maxSpeed, maxSpeed);
  }
  delay(turn_time);
}

void go_right(int turn_time)
{
  if (status == RIGHT)
  {
    // do nothing
  }
  else
  {
    set_speed(maxSpeed, -maxSpeed);
  }
  delay(turn_time);
}

void go_straight()
{
  if (status == FORWARD)
  {
    // do nothing
  }
  else
  {
    set_speed(maxSpeed, maxSpeed);
  }
}

void setup() {
  for (int i = 0; i < 2; i++) 
  {
    pinMode(leftMotor[i], OUTPUT);
    pinMode(rightMotor[i], OUTPUT);
  }

  pinMode(left_trigger, OUTPUT);
  pinMode(right_trigger, OUTPUT);
  pinMode(center_trigger, OUTPUT);
  pinMode(left_echo, INPUT);
  pinMode(right_echo, INPUT);
  pinMode(center_echo, INPUT);

  Serial.begin(115200);
}

void loop() {
  float left_distance = take_reading(left_trigger, left_echo);
  float center_distance = take_reading(center_trigger, center_echo);
  float right_distance = take_reading(right_trigger, right_echo);

  Serial.print("left distance: ");
  Serial.println(left_distance);
  Serial.print("right distance: ");
  Serial.println(right_distance);
  Serial.print("center distance: ");
  Serial.println(center_distance);

  int l = 1;
  int c = 1;
  int r = 1;

  if (left_distance < danger)
  {
    l = 0;
  }

  if (center_distance < danger)
  {
    c = 0;
  }

  if (right_distance < danger)
  {
    r = 0;
  }

  int result = 4 * l + 2 * c + r;

  if (result == 0b000)
  {
    stop();
    status = STOP;
  }
  else if (result == 0b001)
  {
    go_right(HIGH_T);
    status = RIGHT;
  }
  else if (result == 0b010)
  {
    go_straight();
    status = FORWARD;
  }
  else if (result == 0b011)
  {
    go_right(LOW_T);
    status = RIGHT;
  }
  else if (result == 0b100)
  {
    go_left(HIGH_T);
    status = LEFT;
  }
  else if (result == 0b101)
  {
    // Prioritized right
    go_right(HIGH_T);
    status = RIGHT;
  }
  else if (result == 0b110)
  {
    go_left(LOW_T);
    status = LEFT;
  }
  else if (result == 0b111)
  {
    go_straight();
    status = FORWARD;
  }
}