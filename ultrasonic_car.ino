//Bluetooth
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 9); // RX, TX
String command = "";
int val;
//

//Motores
int pinA1 = 5; //right1
int pinA2 = 6; //right2
int pinB1 = 10; //left1
int pinB2 = 11; //left2

//Switch
int pinDipSwitch1 = 13;
int pinDipSwitch2 = 12;

//Obstaculos e servo motor
#include <Servo.h> // importar a biblioteca de servo motor
#include <Ultrasonic.h> // importar a biblioteca do sensor ultrassonico
#define pinoRecep A0 // definir pino de recepcao do sensor ultrassonico Echo
#define pinoEmiss A1 // definir pino de emissão do sensor ultrassonico Trig
Servo servo; // cria um obejeto Servo
Ultrasonic ultra(pinoEmiss, pinoRecep); // criando um objeto ultrassonico

#define SensorLeft    3   // Sensor Infra vermelhos esquerda
#define SensorMiddle  4  // Sensor Infra vermelhos meio
#define SensorRight   7  // Sensor Infra vermelhos direita

void setup() {
  //Controladores dos Motores
  pinMode (pinA1, OUTPUT);
  pinMode (pinA2, OUTPUT);
  pinMode (pinB1, OUTPUT);
  pinMode (pinB2, OUTPUT);

  //Switch
  pinMode (pinDipSwitch1, INPUT);
  pinMode (pinDipSwitch2, INPUT);

  pinMode(SensorLeft,INPUT);
  pinMode(SensorMiddle,INPUT);
  pinMode(SensorRight,INPUT);
    
  //Bluetooth
  Serial.begin(115200);
  mySerial.begin(9600);
  //

  //Obstaculos e servo motor
  pinMode(pinoEmiss, OUTPUT);
  pinMode(pinoRecep, INPUT);
  servo.attach(8); // Colocar servo associado a porta 
  servo.write(90); // Servo.write serve para colocar o angulo que queremos no servo motor
  delay(3000);
}

// movimento do robot
void frente() {
  motorAfrente();
  motorBfrente();
}

void tras() {
  motorAtras();
  motorBtras();
}

void parar() {
  motorAparar();
  motorBparar();
}

void direita() {
  motorAtras();
  motorBfrente();
}
void esquerda() {
  motorAfrente();
  motorBtras();
}

//movimento individual dos motores
void motorAfrente() {
  digitalWrite (pinA1, HIGH);
  digitalWrite (pinA2, LOW);
}
void motorBfrente() {
  digitalWrite (pinB1, HIGH);
  digitalWrite (pinB2, LOW);
}
void motorAtras() {
  digitalWrite (pinA1, LOW);
  digitalWrite (pinA2, HIGH);
}
void motorBtras() {
  digitalWrite(pinB1, LOW);
  digitalWrite(pinB2, HIGH);
}
void motorAparar() {
  digitalWrite (pinA1, HIGH);
  digitalWrite (pinA2, HIGH);
}
void motorBparar() {
  digitalWrite (pinB1, HIGH);
  digitalWrite (pinB2, HIGH);
}

void loop() {
  servo.write(90);
  delay(20);
  int dip1 = digitalRead(pinDipSwitch1);
  int dip2 = digitalRead(pinDipSwitch2);
  // Ler os switches para verificar qual modo e que o carro vai utilizar
  if (dip1 == 1 && dip2 == 1) {
    
      //UTILIZAR SENSOR ULTRA SONS
      servo.write(90);
      delay(100);
      //-----------------------------------
      // Aqui mexe-se o sensor de ultra sons ligeiramente para os lados para detetar
      // mais coisas que estejam a frente.
      // Caso encontre para logo e calcula o melhor caminho com a funcao busca();
      
      servo.write(112);
      delay(150);
      int distanciaesquerda = ultra.Ranging(CM);
      if(distanciaesquerda < 50) {
          busca();
          return;
      }
      servo.write(93);
      delay(150);
      int distancia = ultra.Ranging(CM); 
      if(distancia < 50) {
          busca();
          return;
      }
      servo.write(67);
      delay(150);
      int distanciadireita = ultra.Ranging(CM);
      if (distanciadireita < 50) {
          busca();
          return;
      }

      //-----------------------------------
      
      frente();
  } else if (dip1 == 0 && dip2 == 1) {
      delay(120);
      // UTILIZAR SENSOR DE CORES
      int SL = digitalRead(SensorLeft);
      delay(20);
      int SM = digitalRead(SensorMiddle);
      delay(20);
      int SR = digitalRead(SensorRight);

      // Esta em cima da linha preta
       if (SL == 1 && SM == 1 && SR == 1)
          frente();
         
       // Esta a ir para a esquerda logo temos de o mandar para a direita
       if (SL == 1 && SM == 0 && SR == 0)
          direita();
          
       // Esta a ir para a direita logo temos de o mandar para a esquerda
       if (SL == 0 && SM == 0 && SR == 1)
          esquerda();
          
  } else if (dip1 == 1 && dip2 == 0) {
  
      //bluetooth
      if (mySerial.available()) 
      {  
         while(mySerial.available()) 
         { // While there is more to be read, keep reading.  
           command += (char)mySerial.read();  
         }
         
         // Ver os comandos recebidos do modulo bluetooth
          if (command == "1"){
              frente();
          } else if (command == "2") {
              tras();
          } else if (command == "3") {
              esquerda();
          } else if (command == "4") {
              direita();
          } else {
              parar();
          }
          command = ""; // Limpar a variavel
      }

  } else {
      parar();
      return;
  }
  
}

// função busca (vai procurar por um caminho sem obstáculos)
void busca() {
  parar();
  delay(500);
  servo.write(175); // servo vira para 15 graus
  delay(500);
  int distanciaesquerda = ultra.Ranging(CM);
  servo.write(17); // servo vira para 15 graus 
  delay(500);
  int distanciadireita = ultra.Ranging(CM);
  servo.write(93);
  delay(250);
  if (distanciadireita > distanciaesquerda && distanciadireita > 30){
      tras();
      delay(150);   
      direita();
      delay(300); 
      //Serial.println("Virou a direita!");
  } else if (distanciadireita <= distanciaesquerda && distanciaesquerda > 30){
      tras();
      delay(150);   
      esquerda();
      delay(300);
      //Serial.println("Virou a esquerda!");
  } else {
      tras();
      delay(250);  
      //Serial.println("Tras!");
  }
  parar();
}

