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

Atm_led led0;

Atm_timer screenTimer;

// +++ States
enum modeState
{
  BOOT,
  MODE_0,
  MODE_A,
  MODE_B
};

enum modeState curMode;

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
  pushButton0.begin(PUSH_BUTTON_0).onPress(onPushButtonPress, 0); // TODO: add other events as needed https://github.com/tinkerspy/Automaton/wiki/The-button-machine#atm_button--onrelease-connector-connector-argument-
  pushButton1.begin(PUSH_BUTTON_1).onPress(onPushButtonPress, 1);
  pushButton2.begin(PUSH_BUTTON_2).onPress(onPushButtonPress, 2);
  pushButton3.begin(PUSH_BUTTON_3).onPress(onPushButtonPress, 3);
}

void setupOutputs()
{
  led0.begin(LED_0);
}

void boot()
{
  screenTimer.begin(100)
      .repeat(ATM_COUNTER_OFF)
      .onTimer(onUpdateScreenTimer)
      .start();
  delay(BOOT_DELAY);
  led0.on();
  curMode = MODE_0;
}

// +++ Screen

void updateScreenBoot()
{
  display.setCursor(0, 20);
  display.println("Booting...");
}

void updateScreenMode()
{
  display.setCursor(0, 20);
  display.println("Running...");
}

void updateScreen()
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);
  display.println((String)F("DL1PEU LoopCtrl ") + VERSION);

  switch (curMode)
  {
  case MODE_0:
    updateScreenMode();
    break;
  default:
    updateScreenBoot();
    break;
  }
  //display.setTextSize(3);
  //display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse'
  display.display();
}

// +++ Events
void onPushButtonPress(int idx, int v, int up)
{
  Log.verbose(F("onPushButtonPress: idx=%d v=%d up=%d" CR), idx, v, up);
}

void onUpdateScreenTimer(int idx, int v, int up)
{
  updateScreen();
}

// +++ Setup and main loop
void setup()
{
  curMode = BOOT;
  // Setup logging / debugging
  Serial.begin(9600);
  setupLogging();

  setupInputs();

  setupOutputs();

  setupDisplay();
  updateScreen();

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
