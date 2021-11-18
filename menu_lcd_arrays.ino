/////////librerias//////////////
///#include <LiquidCrystal.h>//sin i2c
///i2c
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
///////////////////////////DECLARACION DE FUNCIONES///////////////////////
void setup();////NO COMENT
void loop();///IDEM
void llenarpantalla();///escribo las dos lineas
void mostrarlinea(int lineamenu);////llamo a las de abajo, solo para dividir el switch case
void menuinicio(int lineamenu);///led1,led2,led3,todos 1 seg,todos 0.5 seg,Todos,distintos valores
void submenu1(int lineamenu);///on//off/brillo//volver
void submenu2(int lineamenu);///on//off/brillo//volver
void submenu3(int lineamenu);///on//off/brillo//volver
void submenu4(int lineamenu);///on//off/brillo//volver
void mirarboton();///
void hacer_caso_boton(); ///esto se hara cuando pulse el boton
void ClockChanged();////////////MIRA HACIA DONDE GIRA EL ENCODER Y CAMBIA VARIABLE DE MOVERSE POR MENUS
void parpadeos(byte indice_led,byte indice_intervalo);////////////CONTROLA TODOS LOS PARPADEOS DE LOS LEDS
void todos_05_seg();/////////////finiquitat/////////////500
void todos_1_seg(); /////////////finiquitat/////////////1000
void cuatro_a_la_vez(); /////////////finiquitat/////////////
void todos_off();//////claro como el agua
void sacar_variables_serial();/////salida por serial formateada para leer tranquilamente  
/*
Estructura programa
el scroll mueve una variable,posscroll, segun ella se muentran las cosas en pantalla
cuando se pulsa el boton, posscroll pasa su valor a elementopulsado para asi actuar en consecuancia
*/
///////////////LCD///////////////////////
///const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
//const int rs = A0, en = A1, d4 = A2, d5 = A3, d6 = A4, d7 = A5;///por si necesitas pines
//const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;////df robot lcd shield
//LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
//Crear el objeto lcd  dirección  0x27 y 16 columnas x 2 filas
LiquidCrystal_I2C lcd(0x27,16,2);  //

///ROTARY ENCODER//////////////
byte CLK = 2;//CLK->D2
byte DT = 3;//DT->D3
byte SW = 4;//SW->D4 
///En Arduino y Nano se dispone de dos interrupciones, 0 y 1, asociados a los pines digitales 2 y 3
const byte interrupt0 = 0;// Interrupt 0 在 pin 2 上

 /////////////minimos y maximos del menu////////////////////
byte minmax[5][2] = {///para ponerle limites al encoder
  {00, 07},
{10, 13},
{20, 23},
{30, 33},
{40,43}};////por legibilidad///

///////////////EL MENU////////////////////////
byte submenu=0;///ANDE ANDO
byte posscroll = 0;///////////donde estoy en el menu()
byte ultimoclick = 50;////antirrebote de pulsaciones de boton
int elementopulsado = -1;/////si he pulsado algo esto lo hara indefinidamente

///////////////pines ledes/////////////////
//pwm  3, 5, 6, 9, 10 y 11.
byte ledes[] = {6,9,10,11};///numero de los pines

//////////////////ESTADOS LEDES//////////////////////
boolean estadoledes[] = {false,false,false,false};
boolean estadotodos= false;
boolean ajustandoled = false;///metido en encoder (ClockChanged) y en ajustarbrilloled 
byte numeroled = 0;//// se guardara indide de ledes
byte brilloledes[] = {0,0,0,0};//0-255////////valores pwm de brillo de los leds
////tiempos
int intervalos[] = {500,1000,1500,2000};

///variables////////     ////////son millis()
unsigned long antes[] = {0,0,0,0};
unsigned long ahora = 0;

