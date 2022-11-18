
//////////////////////////////////////////////////
//////////////////////////
/// LIBRERIAS

#include <Base64.h>
#include <SPI.h>
#include <Ethernet.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <EEPROM.h>
#include "DHT.h"
LiquidCrystal_I2C lcd(0x27, 16, 2);//0x27
///////////////////////////

///// variables para el servidor y para el mail/////
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177); /// cambiar esto
EthernetServer server(80);
char server2[] = "192.168.1.19"; /// cambiar esto ip server
long tiempo_pasado_mail = 0;       // variable temporal utilizad praa millis para saber cadada cuanto  actualizar la pantalla
long tiempo_lectura_mail = 900000;
short int bandera_mail = 0;
/////////////////////////////////////////////////////
////////////////////////////////////////////
short int RELAY_AIRE1 = 3; // Salida del RELAY  aire 1
short int entrada_on_aire1 = 5; // ENTRADA LLAVE AIRE 1
short int entrada_auto_aire = 7; // ENTRADA LLAVE AUTO
boolean pantalla_cambio = 1; // variable para actualizar mi lcd cada vez q se pone en 1 es pq se tiene q actualizar se pone ne 0 una vez actualizada
short int RELAY_AIRE2 = 4; // Salida del RELAY aire 2
short int entrada_on_aire2 = 6; // entrad aaire 2
short int bandera_temperatura = 1; // bandera utilizada para saber si la temperatura esta en el rango programado y para saber si esta fallando el sensor de temp
boolean ledstate2 = HIGH; // variable utilziada para intercalar los aires
boolean ledstate = LOW; // variable utilizada para intercalar los aires
float temperatura = 30; // variable utilizada para alacenar la temperatura e imprimir
float humedad = 0; // el valor de la humedad
//////////// interrupciones
short int boton_1 = A0; // boton 1  para menu
short int boton_2 = A1; // boton 2  para menu
short int boton_3 = A2; // boton 3 par amenu
////////////////////
short int temperatura_minima = 21; // temperatura minima programda
short int temperatura_maxima = 25; // temperatura maxima programada
const long tiempo_reinicio = 1296000000; // reiniicia cada 15 dias
int tiempo_lectura_temperatura = 5000; //// variable temporal utilizad praa millis para saber cadada cuanto leer la temperatura
//////////////////// tiempos
////////////////

unsigned long tiempo_pasado_temperatura = 0;   // variable temporal utilizad praa millis para saber cadada cuanto leer la temperatura


long tiempo_pasado_intercalacion = 0;  // variable temporal utilizad praa millis para saber cadada cuanto  intercalar los aires
long intervalo_on_aires = 3600000;           // variable temporal utilizad praa millis para saber cadada cuanto  intercalar los aires
long intervalo_off_aires = 3600000;  // variable temporal utilizad praa millis para saber cadada cuanto  intercalar los aires


#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

short int ESTADO_AUTOMATICO_MODO = 0; // variable se pone en 1 si esta el aire 1 prendido y el aire 2 apagado, y se pone en 0 si esta el aire 1 apagado y el 2 prendido , esto se usa para detectar problemas en aires
short int CONTADOR_ESTADO_AUTOMATICO_MODO = 0; // es un contador que si llega a 3 es pq uno de los aires no esta funcionando bien se usa en la funcion nuevalarma
short int BANDERA_ESTADO_AUTOMATICO_MODO = 0; // se utiliza esta bandera, para el tema del conteo que se realiza cada media hora, asi se resetea
long TIEMPO_ESTADO_AUTOMATICO_MODO = 1800000; // cada media hora detecta si los aires se prendieron mas de tres veces
long  TIEMPO_PASADO_AUTOMATICO_MODO = 0;
int alerta_aire1_problemas = 0; // si esta alarma se pone en 1, manda mails y muestra en la pagina web
short int CONTADOR_ESTADO_AUTOMATICO_MODO2 = 0;
short int   BANDERA_ESTADO_AUTOMATICO_MODO2 = 0;
short int   alerta_aire2_problemas = 0;

short int   alerta_ambos_prendidos = 0;

long TIEMPO_ALARMAS2 = 3600000; // ambos aires tiempo
long TIEMPO_ALARMAS2_PASADO = 0; // cada media hora detecta si los aires se prendieron mas de tres veces

String readString;
char linebuf[80];
int charcount = 0;
boolean authentificated = false;

String password = "hola1000"; // password actual que se queda guardado en la eprom 
String password_actual = "hola1000"; // password que debe introducir en la pagina para verifica que es la actual 
//boolean authentificated=false;
char password2[] = "dGVjbm8xMDA"; // password convertida a base 64 
int error_8letras = 0;

//////////////////////// funciones////////////
void leer_temperatra();
int accion_sobre_aires(int num_llave_on1, int num_llave_on2, int num_llave_auto );
int modo_aire_auto();
int imprimir_pantalla(int numero);
void detector_interrupciones();
int pagina_web(int num);
int temperatura_alarma = 27;
void inicializar_erom();
void reinicio();
void mandar_mails();
void leer_boton1();
boolean boton1_pasado = 0;
boolean estado_boton1 = 0;
void leer_boton2();
boolean boton2_pasado = 0;
boolean estado_boton2 = 0;
void leer_boton3();
boolean boton3_pasado = 0;
boolean estado_boton3 = 1;
boolean verificacion_exitosa=0;
void leer_temperatra_inicio();



