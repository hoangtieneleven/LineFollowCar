#include <EightLineSensor.h>

//Name-----------Pin

#define btn1      0
#define btn2      1
#define led1      13
 
#define led_on    9   //~
#define le1       3
#define le2       4
#define pwmi      6   //~
#define ri1       8
#define ri2       7
#define pwmd      5   //~
   
 
#define NUM_SENSORS             8
#define NUM_SAMPLES_PER_SENSOR  1
#define EMITTER_PIN             9
 
EightLineSensorAnalog qtra((unsigned char[]) {7, 6, 5, 4, 3, 2,1,0},NUM_SENSORS, NUM_SAMPLES_PER_SENSOR, EMITTER_PIN);
unsigned int sensorValues[NUM_SENSORS];
int proporcional=0;
int derivativo=0;
int integral=0;
int salida_pwm=0;
int proporcional_pasado=0;
int position=0; // Vi Tri
 
int velocidad=255; //Toc do toi da

float KP=0.01, KD=0.15 , KI=0.0001;
 

int line=1;// 1 White Line - 0 Black Line
int linea=0;
int flanco_color=  0 ;
int en_linea=  500 ; //deviation between white and black
int ruido= 30; //Noise

int button1=7;
int button2=7;
 
void setup()
{
    pinMode(led1,OUTPUT);
    pinMode(led_on,OUTPUT);
    pinMode(le1,OUTPUT);
    pinMode(le2,OUTPUT);
    pinMode(pwmi,OUTPUT);
    pinMode(ri1,OUTPUT);
    pinMode(ri2,OUTPUT);
    pinMode(pwmd,OUTPUT);
 
    digitalWrite(led1,HIGH);
    delay(200);
    digitalWrite(led1,LOW);

    while(true)
     {
        digitalWrite(led1, HIGH);
        buttones();
        if(button2==0) 
        {
          delay(50);
          break;
        }
        delay(80);
        digitalWrite(led1, LOW);
        delay(80);
     }
    
    digitalWrite(led1, HIGH);  
    
    for (int i = 0; i < 300; i++)
    {
      qtra.calibrate(); 
    }
    digitalWrite(led1, LOW); 
 
     while(true)
     {
        buttones();
        if(button2==0) 
        {
          delay(20);
          digitalWrite(led1,HIGH);
          delay(100);
          digitalWrite(led1,LOW);
          delay(100);
          break;
        }
     }
   
    Serial.begin(115200);
 
}
 
void loop()
{
    pid(line,velocidad,KP,KI,KD);
    frenos_contorno(600);
 
    Serial.println(position);
    delay(2);
}
 
void pid(int linea, int velocidad, float Kp, float Ki, float Kd)
{
    
   position = qtra.readLine(sensorValues,QTR_EMITTERS_ON, linea, flanco_color, en_linea, ruido);
  proporcional = (position) - 3500; 
  integral=integral + proporcional_pasado;
  derivativo = (proporcional - proporcional_pasado);
  int ITerm=integral*KI;
  if(ITerm>=255) ITerm=255;
  if(ITerm<=-255) ITerm=-255;
   
  salida_pwm =( proporcional * KP ) + ( derivativo * KD )+(ITerm);
   
  if (  salida_pwm >velocidad )  salida_pwm = velocidad;
  if ( salida_pwm  <-velocidad )  salida_pwm = -velocidad;
   
  if (salida_pwm < 0)
 {
    int der=velocidad-salida_pwm; //(+)
    int izq=velocidad+salida_pwm;  //(-)
    if(der>=255)der=255;
    if(izq<=0)izq=0;
    motor_run(izq, der);
 }
 if (salida_pwm >0)
 {
  int der=velocidad-salida_pwm; //(-)
  int izq=velocidad+salida_pwm; //(+)
   
  if(izq >= 255) izq=255;
  if(der <= 0) der=0;
  motor_run(izq ,der );
 }
 
 proporcional_pasado = proporcional;  
}
 
 
void frenos_contorno(int flanco_comparacion)
{
    if (position <=10)
    {
      while(true)
      { 
        digitalWrite(led1,HIGH);
        motor_run(-125,60);
        qtra.read(sensorValues);
        if ( sensorValues[0]<flanco_comparacion || sensorValues[1]<flanco_comparacion || sensorValues[2]<flanco_comparacion || sensorValues[3]<flanco_comparacion || sensorValues[4]<flanco_comparacion || sensorValues[5]<flanco_comparacion || sensorValues[6]<flanco_comparacion || sensorValues[7]<flanco_comparacion)
        {
          break;
        }
         
      }
    }
 
    if (position>=6990)
    {
      while(true)
      {
        digitalWrite(led1,HIGH);
        motor_run(60,-125); 
        qtra.read(sensorValues);
        if (sensorValues[7]<flanco_comparacion || sensorValues[6]<flanco_comparacion|| sensorValues[5]<flanco_comparacion || sensorValues[4]<flanco_comparacion || sensorValues[3]<flanco_comparacion || sensorValues[2]<flanco_comparacion || sensorValues[1]<flanco_comparacion|| sensorValues[0]<flanco_comparacion)
        {
          break;
        }
      }
  }
  digitalWrite(led1,LOW);
}
 
 
void motor_run(int motor_left, int motor_right)
{
  if ( motor_left >= 0 )  
  {
    digitalWrite(le1,LOW);
    digitalWrite(le2,HIGH); 
    analogWrite(pwmi,motor_left); 
  }
  else
  {
    digitalWrite(le1,HIGH); 
    digitalWrite(le2,LOW);
    motor_left = motor_left*(-1); 
    analogWrite(pwmi,motor_left);
  }
 
  if ( motor_right >= 0 )
  {
    digitalWrite(ri1,LOW);
    digitalWrite(ri2,HIGH);
    analogWrite(pwmd,motor_right);
  }
  else
  {
    digitalWrite(ri1,HIGH);
    digitalWrite(ri2,LOW);
    motor_right= motor_right*(-1);
    analogWrite(pwmd,motor_right);
  }
}
 
void buttones()
{
    button1=digitalRead(btn2);
    button2=digitalRead(btn1); 
}
