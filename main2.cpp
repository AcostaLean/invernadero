#include <Arduino.h>
#include <DHT.h>		// importa la Librerias DHT
#include <DHT_U.h>
#include <WiFi.h>

//------------------Servidor Web en puerto 80---------------------

WiFiServer server(80);

//---------------------Credenciales de WiFi-----------------------

const char* ssid     = "Gladys";
const char* password = "gladys-115";

//---------------------VARIABLES GLOBALES-------------------------
int contconexion = 0;

String header; // Variable para guardar el HTTP request

//------------------------CODIGO HTML------------------------------
String paginaInicio = "<!DOCTYPE html>"
"<html>"
"<head>"
"<meta charset='utf-8' />"
"<META HTTP-EQUIV='Refresh' CONTENT='1'>"
"<title>Invernadero IOT</title>"
"<head>Invernadero IOT</head>"
"<body>"
"<center>"
"<h3>humedad</h3>"
;

String pagina2 = "<body>"
"<center>"
"<h3>temperatura</h3>";

String paginaFin = "</center>"
"</body>"
"</html>"
;

#define TEMPERATURA_MAXIMA 25 //es la temperatura maxima, cuando se pase esta temperatura se va a prender el cooler
#define TEMPERATURA_MINIMA 15 //es la temperatura minima, cuando la temperatura este por debajo de esta se activa la lampara
#define TEMPERATURA_PROMEDIO 20 //es la temperatura promedio.

//si la temperatura es menor que la minima se prende la lampara, esta va a estar prendida hasta que la temperatura sea mayor o igual  que la temperatura promedio
//si la temperatura es mayor a la temperatura maxima se prende el cooler, este se va a apagar cuando la temperatura sea menor o igual a la promedio

//porcentajes de humedad
//////////////////////// el 50 de humedad es critico, practicamente agua ///////////////////////////////
#define HUMEDAD_MAXIMA 28 
#define HUMEDAD_MINIMA 10 
#define HUMEDAD_PROMEDIO 25

//si la humedad esta por debajo de la humedad minima se prende la bomba de agua
//la bomba de agua se apagara cuando el porcentame de humedad sea mayor o igual que la humedad promedio
//si la humeda es mayor que la humedad maxima se prendera la lampara
//la lampara se apagara cuando la humedad sea menor o igual a la humedad promedio

#define BOMBA_DE_AGUA 19 //pin del rele que controla la bomba de agua 

#define LAMPARA 13 //pin del rele que controla la lampara

#define COOLER 23 //pin del rele que controla el cooler

#define SENSOR_DHT  27		// pin DATA de DHT11 que controla la temperatura ambiente

int SENSOR_LM35 ; 

#define SENSOR_HUMEDAD_TIERRA 33 //pin del sensor lh-69 que controla la humedad de la tierra

int temperatura;      //variable donde vamos a almacenar la temperatura obtenida por el sensor

int sensor_de_dht_funcional = true;

int lm35=5;


//int humedad;          //variable donde vamos a almacenar la humedad obtenida por el sensor


DHT dht(SENSOR_DHT, DHT11);	
// creacion del objeto, le indicamos el pin de data del sensor y el modelo de este.
//ya que con esta misma libreria se le puede dar uso al DHT22


void setup(){

Serial.begin(9600);

Serial.println("");

  // Conexión WIFI
  WiFi.begin(ssid, password);
  //Cuenta hasta 50 si no se puede conectar lo cancela
  while (WiFi.status() != WL_CONNECTED and contconexion <50) { 
    ++contconexion;
    delay(500);
    Serial.print(".");
  }
  if (contconexion <50) {
      //para usar con ip fija
      //IPAddress ip(192,168,1,180); 
      //IPAddress gateway(192,168,1,1); 
      //IPAddress subnet(255,255,255,0); 
      //WiFi.config(ip, gateway, subnet); 

      Serial.println("");
      Serial.println("WiFi conectado");
      Serial.println(WiFi.localIP());
      server.begin(); // iniciamos el servidor
  }
  else { 
      Serial.println("");
      Serial.println("Error de conexion");
  }

 
 { 
  // inicializacion de sensor dht
  dht.begin();	

  //establecemos los pines de los reles como salida (output)
  pinMode(BOMBA_DE_AGUA, OUTPUT);
  pinMode(LAMPARA, OUTPUT);
  pinMode(COOLER, OUTPUT);
  pinMode(lm35, INPUT);
   pinMode(SENSOR_HUMEDAD_TIERRA, INPUT);

 }

}