void setup() {
  Serial.begin(9600);
  //pinMode(A15, OUTPUT);
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  //////////////////////////////////
  pinMode(RELAY_AIRE1, OUTPUT);
  pinMode(RELAY_AIRE2, OUTPUT);
  /// pines lectores del aire 1
  pinMode(entrada_on_aire1, INPUT);
  pinMode(entrada_auto_aire, INPUT);
  pinMode(entrada_on_aire2, INPUT);

  pinMode(boton_1, INPUT);
  pinMode(boton_2, INPUT);

  //////////////////
  Wire.begin();
  lcd.begin(16, 2);
  lcd.clear();
  lcd.backlight();
  inicializar_erom();
  base64();
  dht.begin();
  leer_temperatra_inicio();

  delay (100);

}
///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  short int numero_pantalla;
  short int variable_pantalla;

  leer_boton1(); // boton llave 1 lee con un guau filtro
  leer_boton2(); // boton llave 2 lee con un guau filtro
  leer_boton3(); // boton auto   lee con un guau filtro




  leer_temperatra();//lee cada  5 segundos la temperatura , tambien cambia el valor de la bandera_temperatura que se usa para saber si fallta el sensor de temperatura en  la funcion modo aire auto
  variable_pantalla = accion_sobre_aires(estado_boton1, estado_boton2, estado_boton3); /// realiza la accion en los aires, tiene un modo manual y automatico retorna el valor del modo que sei mrpiem ne la pantalla
  imprimir_pantalla(variable_pantalla); // imprimir la pantalla lcd
  // sei se apretan los tres botones se interrumpe
  nuevas_alarmas(); /// ACTIVA LA BANDERA ALARMA alerta_aire2_problemas alerta_aire1_problemas si uno de los aires se prende y se apaga muchas veces por que el otro no funca
  nuevas_alarmas2(); // activa la bandera alerta_ambos_prendidos por si los dos iares estan prendidos por mas de una hora
  /// si detecta que los tres botones del menu estan apretados entra en una interrumpcion que es modo menu donde configuras la tempeeratura minima maxima  el t de intercalacion y al temp alarma
  if (digitalRead(boton_1) && digitalRead(boton_2) && digitalRead(boton_3)) {
    pantalla_cambio = 1;
    detector_interrupciones();
  }
  /// genera un servidor web en el local host del equipo donde muestra los estados de los aires y la temepratura
  pagina_web(variable_pantalla);  // cada 20 segundos actualiza la pagina web, se accede dependiendo de la ip asignada
  /// funcion que actualiza la bandera del mail, si es que hace falta mandar un mail por temperatura alta la bandera se cambia a 1, luego se pone en 2  abajo para no mandar todo el tiempo mails, vuelve a 0 despues de un tiempo
  actualizar_banderamail();
  ////// si la bandera mail se puso en 1 manda el mail
  if (bandera_mail == 1) { /// pregunta si la bandera mail esta activada
    mandar_mails();
    bandera_mail = 2;
  }

  reinicio(); // reinicia despues de 15 dias
  //Serial.println(control_web);

  //delay (400);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void leer_boton1() {
  boolean boton1_actual = 0;
  boton1_actual = digitalRead(entrada_on_aire1); // asigno los vallores de las tres llaves
  delay(30);
  if ((boton1_actual == 1) && (boton1_pasado == 0)) {
    pantalla_cambio = 1;
    estado_boton1 = 1 - estado_boton1;
    delay(30);
  }
  boton1_pasado = boton1_actual;
  EEPROM.update(6, estado_boton1);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void leer_boton2() {
  boolean boton2_actual = 0;
  boton2_actual = digitalRead(entrada_on_aire2); // asigno los vallores de las tres llaves
  delay(30);
  if ((boton2_actual == 1) && (boton2_pasado == 0)) {
    pantalla_cambio = 1;
    estado_boton2 = 1 - estado_boton2;
    delay(30);
  }
  boton2_pasado = boton2_actual;
  EEPROM.update(7, estado_boton2);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void leer_boton3() {
  boolean boton3_actual = 0;
  boton3_actual = digitalRead(entrada_auto_aire); // asigno los vallores de las tres llaves
  delay(30);
  if ((boton3_actual == 1) && (boton3_pasado == 0)) {
    pantalla_cambio = 1;
    estado_boton3 = 1 - estado_boton3;
    delay(30);
  }
  boton3_pasado = boton3_actual;
  EEPROM.update(8, estado_boton3);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void reinicio() {

  unsigned long currentMillis = 0;
  currentMillis = millis();
  if ( currentMillis >= tiempo_reinicio) {
    delay (10);
    asm("jmp 0x0000");
  }
}


void actualizar_banderamail() {
  unsigned long currentMillis = 0;
  currentMillis = millis();
  if (bandera_mail == 0) {
    if ((temperatura >= temperatura_alarma) || (temperatura < 0) || alerta_aire2_problemas == 1 || alerta_aire1_problemas == 1 || alerta_ambos_prendidos == 1) { // pregunta si hay un error y pone la bandera en 1 del mail

      bandera_mail = 1;

    } else {
      bandera_mail = 0;
    }
  }

  if (bandera_mail == 2) { // despues de 15 minutos cera la bandera otravez para mandar el mail again

    if (currentMillis - tiempo_pasado_mail > tiempo_lectura_mail)
    {

      tiempo_pasado_mail = currentMillis;
      bandera_mail = 0;
    }

  }


}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void mandar_mails() {
  EthernetClient cliente;
  //Serial.println("mandandoemail");

  if (cliente.connect(server2, 80)) {
    // Make a HTTP request:
    if (temperatura >= temperatura_alarma) {
      // Serial.print("mandando mail");
      cliente.println("GET /MAIL/arduino-mail.php?text=ALTA.TEMPERATURA.EN.LA.SALA.DE.SERVIDORES. HTTP/1.1");
      cliente.println("Host: 192.168.1.19");                   //your domain cambiar esto ip server
      cliente.println("Connection: close");
      cliente.println();

    } else if (temperatura < 0) {
      //Serial.print("mandando mail2");
      cliente.println("GET /MAIL/arduino-mail.php?text=ERROR.EN.EL.SENSOR.DE.TEMPERATURA. HTTP/1.1");
      cliente.println("Host: 192.168.1.19");                   //your domain cambiar esto ip server 
      cliente.println("Connection: close");
      cliente.println();

    } else if (alerta_aire2_problemas == 1) {
      //Serial.print("mandando mail3");
      cliente.println("GET /MAIL/arduino-mail.php?text=ALERTA.POSIBLES.PROBLEMAS.EN.EL.AIRE.2.NO.ESTA.ENFRIANDO.LO.SUFICIENTE. HTTP/1.1");
      cliente.println("Host: 192.168.1.19");                   //your domain cambiar esto ip server
      cliente.println("Connection: close");
      cliente.println();

    }
    else if (alerta_aire1_problemas == 1) {
      //Serial.print("mandando mail4");
      cliente.println("GET /MAIL/arduino-mail.php?text=ALERTA.POSIBLES.PROBLEMAS.EN.EL.AIRE.1.NO.ESTA.ENFRIANDO.LO.SUFICIENTE.. HTTP/1.1");
      cliente.println("Host: 192.168.1.19");                   //your domain cambiar esto ip server
      cliente.println("Connection: close");
      cliente.println();

    }
    else if (alerta_ambos_prendidos == 1) {
      //Serial.print("mandando mail5");
      cliente.println("GET /MAIL/arduino-mail.php?text=ALERTA.AMBOS.AIRES.ESTAN.ENCENDIDOS.HACE.MÁS.DE.1.HORA. HTTP/1.1");
      cliente.println("Host: 192.168.1.19");                   //your domain cambiar esto ip server
      cliente.println("Connection: close");
      cliente.println();

    }

    //Serial.println("mandandando mail");
    delay(200);
    cliente.stop();
  }
}

/////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int pagina_web(int num) {
  // listen for incoming clients
  EthernetClient client = server.available();
  memset(linebuf, 0, sizeof(linebuf));
  charcount = 0;
  authentificated = false;

  if (client) {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        linebuf[charcount] = c;
        if (charcount < sizeof(linebuf) - 1) charcount++;
        if (readString.length() < 100)
        {
          readString = readString + c;// Store characters to string
        }
        if (c == '\n' && currentLineIsBlank) {
          if (authentificated) {

            // send a standard http response header
            client.println(F("HTTP/1.1 200 OK"));
            client.println(F("Content-Type: text/html"));
            //client.println("Connection: close");
            client.println();
            client.println(F("<!DOCTYPE HTML>"));
            client.println(F("<BODY>"));
            client.println("<html>");
            // add a meta refresh tag, so the browser pulls again every 10 seconds:
            //client.println("<meta http-equiv=\"refresh\" content=\"15\">");
            client.println("<meta http-equiv='REFRESH' content='15; url=http://192.168.1.177'> ") ; // cambiar esto
            // output the value of each analog input pin
            client.println("<HEAD>");

            client.println("<TITLE>SALA DE SERVIDORES</TITLE>");
            client.println("</HEAD>");


            client.println("<img src=\"https://a.storyblok.com/f/45096/912x238/b7fe1a8844/tecnologik.jpg\" width=600 height=200>");
            //client.println("<img src=\"https://a.storyblok.com/f/45096/912x238/b7fe1a8844/tecnologik.jpg\" width=600 height=200>");
            delay (100);
            client.println("<br />");


            client.println("<h2 style=\"color:Navy\">MONITOREO DE TEMPERATURA EN LA SALA DE SERVIDORES</h2>");
            client.println("<br />");
            client.print("<font size=\"+2\">TEMPERATURA:&nbsp");
            client.print("<font color=\"blue\"> " );
            client.print(temperatura);
            client.print("C");
            client.print("</font>");
            client.print("&nbsp&nbsp");
            client.print("HUMEDAD:");
            client.print("<font color=\"blue\"> " );
            client.print(humedad);
            client.print("%");
            client.print("</font>");
            client.print("</font>");

            client.println("<br />");
            if (temperatura > temperatura_alarma) {

              //client.print();
              client.print( "<font color=\"red\">ALERTA LA SALA DE SERVIDORES TIENE UNA ALTA TEMPERATURA: </font>" );
              if (num == 7 || num == 1 ) {
                client.print("<font color=\"red\">POSIBLES FALLAS EN AMBOS AIRES </font>" );
              } else {
                client.print("<font color=\"red\">POSIBLE FALLA EN UNO DE LOS  AIRES </font>" );
              }
              client.println("<br />");
            }

            if (temperatura < 0) {
              //client.print("ALERTA FALLA EN EL SENSOR DE TEMPERATURA ");
              client.print("<font color=\"red\">ALERTA FALLA EN EL SENSOR DE TEMPERATURA </font>" );
              client.println("<br />");
            }
            if (alerta_aire1_problemas == 1) {
              client.print("<font color=\"red\">ALERTA POSIBLES PROBLEMAS EN EL AIRE 1. NO ESTA ENFRIANDO LO SUFICIENTE </font>" );
              client.println("<br />");
            }

            if (alerta_aire2_problemas == 1) {
              client.print("<font color=\"red\">ALERTA POSIBLES PROBLEMAS EN EL AIRE 2. NO ESTA ENFRIANDO LO SUFICIENTE </font>" );
              client.println("<br />");
            }

            if (alerta_ambos_prendidos == 1) {
              client.print("<font color=\"red\">ALERTA AMBOS AIRES PRENDIDOS HACE MAS DE 1 HORA </font>" );
              client.println("<br />");
            }
            switch (num) {
              case 5:
                //client.print("MODO DE OPERACION: AUTOMATICO");
                client.print("<font size=\"+2\">MODO DE OPERACION: </font>");
                client.print("<font color=\"blue\" size=\"+2\">AUTOMATICO</font>");
                client.println("<br />");
                client.print("<font size=\"+2\">AIRE 1: </font>");
                client.print("<font color=\"green\" size=\"+2\">ENCENDIDO</font>");
                client.println("<br />");
                client.print("<font size=\"+2\">AIRE 2: </font>");
                client.print("<font color=\"red\" size=\"+2\">APAGADO</font>");
                break;

              case 6:
                client.print("<font size=\"+2\">MODO DE OPERACION: </font>");
                client.print("<font color=\"blue\" size=\"+2\">AUTOMATICO</font>");
                client.println("<br />");
                client.print("<font size=\"+2\">AIRE 1: </font>");
                client.print("<font color=\"red\" size=\"+2\">APAGADO</font>");
                client.println("<br />");
                client.print("<font size=\"+2\">AIRE 2: </font>");
                client.print("<font color=\"green\" size=\"+2\">ENCENDIDO</font>");
                break;

              case 7:
                client.print("<font size=\"+2\">MODO DE OPERACION: </font>");
                client.print("<font color=\"blue\" size=\"+2\">AUTOMATICO</font>");;
                client.println("<br />");
                client.print("<font size=\"+2\">AIRE 1: </font>");
                client.print("<font color=\"green\" size=\"+2\">ENCENDIDO</font>");
                client.println("<br />");
                client.print("<font size=\"+2\">AIRE 2: </font>");
                client.print("<font color=\"green\" size=\"+2\">ENCENDIDO</font>");
                break;



              case 1:
                client.print("<font size=\"+2\">MODO DE OPERACION: </font>");
                client.print("<font color=\"blue\" size=\"+2\">MANUAL</font>");
                client.println("<br />");
                client.print("<font size=\"+2\">AIRE 1: </font>");
                client.print("<font color=\"green\" size=\"+2\">ENCENDIDO</font>");
                client.println("<br />");
                client.print("<font size=\"+2\">AIRE 2: </font>");
                client.print("<font color=\"green\" size=\"+2\">ENCENDIDO</font>");
                break;

              case 3:
                client.print("<font size=\"+2\">MODO DE OPERACION: </font>");
                client.print("<font color=\"blue\" size=\"+2\">MANUAL</font>");;
                client.println("<br />");
                client.print("<font size=\"+2\">AIRE 1: </font>");
                client.print("<font color=\"green\" size=\"+2\">ENCENDIDO</font>");
                client.println("<br />");
                client.print("<font size=\"+2\">AIRE 2: </font>");
                client.print("<font color=\"red\" size=\"+2\">APAGADO</font>");
                break;

              case 2:
                client.print("<font size=\"+2\">MODO DE OPERACION: </font>");
                client.print("<font color=\"blue\" size=\"+2\">MANUAL</font>");
                client.println("<br />");
                client.print("<font size=\"+2\">AIRE 1: </font>");
                client.print("<font color=\"red\" size=\"+2\">APAGADO</font>");
                client.println("<br />");
                client.print("<font size=\"+2\">AIRE 2: </font>");
                client.print("<font color=\"red\" size=\"+2\">APAGADO</font>");
                break;

              case 4:
                client.print("<font size=\"+2\">MODO DE OPERACION: </font>");
                client.print("<font color=\"blue\" size=\"+2\">MANUAL</font>");
                client.println("<br />");
                client.print("<font size=\"+2\">AIRE 1: </font>");
                client.print("<font color=\"red\" size=\"+2\">APAGADO</font>");
                client.println("<br />");
                client.print("<font size=\"+2\">AIRE 2: </font>");
                client.print("<font color=\"green\" size=\"+2\">ENCENDIDO</font>");
                break;
            }
            client.println("<br />");


            client.println("<h2   style=\"color:Navy\">SISTEMA DE CONTROL </h2>");
            //client.print(F("&nbsp"));

            client.print(F("<input type=button value='ENCENDER/APAGAR AIRE 1'style=\"width: 15%; height: 100px; background-color:green; border-color:green; color:white\" onmousedown=location.href='/?Ch_on'>"));


            client.print(F("&nbsp&nbsp&nbsp"));

            client.print(F("<input type=button value='MODO AUTO/MODO MANUAL ' style=\"width: 15%; height: 100px; background-color:green; border-color:green; color:white\" onmousedown=location.href='/?ML_on'>"));

            client.print(F("&nbsp&nbsp&nbsp"));


            client.print(F("<input type=button value='ENCENDER/APAGAR AIRE 2 ' style=\"width: 15%; height: 100px; background-color:green; border-color:green; color:white\" onmousedown=location.href='/?Ch2_on'>"));

            client.println("<br />");
            client.println("<br />");

            if (error_8letras == 1) {
              client.print( "<font color=\"red\">LA PASSWORD DEBE CONTENER 8 CARACTERES EN TOTAL, PUEDEN SER LETRAS O NUMEROS </font>" );
            }
            client.println("<FORM ACTION='/' method=get >");

            ////////////
            if (verificacion_exitosa==0){
              client.println( "<label for=\"pass\">Si quiere cambiar la password de ingreso, ingrese las password ACTUAL y presione enter : </label>");
            client.println("<br />");
            client.print("<font color=\"navy\" size=\"+0\">PASSWORD ACTUAL: </font>");
            client.println("<input type=\"text\"  id=\"pass\" name=\"pass\" value=\"\" style=\"width: 5%; height: 100p;\" >");
            client.println("<br />");
            client.println("<br />");
              
            }else {
            client.println( "<label  for=\"fname\">AHORA INGRESE LA NUEVA PASSWORD (tiene que tener 8 letras o digitos): </label>");
            client.println("<br />");
            client.print("<font color=\"green\" size=\"+0\">NUEVA PASSWORD: </font>");
            client.println("<input type=\"text\"  id=\"fname\" name=\"fname\" value=\"\" style=\"width: 5%; height: 100p;\" >");
            client.println("<br />");
            client.println("<br />");
              
            }
            

            

           
          
              
            
            


              if (readString.indexOf("/?pass") > 0)
            {
              
              int pos3 = readString.indexOf("/?pass");
              int pos4 = readString.indexOf("HTTP");
              //Serial.println(pos);
              //Serial.println(pos2);
              pos3 = pos3 + 7;
              password_actual = readString.substring(pos3, pos4 - 1);
              String password_emergencia="tecno100";
              if ((password_actual ==  password)|| (password_actual ==  password_emergencia)){
                verificacion_exitosa=1;
              }else {
                verificacion_exitosa=0;
              }
              //Serial.println(password_actual);
             // base64();
              //Serial.println(password2);
              //Serial.println("");
              client.println("<meta http-equiv='REFRESH' content='0; url=http://192.168.1.177'> ") ; /// cambiar esto 
              //int l=password.length();
              //Serial.println(l);


            }
            
            if (readString.indexOf("/?fname") > 0)
            {

              int pos = readString.indexOf("/?fname");
              int pos2 = readString.indexOf("HTTP");
              //Serial.println(pos);
              //Serial.println(pos2);
              pos = pos + 8;
              password = readString.substring(pos, pos2 - 1);
              //Serial.println(password);
              base64();
              //Serial.println(password2);
              //Serial.println("");
              client.println("<meta http-equiv='REFRESH' content='0; url=http://192.168.1.177'> ") ; // cambiar esto 
              //int l=password.length();
              //Serial.println(l);


            }
            client.println("<small>");
            client.print("<font size=\"-2\">MONITOREO DE SALA DE SERVIDORES VERSION 1.0 </font>");
            client.println("</small>");
            client.println(F("</body></html>"));
            break;

          }
          else {
            client.println("HTTP/1.1 401 Authorization Required");
            client.println("WWW-Authenticate: Basic realm=\"Secure Area\"");
            client.println("Content-Type: text/html");
            client.println("Connnection: close");
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<HTML>  <HEAD>   <TITLE>Error</TITLE>");
            client.println(" </HEAD> <BODY><H1>401 Unauthorized.</H1></BODY> </HTML>");
            break;
          }
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
          if (strstr(linebuf, "Authorization: Basic") > 0 && ((strstr(linebuf, password2) > 0 ) || (strstr(linebuf, "dGVjbm8xMDA=") > 0) )  )
            authentificated = true;
          memset(linebuf, 0, sizeof(linebuf));
          charcount = 0;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }

      }
    }
    // give the web browser time to receive the data
    delay(10);
    // close the connection:
    if (readString.indexOf("/?ML_on") > 0)
    {
      client.println("<meta http-equiv='REFRESH' content='0; url=http://192.168.1.177'> ") ; // cambiar esto
      if ( estado_boton3 == 1) {
        estado_boton3 = 0;
      } else {
        estado_boton3 = 1; // Switch on Main Light
      }

      pantalla_cambio = 1;

    }
    //
    //          if(readString.indexOf("/?ML_off") > 0){
    //            estado_boton3=0;// Switch off Main Light
    //            pantalla_cambio=1;
    //            client.println("<meta http-equiv='REFRESH' content='0; url=http://192.168.1.177'> ") ;
    //          }

    if (readString.indexOf("/?Ch_on") > 0) {
      client.println("<meta http-equiv='REFRESH' content='0; url=http://192.168.1.177'> ") ; // cambiar esto 
      if (estado_boton1 == 1) {
        estado_boton1 = 0;
        Serial.println("aca");
      } else{
        estado_boton1 = 1; //
        pantalla_cambio = 1;
      }
    }
    //
    //          if(readString.indexOf("/?Ch_off") > 0){
    //            client.println("<meta http-equiv='REFRESH' content='0; url=http://192.168.1.177'> ") ;
    //            estado_boton1=0;//
    //            pantalla_cambio=1;
    //          }

    if (readString.indexOf("/?Ch2_on") > 0) {
      client.println("<meta http-equiv='REFRESH' content='0; url=http://192.168.1.177'> ") ; // cambiar esto 
      if (estado_boton2 == 1) {
        estado_boton2 = 0;
      } else{
        estado_boton2 = 1; //
       pantalla_cambio = 1;
      }
    }

    if (readString.indexOf("/?Ch2_off") > 0) {
      client.println("<meta http-equiv='REFRESH' content='0; url=http://192.168.1.177'> ") ; // cambiar esto 
      estado_boton2 = 0; //
      pantalla_cambio = 1;
    }

    readString = "";// Clearing string for next read
    client.stop();


  }

}