void setup() {
    ///lcd.begin(16, 2);
    // Inicializar el LCD
  lcd.init();  
  //Encender la luz de fondo.
  lcd.backlight();  
  // Escribimos el Mensaje en el LCD.
  ///lcd.print("Hola Mundo");
  
  ////ENCODER
  pinMode(SW, INPUT);
  digitalWrite(SW, HIGH);
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  attachInterrupt(interrupt0, ClockChanged,LOW);//Set the interrupt 0 handler, trigger level change
  ///los leds
  for(int i =0;i<4;i++){
  pinMode(ledes[i],OUTPUT);
  }
  Serial.begin(9600);
  Serial.println("Estoy en el setup");
}

void loop() {
    ///////////PRIMERO MOSTRAR EN PANTALLA LO QUE TOQUE///////////////
  if(!ajustandoled){llenarpantalla();}
  else{ajustarbrilloled(numeroled);}  
  /////luego toca ver si he pulsado el booton
  mirarboton();
   ////y ahora actuar///// 
   if(!(elementopulsado>=4&&elementopulsado<10)){///si elementopulsado NO esta en el rango 4-10
    for(int i=0;i<4;i++){analogWrite(ledes[i],brilloledes[i]);}  
    }else{hacer_lo_que_se_pulso();}
}
void llenarpantalla(){
    // set the cursor to column 0, line 1 (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(1, 0);///desplazo algo a la derecha el elemento seleccionado
  mostrarlinea(posscroll);
  lcd.print("            ");
  lcd.setCursor(0, 1);

  int aux = posscroll / 10; ////fila para minmax
  if (posscroll >= minmax[aux][1]) {
    ////no imprimo nada; puede quedar la de abajo vacia///
  } else {
    mostrarlinea(posscroll + 1); ////la siguiente
  }/*  lcd.print("  puls");  lcd.print(elementopulsado);*/
  lcd.print("                   ");////////quito restos de caracteres viejos
}
void mostrarlinea(int lineamenu) {////llamo a las de abajo, solo para dividir el switch case
  //Serial.print("mostrarlinea  ");
  //Serial.println(lineamenu);
  if (lineamenu >= 0 && lineamenu< 10) {
    menuinicio(lineamenu);
  } else if (lineamenu >= 10 && lineamenu< 20) {
    submenu1( lineamenu);
  } else if (lineamenu >= 20 && lineamenu< 30) {
    submenu2(lineamenu);
  } else if (lineamenu >= 30 && lineamenu< 40){
  submenu3(lineamenu);
  } else if (lineamenu >= 40 && lineamenu< 50){
  submenu4(lineamenu);
  } else {}
}
void menuinicio(int lineamenu) {///led1,led2,led3,led4,todos 1 seg,todos 0.5 seg,Todos,distintos valores
  submenu = 0;
  ///Serial.print("menuinicio  ");
  ///Serial.println(lineamenu);
  switch (lineamenu) {
    case 0:
      lcd.print("LED 1");
      break;
    case 1:
      lcd.print("LED 2");
      break;
    case 2:
      lcd.print("LED 3");
      break;
    case 3:
      lcd.print("LED 4");
      break;
    case 4:
      lcd.print("Todos 0.5 seg");
      break;
    case 05:
      lcd.print("Todos 1 seg");
      break;
    case 06:
      lcd.print("Todos,distintos valores");
      break;
    case 07:
      lcd.print("Todos off");
      break;
    default:
      // statements
      break;
  }
}
void submenu1(int lineamenu) {///on//off/brillo//volver
  ///Serial.println("submenu1");
  submenu = 1;
  switch (lineamenu) {
    case 10:
      lcd.print("ON");
      break;
    case 11:
      lcd.print("OFF");
      break;
    case 12:
      lcd.print("BRILLO  ");
      lcd.print(brilloledes[0]);
      break;
    case 13:
      lcd.print("VOLVER");
      break;
    default:
      // statements
      break;
  }
}
void submenu2(int lineamenu) {///on//off/brillo//volver
  submenu = 2;
  switch (lineamenu) {
    case 20:
      lcd.print("ON");
      break;
    case 21:
      lcd.print("OFF");
      break;
    case 22:
      lcd.print("BRILLO  ");
      lcd.print(brilloledes[1]);
      break;
    case 23:
      lcd.print("VOLVER");
      break;
    default:
      // statements
      break;
  }
}
void submenu3(int lineamenu) {///on//off/brillo//volver
  submenu = 3;
  switch (lineamenu) {
    case 30:
      lcd.print("ON");
      break;
    case 31:
      lcd.print("OFF");
      break;
    case 32:
      lcd.print("BRILLO  ");
      lcd.print(brilloledes[2]);
      break;
    case 33:
      lcd.print("VOLVER");
      break;
    default:
      // statements
      break;
  }
}
void submenu4(int lineamenu) {///on//off/brillo//volver
  submenu = 4;
  switch (lineamenu) {
    case 40:
      lcd.print("ON");
      break;
    case 41:
      lcd.print("OFF");
      break;
    case 42:
      lcd.print("BRILLO  ");
      lcd.print(brilloledes[3]);
      break;
    case 43:
      lcd.print("VOLVER");
      break;
    default:
      // statements
      break;
  }
}
void ajustarbrilloled(byte indice_led){///////////FINIQUITAT//////
  ajustandoled = true;
  numeroled = indice_led; 
  lcd.setCursor(0, 1);///desplazo algo a la derecha el elemento seleccionado
  lcd.print("AJUSTANDO LED :");
  lcd.print(numeroled+1);
  lcd.setCursor(1, 0);
  lcd.print("INTENSIDAD:");
  lcd.print(brilloledes[indice_led]);
}

