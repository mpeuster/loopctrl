#include <Automaton.h>
#include <ArduinoLog.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define VERSION "v0.01"
#define BOOT_DELAY 2000

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

// +++ Display configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// +++ Global variables
Atm_button pushButton0;
Atm_button pushButton1;
Atm_button pushButton2;
Atm_button pushButton3;

Atm_analog poti0;
Atm_analog switch0;

Atm_led led0;

Atm_timer screenTimer;

// +++ States
enum modeState
{
  MODE_BOOT,
  MODE_0,
  MODE_A,
  MODE_B
};

struct GlobalStatus
{
  enum modeState mode;
  uint8_t acceleration;
  bool moveDown;
  bool moveUp;
  bool moveFast;
};

GlobalStatus curState;

void setupLogging()
{
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
}

void setupDisplay()
{
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    led0.blink(1000, 1000).start();
    Log.fatal(F("SSD1306 display driver allocation failed"));
    for (;;)
    {
      automaton.run(); // stay in failed state
    }
  }

  display.clearDisplay();
  display.setCursor(0, 0);
}

void setupInputs()
{
  pushButton0.begin(PUSH_BUTTON_0)
      .onPress(onPushButtonPress, 0)
      .onRelease(onPushButtonRelease, 0);
  pushButton1.begin(PUSH_BUTTON_1)
      .onPress(onPushButtonPress, 1)
      .onRelease(onPushButtonRelease, 1);
  pushButton2.begin(PUSH_BUTTON_2)
      .onPress(onPushButtonPress, 2)
      .onRelease(onPushButtonRelease, 2);
  pushButton3.begin(PUSH_BUTTON_3)
      .onPress(onPushButtonPress, 3)
      .onRelease(onPushButtonRelease, 3);

  poti0.begin(POTI_0, 100)
      .onChange(onAccelerationPotiChange, 0);
  switch0.begin(SWITCH_0, 100)
      .onChange(onModeSwitchChange, 0);
}

void setupOutputs()
{
  led0.begin(LED_0);
  pinMode(MOTOR_A_PWM, OUTPUT);
  pinMode(MOTOR_A_DIR, OUTPUT);
  pinMode(MOTOR_B_PWM, OUTPUT);
  pinMode(MOTOR_B_DIR, OUTPUT);

  analogWrite(MOTOR_A_PWM, 0);
  analogWrite(MOTOR_B_PWM, 0);
}

void boot()
{
  // initialize state
  curState.mode = MODE_BOOT;
  curState.acceleration = 0;
  curState.moveDown = false;
  curState.moveUp = false;
  curState.moveFast = false;

  // outputs
  updateScreen();
  screenTimer.begin(100)
      .repeat(ATM_COUNTER_OFF)
      .onTimer(onUpdateScreenTimer)
      .start();
  delay(BOOT_DELAY);
  led0.on();
  curState.mode = MODE_0;
}

// +++ Screen

void updateScreenBoot()
{
  display.setCursor(0, 20);
  display.println("Booting...");
}

void updateScreenMode(const char *mode)
{

  display.setTextSize(2);
  display.setCursor(0, 16);
  display.println(mode);
  display.setCursor(90, 16);
  char tmp[4];
  if (sprintf(tmp, "%03d", curState.acceleration))
    display.println(tmp);

  if (curState.moveDown || curState.moveUp)
  {
    display.fillRect(30, 16, 44, 20, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK); //inverse
    display.setTextSize(1);
    display.setCursor(34, 21);
    if (curState.moveDown && !curState.moveFast)
      display.println(" DOWN");
    if (curState.moveDown && curState.moveFast)
      display.println("F-DOWN");
    if (curState.moveUp && !curState.moveFast)
      display.println("  UP");
    if (curState.moveUp && curState.moveFast)
      display.println(" F-UP");
  }
}