// activacion del transistor con un nivel alto

void prender( int transistor){
  digitalWrite(transistor, HIGH);	
}

// apagado del transistor con un nivel bajo

void apagar(int transistor){
  digitalWrite(transistor, LOW);
}



void prender_rele( int transistor){
  digitalWrite(transistor, LOW);	
}

// apagado del transistor con un nivel bajo

void apagar_rele(int transistor){
  digitalWrite(transistor, HIGH);
}
void loop(){


WiFiClient client = server.available();   // Escucha a los clientes entrantes

  if (client) {                             // Si se conecta un nuevo cliente
    Serial.println("New Client.");          // 
    String currentLine = "";                //
    while (client.connected()) {            // loop mientras el cliente está conectado
      if (client.available()) {             // si hay bytes para leer desde el cliente
        char c = client.read();             // lee un byte
        Serial.write(c);                    // imprime ese byte en el monitor serial
        header += c;
        if (c == '\n') {                    // si el byte es un caracter de salto de linea
          // si la nueva linea está en blanco significa que es el fin del 
          // HTTP request del cliente, entonces respondemos:
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

           int humedad_voltaje = analogRead(SENSOR_HUMEDAD_TIERRA);

            // Muestra la página web
            client.println(paginaInicio + String(map(humedad_voltaje, 4095, 0, 0, 100)) + paginaFin);
            client.println(pagina2 + String (dht.readTemperature()) + paginaFin);

            // la respuesta HTTP temina con una linea en blanco
            client.println();
            break;
          } else { // si tenemos una nueva linea limpiamos currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // si C es distinto al caracter de retorno de carro
          currentLine += c;      // lo agrega al final de currentLine
        }
      }
    }
    // Limpiamos la variable header
    header = "";
    // Cerramos la conexión
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }



if(sensor_de_dht_funcional){
    temperatura = dht.readTemperature();	// obtencion de valor de temperatura
  }

  if(!sensor_de_dht_funcional){
    SENSOR_LM35 = analogRead(lm35); //dentro de analogRead va el pin del sensor lm-35
    temperatura = (( SENSOR_LM35 * 5000.0) / 4095) / 10;	// obtencion de valor de temperatura
    //calculo necesario para pasar los mili volts que obtiene el sensor lm-35 a temperatura
  }
  
  


  //obtenemos el voltaje en bits, leyendo el pinde del sensor
  // de 0 a 4095 siendo este el maximo 
  
  int humedad_voltaje = analogRead(SENSOR_HUMEDAD_TIERRA);
  //mapeamos el valor de la humedad para pasarlo a un porcentaje
  //siendo el maximo 4095 el 100% y 0 es 0% 
  //de esta forma en la variable humedad guardaremos el porcentaje
  int humedad = map(humedad_voltaje, 4095, 0, 0, 100);
  
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
    prender_rele(LAMPARA);
  }else if(temperatura >= TEMPERATURA_PROMEDIO){
    //apagar lampara
    apagar_rele(LAMPARA);
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
  if(temperatura == 0 || temperatura ==  100){
  sensor_de_dht_funcional = false;
  }

  //un pequeÃ±o delay que va a causar una pausa de 500 mili segundos 
  //con el fin de pausar medio segundo cada ronda del programa
  //este recurso es opcional
  delay(500);	


Serial.print("humedad:");

Serial.println(humedad);



Serial.print("temperatura:");

Serial.println(temperatura);

}