////////////////////////el boton///////////////////////////////
void mirarboton(){/////////////ARREGLADO
    if (!digitalRead(SW) && (ultimoclick != posscroll)) //leo el boton y asigno posscroll a elementopulsado
  {
    ultimoclick = posscroll;
    Serial.print("Elemento pulsado: ");
    Serial.println(ultimoclick);
    sacar_variables_serial();
    hacer_caso_boton();
  }      
    if(!digitalRead(SW) && ajustandoled){/// solo cuando ajustandoled = true
      ajustandoled=false;///SALGO DE AHI
        ///elementopulsado=-1;
        posscroll=submenu-1;
        ultimoclick = posscroll;
        delay(200);
        }else{}
}
void hacer_caso_boton() { ///esto se hara cuando pulse el boton
  if (posscroll >= 0 && posscroll< 10) {///pulso boton en menu inicio
    submenu=0;
    if(posscroll<4){
      posscroll++;
      posscroll = posscroll*10;///me meto en submenu; opcion 0 ha de acabar en opcion 10
      ///elementopulsado=0;
      }else{
        elementopulsado = posscroll;///no hay submenu, es funcion directa
        hacer_lo_que_se_pulso();      
        }
  }else{//////////mayor de 10, estoy en un subapartado
    int subapartado = posscroll%10;//% DEVUELVE EL RESTO DE LA DIVISION
    if(subapartado==3){posscroll=0;/*elementopulsado=0;*/}////volver///
    else{
      elementopulsado = posscroll;
      submenu = (posscroll/10);
      numeroled = submenu-1;
          if(subapartado==0){brilloledes[numeroled]=255;}///a tope
    else if(subapartado==1){brilloledes[numeroled]=0;}////off
    else if(subapartado==2){ajustarbrilloled(numeroled);}else{}///
      }
    }
}
void hacer_lo_que_se_pulso(){
      switch (elementopulsado) {
      Serial.println(elementopulsado);          
    case 04:
      todos_05_seg();
      break;
    case 05:
      todos_1_seg();
      break;
    case 06:
      cuatro_a_la_vez();
      break;
    case 07:
      todos_off();
      break;
    default:
      // statements
      break;
      }///fin del switch 
  }
