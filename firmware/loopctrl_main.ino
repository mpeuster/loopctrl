#include <Automaton.h>
#include <ArduinoLog.h>

// +++ Pin configuration
const uint8_t MOTOR_A_PWM = 3; // Motor A: PWM
const uint8_t MOTOR_B_PWM = 9; // Motor B: PWM
const uint8_t MOTOR_A_DIR = 2; // Motor A: Direction
const uint8_t MOTOR_B_DIR = 8; // Motor B: Direction

const uint8_t PUSH_BUTTON_0 = 4; // Direction control buttons
const uint8_t PUSH_BUTTON_1 = 5;
const uint8_t PUSH_BUTTON_2 = 6;
const uint8_t PUSH_BUTTON_3 = 7;

const uint8_t LED_0 = 13; // Status indicator LED

const uint8_t POTI_0 = A0;   // Analog turn poti
const uint8_t SWITCH_0 = A1; // Mode selector switch

// TODO: A4/A5 for I2C display

// +++ Global variables
Atm_button pushButton0;
Atm_button pushButton1;
Atm_button pushButton2;
Atm_button pushButton3;

Atm_led led0;

// +++ Logging helper
void printTimestamp(Print *_logOutput)
{
  char c[12];
  int m = sprintf(c, "%10lu ", millis());
  _logOutput->print(c);
}

void printNewline(Print *_logOutput)
{
  _logOutput->print('\n');
}

void setupLogging()
{
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
  Log.setPrefix(printTimestamp);
  Log.setSuffix(printNewline);
}

// +++ Events
void onPushButtonPress(int idx, int v, int up)
{
  Log.verbose("onPushButtonPress: idx=%d v=%d up=%d", idx, v, up);
}

// +++ Setup and main loop
void setup()
{
  // Setup logging / debugging
  Serial.begin(9600);
  setupLogging();

  // Setup inputs
  pushButton0.begin(PUSH_BUTTON_0).onPress(onPushButtonPress, 0);  // TODO: add other events as needed https://github.com/tinkerspy/Automaton/wiki/The-button-machine#atm_button--onrelease-connector-connector-argument-
  pushButton1.begin(PUSH_BUTTON_1).onPress(onPushButtonPress, 1);
  pushButton2.begin(PUSH_BUTTON_2).onPress(onPushButtonPress, 2);
  pushButton3.begin(PUSH_BUTTON_3).onPress(onPushButtonPress, 3);


  led0.begin( LED_0 ).blink( 40, 250 );
  led0.trigger( led0.EVT_BLINK );


  /*

  pinMode(pwm_a, OUTPUT);  //Set control pins to be outputs
  pinMode(pwm_b, OUTPUT);
  pinMode(dir_a, OUTPUT);
  pinMode(dir_b, OUTPUT);

  pinMode(buttonPin1, INPUT_PULLUP);
  
  analogWrite(pwm_a, 0);  //pwm 0 ... 255
  analogWrite(pwm_b, 0);

  */

  Log.notice("DL1PEU - MagLoopCtrl initialized");
}

void loop()
{
  // call automaton framework
  automaton.run();
  /*
  digitalWrite(dir_a, LOW); 
  analogWrite(pwm_a, 255);  
  delay(2000);
  
  analogWrite(pwm_a, 0);  
  delay(2000);

  digitalWrite(dir_a, HIGH); 
  analogWrite(pwm_a, 255);  
  delay(2000);

  analogWrite(pwm_a, 0);  
  delay(2000);
  */

  int val;
  val = analogRead(SWITCH_0);
  Serial.println(val);

  // read the state of the pushbutton value:
  //buttonState = digitalRead(buttonPin1);

  //Serial.println(buttonState);

  delay(10);
}