/////////////////////////////////////////////////////////////////////////////////////////////

void detector_interrupciones() {
  short int contador = 0;
  int variable_intercalacion;
  long int contador_sin_movimiento = 0;
  int bandera = 0;
  boolean contador_pantalla3 = 0;
  boolean contador_pantalla2 = 0;
  boolean contador_pantalla1 = 0;
  boolean contador_pantalla0 = 0;





  while (digitalRead(boton_1) || digitalRead(boton_2) || digitalRead(boton_3)) {
    delay (10);
  }
  while (bandera == 0) {
    if (contador_sin_movimiento > 520000) {
      bandera = 1;
    } else {
      contador_sin_movimiento = contador_sin_movimiento + 1;
    }

    if (contador == 0) {
      if (contador_pantalla0 == 0) {
        pantalla_cambio = 1;
        contador_pantalla0 = 1;
      }
      contador_pantalla2 = 0;
      contador_pantalla1 = 0;
      contador_pantalla3 = 0;
      if (digitalRead(boton_3)) {
        while (digitalRead(boton_3)) {
          delay (10);
        }
        pantalla_cambio = 1;
        contador_sin_movimiento = 0;
        intervalo_on_aires = intervalo_on_aires + 3600000;
        intervalo_off_aires = intervalo_on_aires;

        if (intervalo_on_aires >= 43200000) {
          intervalo_on_aires = 43200000;
          intervalo_off_aires = intervalo_on_aires;
        }

      }
      if (digitalRead(boton_1)) {
        while (digitalRead(boton_1)) {
          delay (10);
        }
        pantalla_cambio = 1;
        contador_sin_movimiento = 0;
        intervalo_on_aires = intervalo_on_aires - 3600000;
        intervalo_off_aires = intervalo_on_aires;

        if (intervalo_on_aires <= 0) {
          intervalo_on_aires = 3600000;
          intervalo_off_aires = intervalo_on_aires;
        }


      }
      if (digitalRead(boton_2)) {

        while (digitalRead(boton_2)) {
          delay (10);
        }
        pantalla_cambio = 1;
        contador_sin_movimiento = 0;
        contador += 1;
      }
      imprimir_pantalla(8);

    }

    if ( contador == 1) {
      if (contador_pantalla1 == 0) {
        pantalla_cambio = 1;
        contador_pantalla1 = 1;
      }
      if (digitalRead(boton_3)) {
        while (digitalRead(boton_3)) {
          delay (10);
        }
        pantalla_cambio = 1;
        temperatura_minima = temperatura_minima + 1;
        contador_sin_movimiento = 0;
        if ((temperatura_maxima - temperatura_minima < 3)) {

          temperatura_minima = temperatura_minima - 1;
        }
      }
      if (digitalRead(boton_1)) {
        while (digitalRead(boton_1)) {
          delay (10);
        }
        pantalla_cambio = 1;
        contador_sin_movimiento = 0;
        temperatura_minima = temperatura_minima - 1;
        if (temperatura_minima <= 12) {
          temperatura_minima = 12;
        }
      }
      if (digitalRead(boton_2)) {
        while (digitalRead(boton_2)) {
          delay (10);
        }
        pantalla_cambio = 1;
        contador += 1;
        contador_sin_movimiento = 0;
      }
      imprimir_pantalla(9);
    }

    if ( contador == 2) {
      if (contador_pantalla2 == 0) {
        pantalla_cambio = 1;
        contador_pantalla2 = 1;
      }
      if (digitalRead(boton_3)) {
        while (digitalRead(boton_3)) {
          delay (10);
        }
        pantalla_cambio = 1;
        temperatura_maxima = temperatura_maxima + 1;
        if ( temperatura_maxima > 28) {
          temperatura_maxima = 28;
        }
        contador_sin_movimiento = 0;

      }
      if (digitalRead(boton_1)) {
        while (digitalRead(boton_1)) {
          delay (10);
        }
        pantalla_cambio = 1;
        temperatura_maxima = temperatura_maxima - 1;
        if ( temperatura_maxima - temperatura_minima < 3) {
          temperatura_maxima = temperatura_maxima + 1;
        }
        contador_sin_movimiento = 0;
      }
      if (digitalRead(boton_2)) {
        while (digitalRead(boton_2)) {
          delay (10);
        }
        pantalla_cambio = 1;
        contador_sin_movimiento = 0;
        contador += 1;

      }
      imprimir_pantalla(10);
    }



    if ( contador == 3) {
      if (contador_pantalla3 == 0) {
        pantalla_cambio = 1;
        contador_pantalla3 = 1;
      }
      if (digitalRead(boton_3)) {
        while (digitalRead(boton_3)) {
          delay (10);
        }
        pantalla_cambio = 1;
        temperatura_alarma = temperatura_alarma + 1;
        if ( temperatura_alarma > 30) {
          temperatura_alarma = 30;
        }
        contador_sin_movimiento = 0;

      }
      if (digitalRead(boton_1)) {
        while (digitalRead(boton_1)) {
          delay (10);
        }
        pantalla_cambio = 1;
        temperatura_alarma = temperatura_alarma - 1;
        contador_sin_movimiento = 0;
      }
      if (digitalRead(boton_2)) {
        while (digitalRead(boton_2)) {
          delay (10);
        }
        pantalla_cambio = 1;
        contador_sin_movimiento = 0;
        contador = 0;
        contador_pantalla0 = 0;
      }


      imprimir_pantalla(12);
    }
  }

  if ( temperatura_maxima > 28) {
    temperatura_maxima = 28;
  }
  if ( temperatura_minima > 25 ) {
    temperatura_minima = 25;
  }

  if ( temperatura_alarma > 30 ) {
    temperatura_alarma = 30;
  }


  EEPROM.update(0, temperatura_maxima);
  EEPROM.update(1, temperatura_minima);
  EEPROM.update(2, (intervalo_on_aires / 1000) / 3600);
  EEPROM.update(3, temperatura_alarma);


  //EEPROM.update(3, intervalo_off_aires);


}
//////////////////////////// funcion leer temperatura //////////////////////
void leer_temperatra() {
  unsigned long currentMillis = 0;

  currentMillis = millis();

  if ( currentMillis - tiempo_pasado_temperatura >= tiempo_lectura_temperatura) {
    pantalla_cambio = 1;


    temperatura = dht.readTemperature();


    if (isnan(temperatura)) { /// si lee error que tire que la temperatura es <0 para seguir la logica anterior
      temperatura = -40;
    }

    tiempo_pasado_temperatura = currentMillis;
    if (temperatura >= temperatura_maxima) {
      bandera_temperatura = 1;
    }
    if (temperatura <= temperatura_minima) {
      bandera_temperatura = 0;
    }
    if (temperatura < 0) {
      bandera_temperatura = 2;
    }

    if (bandera_temperatura == 2 && temperatura > 0) {
      bandera_temperatura = 1;
    }

  }


}
///////////////////////////////////////////////////////////////////////////
void leer_temperatra_inicio() {

  temperatura = dht.readTemperature();
  humedad = dht.readHumidity();
  if (isnan(temperatura)) { /// si lee error que tire que la temperatura es <0 para seguir la logica anterior
    temperatura = -40;
  }


  if (temperatura >= temperatura_maxima) {
    bandera_temperatura = 1;
  }
  if (temperatura <= temperatura_minima) {
    bandera_temperatura = 0;
  }
  if (temperatura < 0) {
    bandera_temperatura = 2;
  }

  if (bandera_temperatura == 2 && temperatura > 0) {
    bandera_temperatura = 1;
  }



}
///////////////////////////////////////////////////////////////////
int accion_sobre_aires(int num_llave_on1, int num_llave_on2, int num_llave_auto ) {
  int numero_pantalla;
  if (num_llave_auto == 1) {
    numero_pantalla = modo_aire_auto(); // acion sobre aire automatico


  } else {
    if (num_llave_on1 == 1 && num_llave_on2 == 1) {
      numero_pantalla = 1;
    } else if (num_llave_on1 == 0 && num_llave_on2 == 0) {
      numero_pantalla = 2;
    } else if (num_llave_on1 == 1 && num_llave_on2 == 0) {
      numero_pantalla = 3;
    }
    else if (num_llave_on1 == 0 && num_llave_on2 == 1) {
      numero_pantalla = 4;
    }
    digitalWrite(RELAY_AIRE1, num_llave_on1);
    digitalWrite(RELAY_AIRE2, num_llave_on2);
  }
  //Serial.print(estado_boton2);
  //Serial.print(estado_boton1);
  return (numero_pantalla);

}
/////////////////////////////////////
//// nueva alarma que detecta si estando 1 aire prendido y el otro apagdo, este se prende y se apga muchas veces para mantener el clima lo cual queire decir que le aire que primeramente estaba prendido no esta enfirando lo suficiente
void nuevas_alarmas() {
  //TIEMPO_ESTADO_AUTOMATICO_MODO=1800000;
  /// TIEMPO_PASADO_AUTOMATICO_MODO=0;
  // short int ESTADO_AUTOMATICO_MODO=0;
  //short int CONTADOR_ESTADO_AUTOMATICO_MODO=0;
  //short int BANDERA_ESTADO_AUTOMATICO_MODO=0;

  unsigned long currentMillis = 0;
  currentMillis = millis();
  if (currentMillis - TIEMPO_PASADO_AUTOMATICO_MODO < TIEMPO_ESTADO_AUTOMATICO_MODO) { /// cada 30 min se restea esto

    if ((BANDERA_ESTADO_AUTOMATICO_MODO == 0) && (estado_boton2 == HIGH) && (ESTADO_AUTOMATICO_MODO == 1) && (estado_boton3 == HIGH)) { /// pregunto si la bandera es 0, luego si el aire que no tiene qu eestar prendido se prendio y si estoy en modo auto
      CONTADOR_ESTADO_AUTOMATICO_MODO += 1; // si paso lo anteriro sumo mi contador
      BANDERA_ESTADO_AUTOMATICO_MODO = 1; // la bandera se pone en 1 para q no cuente muchas vces mientras el otro aire esta prendido
      //Serial.println("ep que pasa aca");
      if (CONTADOR_ESTADO_AUTOMATICO_MODO >= 3) { // para que no se vaya todo con el conteo q cuente hasta 3 vces y si cuenta tres uqe la alram se ponga en 1
        alerta_aire1_problemas = 1; // el aire 1 no enfria lo suficiente por eso el aire 2 se prende todo el tiempo
        CONTADOR_ESTADO_AUTOMATICO_MODO = 3;
      }
    } else if ((estado_boton2 == LOW) && (ESTADO_AUTOMATICO_MODO == 1)) { /// una vez que el aire que no tiene qu e esta prendido se apga que la bandera se ponga en 0 otravez por si se vuelve a prender
      BANDERA_ESTADO_AUTOMATICO_MODO = 0;
    }

    ///////////////////////////// para el aire 2 /////////////////////////

    if ((BANDERA_ESTADO_AUTOMATICO_MODO2 == 0) && (estado_boton1 == HIGH) && (ESTADO_AUTOMATICO_MODO == 2) && (estado_boton3 == HIGH)) {
      CONTADOR_ESTADO_AUTOMATICO_MODO2 += 1;
      BANDERA_ESTADO_AUTOMATICO_MODO2 = 1;
      //Serial.println("ep que pasa aca");
      if (CONTADOR_ESTADO_AUTOMATICO_MODO2 >= 3) {
        alerta_aire2_problemas = 1; // el aire 1 no enfria lo suficiente por eso el aire 2 se prende todo el tiempo
        CONTADOR_ESTADO_AUTOMATICO_MODO2 = 3;
      }
    } else if ((estado_boton1 == LOW) && (ESTADO_AUTOMATICO_MODO == 2)) {
      BANDERA_ESTADO_AUTOMATICO_MODO2 = 0;
    }

    /////////////// reseteo todo depe de los 30 min //////////////////////////

  } else {
    //Serial.println("entra2");
    TIEMPO_PASADO_AUTOMATICO_MODO = currentMillis;
    CONTADOR_ESTADO_AUTOMATICO_MODO = 0;
    BANDERA_ESTADO_AUTOMATICO_MODO = 0;
    alerta_aire1_problemas = 0;

    CONTADOR_ESTADO_AUTOMATICO_MODO2 = 0;
    BANDERA_ESTADO_AUTOMATICO_MODO2 = 0;
    alerta_aire2_problemas = 0;
  }

}