void ClockChanged() {////////////del ejemplo del encoder
  
  int clkValue = digitalRead(CLK);//Read the CLK pin level
  int dtValue = digitalRead(DT);//Read the DT pin level
  ///if (lastCLK != clkValue){
    //lastCLK = clkValue;
    //count += (clkValue != dtValue ? 1 : -1);//CLK and inconsistent DT + 1, otherwise - 1
    ///if (clkValue != dtValue){a=1} else {a=-1}
    if(!ajustandoled){///no estoy ajustando el led
          if (clkValue == dtValue) {
      if(posscroll<minmax[submenu][1]){posscroll++;Serial.println("Sumando:  ");}
    } else {
      if(posscroll>minmax[submenu][0]){posscroll--;Serial.println("Restando:  ");}
    }
      }else{////ajustando el led
          if (clkValue == dtValue) {
      if(brilloledes[numeroled]<250){brilloledes[numeroled] = brilloledes[numeroled]+10;}
    } else {
      if(brilloledes[numeroled]>9){brilloledes[numeroled] = brilloledes[numeroled]-10;}
    }
      }/////fin del else de ajustandoled    
  //}////fin de (lastCLK != clkValue){
  sacar_variables_serial();
}

/*////tiempoos   int intervalos[] = {500,1000,1500,2000};*/
void parpadeos(byte indice_led,byte indice_intervalo) {/////////////finiquitat/////////////500
  ahora = millis();
if(ahora-antes[indice_led]>=intervalos[indice_intervalo]){
  antes[indice_led]=ahora;
  estadoledes[indice_led] = !estadoledes[indice_led];///cambiazo
  digitalWrite(ledes[indice_led],estadoledes[indice_led]);
  }
}

//T 0.5 seg,T 1 seg,distintos valores
void todos_05_seg(){/////////////finiquitat/////////////500  Serial.println("todos_05_seg  ");
  ahora = millis();
if(ahora-antes[0]>=intervalos[0]){
  ///Serial.println("todos_05_seg  ");
  antes[0]=ahora;
  estadotodos = !estadotodos;///cambiazo
  for(int i=0;i<4;i++){digitalWrite(ledes[i],estadotodos);}  
  }
  }
void todos_1_seg() {/////////////finiquitat/////////////1000
  ahora = millis();
if(ahora-antes[0]>=intervalos[1]){
  antes[0]=ahora;
  estadotodos = !estadotodos;///cambiazo
  for(int i=0;i<4;i++){digitalWrite(ledes[i],estadotodos);}  
  }
}
void cuatro_a_la_vez() {/////////////finiquitat/////////////
  // put your main code here, to run repeatedly:
ahora = millis();
for(int i=0;i<4;i++){
  if(ahora-antes[i]>=intervalos[i]){
  antes[i]=ahora;
  estadoledes[i] = !estadoledes[i];/////cambiazo///
  digitalWrite(ledes[i],estadoledes[i]);
  }////fin del if
  }/////find el for
}
void todos_off(){
  for(int i=0;i<4;i++){
  estadoledes[i] = 0;/////apagado///
  brilloledes[i] = 0;
  digitalWrite(ledes[i],estadoledes[i]);
  }/////find el for
  }
void sacar_variables_serial(){////////////ACABAT/////////////
  //////limites del menu
  /*Serial.println("limites del menu");
  for(int i = 0;i<4;i++){
    for(int j=0;j<2;j++){
      Serial.print(minmax[i][j]);
      }
      Serial.println();
    }*/
    Serial.print("Posicion del encoder: ");
    Serial.println(posscroll);
    Serial.print("Submenu:  ");
    Serial.print(submenu);
    Serial.print("Elemento pulsado");
    Serial.println(elementopulsado);
    Serial.print("Ledes  ");
        for(int i = 0;i<4;i++){
          Serial.print(ledes[i]);
          Serial.print("-");
          Serial.print(brilloledes[i]);
          Serial.print("|");
    }
    Serial.println();
    Serial.print("ledactual  ");
    Serial.println(numeroled);
    Serial.print("Ajustandoled: ");
    Serial.println(ajustandoled);
    Serial.print("Estado ledes ");
    for(int i = 0;i<4;i++){
      Serial.print(i+1);
      Serial.print("-");
      Serial.print(estadoledes[i]);
      Serial.print("|");
    }
    Serial.print("todos");
    Serial.println(estadotodos);
    
    Serial.print("Antes ");
    for(int i = 0;i<4;i++){
      Serial.print(antes[i]);
      Serial.print("-");
    }
    Serial.println();
    Serial.println("--------------------------------------");
}

  
