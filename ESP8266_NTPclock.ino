/*  ESP822 acessando o relógio da WEB
    Arduino IDE 1.8.5 - ESP8266
    Gustavo Murta   08/mar/2018
    baseado em https://github.com/arduino-libraries/NTPClient/blob/master/examples/Advanced/Advanced.ino
    Blog Eletrogate http://blog.eletrogate.com/nodemcu-esp12-usando-arduino-ide-2/
*/

#include <NTPClient.h>          // Biblioteca https://github.com/arduino-libraries/NTPClient
#include <ESP8266WiFi.h>        // Biblioteca https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi
#include <WiFiUdp.h>            // Biblioteca https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi

const char *ssid     = "roteador WIFI";       // nome do seu roteador WIFI (SSID)
const char *password = "senha do WIFI";       // senha do roteador WIFI

WiFiUDP ntpUDP;
// Definindo o Servidor de relogio NTP Brasil, ajusta relogio UTC -3 horas, intervalo de atualizacao em milisegundos
NTPClient timeClient(ntpUDP, "gps.ntp.br", -3 * 3600, 60000);

void setup()
{
  Serial.begin(115200);                     // print no Serial Monitor da IDE
  WiFi.begin(ssid, password);               // acessando a rede WIFI

  while ( WiFi.status() != WL_CONNECTED )   // aguardando a conexão WEB
  {
    delay ( 500 );                          // aguarda 0,5 segundos
    Serial.print ( "." );
  }
  timeClient.begin();
}

void loop()
{
  timeClient.update();                              // atualiza o relogio
  Serial.println(timeClient.getFormattedTime());    // print do relogio da WEB
  delay(1000);                                      // atraso de um segundo
}