void nuevas_alarmas2() {
  unsigned long currentMillis = 0;
  currentMillis = millis();
  if (estado_boton2 == HIGH && estado_boton1 == HIGH ) {
    if (currentMillis - TIEMPO_ALARMAS2_PASADO > TIEMPO_ALARMAS2) {
      alerta_ambos_prendidos = 1;
    }
  } else if ((estado_boton2 == HIGH && estado_boton1 == LOW) || (estado_boton2 == LOW && estado_boton1 == HIGH)) {
    TIEMPO_ALARMAS2_PASADO = currentMillis;
    alerta_ambos_prendidos = 0;
  }

}
////////////////////////////////////////////////////////
int modo_aire_auto() {
  unsigned long currentMillis = 0;
  int lcd_auto;
  currentMillis = millis();
  if (bandera_temperatura == 1) { /// SI ESTAN LOS DOS AIRES PRENDIDOS Y HACE CALOR
    //ESTADO_AUTOMATICO_MODO=3; // VARIABLE UTILIZADA PARA ALERTAS
    digitalWrite(RELAY_AIRE1, HIGH);
    digitalWrite(RELAY_AIRE2, HIGH);
    lcd_auto = 7;
    estado_boton2 = HIGH;
    estado_boton1 = HIGH;
    //EEPROM.update(4, ledstate2); // actualizar el valor del aire 
    //EEPROM.update(5, ledstate);
    return (lcd_auto);
  } else if (bandera_temperatura == 0)
  {

    if (ledstate == LOW)  // led state es el aire 2
    {
      lcd_auto = 5;
      ESTADO_AUTOMATICO_MODO = 1;
      //Serial.println("modo1");
      if (currentMillis - tiempo_pasado_intercalacion > intervalo_off_aires)
      {


        tiempo_pasado_intercalacion = currentMillis;
        ledstate = HIGH;

        ledstate2 = LOW;
        //

      }
    } else
    {
      lcd_auto = 6;
      ESTADO_AUTOMATICO_MODO = 2;
      if (currentMillis - tiempo_pasado_intercalacion > intervalo_on_aires)
      {

        tiempo_pasado_intercalacion = currentMillis;
        ledstate = LOW;

        ledstate2 = HIGH;


      }
    }

    digitalWrite(RELAY_AIRE1, ledstate2);
    digitalWrite(RELAY_AIRE2, ledstate);
    estado_boton2 = ledstate; /// variables por si pasa la modo manual siguen igual los aires
    estado_boton1 = ledstate2;
    //EEPROM.update(4, ledstate2);
    //EEPROM.update(5, ledstate);
    return (lcd_auto);
  } else if (bandera_temperatura == 2) {
    estado_boton2 = HIGH;
    estado_boton1 = HIGH;
    digitalWrite(RELAY_AIRE1, HIGH);
    digitalWrite(RELAY_AIRE2, HIGH);
    lcd_auto = 11;
    //EEPROM.update(4, ledstate2);
    //EEPROM.update(5, ledstate);
    return (lcd_auto);
  }

}

