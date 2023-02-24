#include <Arduino.h>
#include <DHT.h>		// importa la Librerias DHT
#include <DHT_U.h>

#define TEMPERATURA_MAXIMA 28 //es la temperatura maxima, cuando se pase esta temperatura se va a prender el cooler
#define TEMPERATURA_MINIMA 15 //es la temperatura minima, cuando la temperatura este por debajo de esta se activa la lampara
#define TEMPERATURA_PROMEDIO 20 //es la temperatura promedio.

//si la temperatura es menor que la minima se prende la lampara, esta va a estar prendida hasta que la temperatura sea mayor o igual  que la temperatura promedio
//si la temperatura es mayor a la temperatura maxima se prende el cooler, este se va a apagar cuando la temperatura sea menor o igual a la promedio

//porcentajes de humedad
//////////////////////// el 50 de humedad es critico, practicamente agua ///////////////////////////////
#define HUMEDAD_MAXIMA 28 //es el porcentaje maximo de humedad 
#define HUMEDAD_MINIMA 20 //es el porcentaje minimo de humedad
#define HUMEDAD_PROMEDIO 25 //es el porcentaje promedio de humedad 

//si la humedad esta por debajo de la humedad minima se prende la bomba de agua
//la bomba de agua se apagara cuando el porcentame de humedad sea mayor o igual que la humedad promedio
//si la humeda es mayor que la humedad maxima se prendera la lampara
//la lampara se apagara cuando la humedad sea menor o igual a la humedad promedio

#define BOMBA_DE_AGUA 12 //pin del rele que controla la bomba de agua 

#define LAMPARA 13 //pin del rele que controla la lampara

#define COOLER 14 //pin del rele que controla el cooler

#define SENSOR_DHT  2			// pin DATA de DHT11 que controla la temperatura ambiente

int SENSOR_LM35 ; 

#define SENSOR_LH69 1 //pin del sensor lh-69 que controla la humedad de la tierra

int temperatura;      //variable donde vamos a almacenar la temperatura obtenida por el sensor

int sensor_de_dht_funcional = true;


int humedad;          //variable donde vamos a almacenar la humedad obtenida por el sensor


DHT dht(SENSOR_DHT, DHT11);	
// creacion del objeto, le indicamos el pin de data del sensor y el modelo de este.
//ya que con esta misma libreria se le puede dar uso al DHT22



void setup(){

Serial.begin(9600);

  // inicializacion de sensor dht
  dht.begin();	

  //establecemos los pines de los reles como salida (output)
  pinMode(BOMBA_DE_AGUA, OUTPUT);
  pinMode(LAMPARA, OUTPUT);
  pinMode(COOLER, OUTPUT);

}

// activacion del modulo de rele con un nivel bajo
//le enviamos al pin del rele un nivel bajo
void prender( int rele){
  digitalWrite(rele, LOW);	
}

// apagado del modulo de rele con un nivel alto
//le enviamos al pin del rele un nivel alto
void apagar(int rele){
  digitalWrite(rele, HIGH);
}

void loop(){
  
  if(sensor_de_dht_funcional){
    temperatura = dht.readTemperature();	// obtencion de valor de temperatura
  }

  if(!sensor_de_dht_funcional){
    SENSOR_LM35 = analogRead(A0); //dentro de analogRead va el pin del sensor lm-35
    temperatura = (( SENSOR_LM35 * 5000.0) / 1023) / 10;	// obtencion de valor de temperatura
    //calculo necesario para pasar los mili volts que obtiene el sensor lm-35 a temperatura
  }


  //obtenemos el voltaje en bits, leyendo el pinde del sensor
  // de 0 a 4095 siendo este el maximo 
  int humedad_voltaje = analogRead(SENSOR_LH69);

  //mapeamos el valor de la humedad para pasarlo a un porcentaje
  //siendo el maximo 4095 el 100% y 0 es 0% 
  //de esta forma en la variable humedad guardaremos el porcentaje
  humedad = map(humedad_voltaje, 4095, 0, 0, 100);
  
  //prender y apagar bomba de agua
  if( humedad <= HUMEDAD_MINIMA ){
    // prender bomba de agua
    prender(BOMBA_DE_AGUA);
  }else if(humedad >= HUMEDAD_PROMEDIO ){
    //apagar bomba de agua
    apagar(BOMBA_DE_AGUA);
  }


  //en el supuesto caso que la humedad sea mayor a la maxima 
  //se prenderan la lampara y el cooler
  //pero solo uno quedara prendido
  //esto se da de la siguiente manera
  //si la humedad es mayor a la maxima se prende el cooler y la lampara
  //en caso de que la temperatura sea menor a la minima el cooler se apagara y quedara la lampara
  //caso contrario, la temperatura es mayor a la maxima se apaga la lampara y quedara el cooler
  if( humedad >= HUMEDAD_MAXIMA ){
    // prender cooler
    prender(LAMPARA);
    prender(COOLER);
  }

  //prender y apagar lampara
  if(temperatura <= TEMPERATURA_MINIMA){
    //prender lampara
    prender(LAMPARA);
  }else if(temperatura >= TEMPERATURA_PROMEDIO){
    //apagar lampara
    apagar(LAMPARA);
  }

  //preder cooler y apagar cooler
  if(temperatura >= TEMPERATURA_MAXIMA){
    //prender cooler
    prender(COOLER);
  }else if(temperatura <= TEMPERATURA_PROMEDIO){
    //apagar cooler
    apagar(COOLER);
  }

  //comprobamos si el dht esta habilitado 
  //si este da como valor de temperatura 0
  //obtenemos las mediciones de el lm35
  if( temperatura == 0){
  sensor_de_dht_funcional = false;
  }

  //un pequeño delay que va a causar una pausa de 500 mili segundos 
  //con el fin de pausar medio segundo cada ronda del programa
  //este recurso es opcional
  delay(500);	

Serial.print("humedad:");

Serial.print(humedad);
}