void updateScreen()
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.println((String)F("DL1PEU LoopCtrl ") + VERSION);
  display.drawLine(0, 9, display.width() - 1, 9, SSD1306_WHITE);

  switch (curState.mode)
  {
  case MODE_0:
    updateScreenMode("-");
    break;
  case MODE_A:
    updateScreenMode("A");
    break;
  case MODE_B:
    updateScreenMode("B");
    break;
  default:
    updateScreenBoot();
    break;
  }
  display.display();
}

void triggerMotorAction()
{
  if (curState.mode != MODE_A && curState.mode != MODE_B)
    return;

  uint16_t acc = 0;
  if (curState.moveUp || curState.moveDown)
  {
    acc = 255;
    if (!curState.moveFast)
      acc = curState.acceleration;
  }

  uint8_t mDir = MOTOR_A_DIR;
  uint8_t mPwm = MOTOR_A_PWM;

  if (curState.mode == MODE_B)
  {
    mDir = MOTOR_B_DIR;
    mPwm = MOTOR_B_PWM;
  }

  if (curState.moveUp)
    digitalWrite(mDir, HIGH);
  else
    digitalWrite(mDir, LOW);

  analogWrite(mPwm, acc);
}

// +++ Events
void onPushButtonPress(int idx, int v, int up)
{
  //Log.verbose(F("onPushButtonPress: idx=%d v=%d up=%d" CR), idx, v, up);
  switch (idx)
  {
  case 0:
    curState.moveDown = true;
    break;
  case 1:
    curState.moveDown = true;
    curState.moveFast = true;
    break;
  case 2:
    curState.moveUp = true;
    break;
  case 3:
    curState.moveUp = true;
    curState.moveFast = true;
    break;
  }
  triggerMotorAction();
}

void onPushButtonRelease(int idx, int v, int up)
{
  //Log.verbose(F("onPushButtonRelease: idx=%d v=%d up=%d" CR), idx, v, up);
  curState.moveDown = false;
  curState.moveUp = false;
  curState.moveFast = false;
  triggerMotorAction();
}

void onAccelerationPotiChange(int idx, int v, int up)
{
  //Log.verbose(F("onPotiChange: idx=%d v=%d up=%d" CR), idx, v, up);
  curState.acceleration = 254 - (uint8_t)(v / 1024.0 * 255);
}

void onModeSwitchChange(int idx, int v, int up)
{
  //Log.verbose(F("onModeSwitchChange: idx=%d v=%d up=%d" CR), idx, v, up);
  if (v < 100)
    curState.mode = MODE_0;
  else if (v < 700)
    curState.mode = MODE_A;
  else
    curState.mode = MODE_B;
}

void onUpdateScreenTimer(int idx, int v, int up)
{
  updateScreen();
}

// +++ Setup and main loop
void setup()
{
  // Setup logging / debugging
  Serial.begin(9600);
  setupLogging();

  setupInputs();

  setupOutputs();

  setupDisplay();
  boot();

  /*
  pinMode(MOTOR_A_PWM, OUTPUT);  //Set control pins to be outputs
  pinMode(MOTOR_A_DIR, OUTPUT);
*/

  Log.notice(F("DL1PEU - MagLoopCtrl %s initialized" CR), VERSION);
}

void loop()
{
  // call automaton framework
  automaton.run();

  /*
  digitalWrite(MOTOR_A_DIR, LOW); 
  analogWrite(MOTOR_A_PWM, 255);  
  delay(2000);
  
  analogWrite(MOTOR_A_PWM, 0);  
  delay(2000);

  digitalWrite(MOTOR_A_DIR, HIGH); 
  analogWrite(MOTOR_A_PWM, 255);  
  delay(2000);

  analogWrite(MOTOR_A_PWM, 0);  
  delay(2000);
  */

  /*
  int val;
  val = analogRead(SWITCH_0);
  Serial.println(val);
*/
  // read the state of the pushbutton value:
  //buttonState = digitalRead(buttonPin1);

  //Serial.println(buttonState);

  //delay(10);
}