///////////////////////////////////////////////
int imprimir_pantalla(int numero) {

  if ( pantalla_cambio == 1) {
    pantalla_cambio = 0;

    switch (numero) {
      case 1:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("MODO MANUAL");
        lcd.setCursor(0, 1);
        lcd.print("A1 ON ");
        lcd.setCursor(7, 1);
        lcd.print("A2 ON ");
        break;
      case 2:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("MODO MANUAL");
        lcd.setCursor(0, 1);
        lcd.print("A1 OFF ");
        lcd.setCursor(7, 1);
        lcd.print("A2 OFF ");
        break;
      case 3:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("MODO MANUAL");
        lcd.setCursor(0, 1);
        lcd.print("A1 ON ");
        lcd.setCursor(7, 1);
        lcd.print("A2 OFF ");

        break;
      case 4:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("MODO MANUAL");
        lcd.setCursor(0, 1);
        lcd.print("A1 OFF ");
        lcd.setCursor(7, 1);
        lcd.print("A2 ON ");
        break;
      case 5:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("MODO AUTO");
        lcd.setCursor(0, 1);
        lcd.print("A1 ON ");
        lcd.setCursor(7, 1);
        lcd.print("A2 OFF ");
        break;
      case 6:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("MODO AUTO");
        lcd.setCursor(0, 1);
        lcd.print("A1 OFF ");
        lcd.setCursor(7, 1);
        lcd.print("A2 ON ");
        break;
      case 7:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("MODO AUTO");
        lcd.setCursor(0, 1);
        lcd.print("A1 ON ");
        lcd.setCursor(7, 1);
        lcd.print("A2 ON ");
        break;
      case 8:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("T. Conmutacion");
        lcd.setCursor(0, 1);
        lcd.print((intervalo_on_aires / 1000) / 60);
        lcd.setCursor(5, 1);
        lcd.print("MINUTO/S");
        break;
      case 9:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("TEMP. LIMITE");
        lcd.setCursor(0, 1);
        lcd.print("INFERIOR");
        lcd.setCursor(10, 1);
        lcd.print(temperatura_minima);
        lcd.setCursor(13, 1);
        lcd.print("C");
        break;

      case 10:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("TEMP. LIMITE");
        lcd.setCursor(0, 1);
        lcd.print("SUPERIOR");
        lcd.setCursor(10, 1);
        lcd.print(temperatura_maxima);
        lcd.setCursor(13, 1);
        lcd.print("C");
        break;

      case 11:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("SENSOR ERROR");
        lcd.setCursor(0, 1);
        lcd.print("Llame Servi.Tec");
        break;

      case 12:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("TEMP. ALARMA");
        lcd.setCursor(0, 1);
        lcd.print("ALARMA");
        lcd.setCursor(10, 1);
        lcd.print(temperatura_alarma); // IMPRIME LA TEMP ALARMA
        lcd.setCursor(13, 1);
        lcd.print("C");
        break;
    }

    /// pregunto si no esta en el menu que imprima la temperatura o en error //
    if (numero == 8 || numero == 9 || numero == 10 || numero == 11 || numero == 12) {
    } else {
      lcd.setCursor(11, 0);
      lcd.print(temperatura);

      lcd.setCursor(15, 0);
      lcd.print("C");
    }

  }



}

