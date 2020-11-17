int pwm_a = 3;  //PWM control for motor outputs 1 and 2 
int pwm_b = 9;  //PWM control for motor outputs 3 and 4 
int dir_a = 2;  //direction control for motor outputs 1 and 2 
int dir_b = 8;  //direction control for motor outputs 3 and 4 

void setup()
{
  pinMode(pwm_a, OUTPUT);  //Set control pins to be outputs
  pinMode(pwm_b, OUTPUT);
  pinMode(dir_a, OUTPUT);
  pinMode(dir_b, OUTPUT);
  
  analogWrite(pwm_a, 0);  //pwm 0 ... 255
  analogWrite(pwm_b, 0);
  
}

void loop()
{
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
  

}