void base64() {
  //Serial.println("Base64 example");
  char charBuf[password.length() + 1];
  password.toCharArray(charBuf, password.length() + 1);
  //Serial.println("pass");
  //Serial.println(charBuf[7]);

  int inputLen = sizeof(charBuf);
  //Serial.println(inputLen);
  if (inputLen == 9) {
    verificacion_exitosa=0;
    error_8letras = 0;
    int encodedLen = base64_enc_len(inputLen);
    char encoded[encodedLen];
    EEPROM.update(9, charBuf[0]);
    EEPROM.update(10, charBuf[1]);
    EEPROM.update(11, charBuf[2]);
    EEPROM.update(12, charBuf[3]);
    EEPROM.update(13, charBuf[4]);
    EEPROM.update(14, charBuf[5]);
    EEPROM.update(15, charBuf[6]);
    EEPROM.update(16, charBuf[7]);

    //Serial.println("PASS");
    //Serial.println(charBuf[0]);
    //Serial.println(charBuf[1]);
    //Serial.println(charBuf[2]);
    //Serial.println(charBuf[3]);
    //Serial.println(charBuf[4]);
    //Serial.println(charBuf[5]);
    //Serial.println(charBuf[6]);
    //Serial.println(charBuf[7]);

    base64_encode(encoded, charBuf, inputLen);

    //Serial.println("encoded");
    //Serial.println(encoded);
    int tam = sizeof(encoded);
    encoded[tam - 1] = NULL;


    memset(password2, 0, sizeof(password2));
    memcpy(password2, encoded, sizeof(encoded[0])*inputLen + 3);

    //Serial.println("PASS");
    //Serial.println(password2[7]);
    // Serial.println(encoded);
    //Serial.println("tamanho");
    //Serial.println(sizeof(encoded));

  } else {
    verificacion_exitosa=1;
    error_8letras = 1;
  }
}

void inicializar_erom() {
  //// variables para el primer inicio del arduino cuando este en default
  //EEPROM.write(4, LOW);
  //EEPROM.write(5, HIGH);
  //EEPROM.write(6, HIGH);
  //EEPROM.write(7, HIGH);
  //EEPROM.write(8, HIGH);
  //EEPROM.write(0, 28);
  //EEPROM.write(1, 20);
  //EEPROM.write(3, 28);
  /// inicializo los valores en al erom
  temperatura_maxima = EEPROM.read(0);
  temperatura_minima = EEPROM.read(1);
  intervalo_on_aires = 3600000 * (EEPROM.read(2));
  intervalo_off_aires = intervalo_on_aires;
  //temperatura_alarma=EEPROM.read(3);
  temperatura_alarma = EEPROM.read(3);
  //ledstate2 = EEPROM.read(4);
  //ledstate = EEPROM.read(5);
  estado_boton1 = EEPROM.read(6);
  estado_boton2 = EEPROM.read(7);
  estado_boton3 = EEPROM.read(8);

  password[0] = EEPROM.read(9);
  password[1] = EEPROM.read(10);
  password[2] = EEPROM.read(11);
  password[3] = EEPROM.read(12);
  password[4] = EEPROM.read(13);
  password[5] = EEPROM.read(14);
  password[6] = EEPROM.read(15);
  password[7] = EEPROM.read(16);
  delay (500);
  ///oma


}
