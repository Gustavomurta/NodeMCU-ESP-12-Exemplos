/*  ESP822 - NodeMCU Alarme Residencial IOT
    Arduino IDE 1.8.5 - ESP8266
    Gustavo Murta   14/mar/2018
    Baseado em:
    https://github.com/witnessmenow/push-notifications-arduino-esp8266/blob/master/PushNotificaitonDemo.ino
    https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/blob/master/examples/ESP8266/FlashLED/FlashLED.ino

    Blog Eletrogate:
    http://blog.eletrogate.com/nodemcu-esp12-alarme-residencial-iot-3/
    
    Você pode copiar, distribuir e modificar o software desde que as modificações sejam descritas e licenciadas gratuitamente em LGPL-3.
    As obras derivadas (incluindo modificações ou qualquer coisa vinculada estaticamente à biblioteca) só podem ser redistribuídas no LGPL-3,
    mas as aplicações que utilizam a biblioteca não precisam ser.
*/


#include <UniversalTelegramBot.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// Configurando a conexao WIFI com o Roteador
char ssid[] = "roteador WIFI";        // nome do seu roteador WIFI (SSID)
char password[] = "senha do WIFI";    // senha do roteador WIFI

#define TELEGRAM_SENSOR_PIN D5       // Sensor de disparo do alarme => D5 = GPIO_14

// Inicializa o BOT Telegram - copie aqui a chave Token quando configurou o seu BOT - entre aspas
#define BOT_TOKEN "123456789:ABCDEFGHIJKLMOPQRST-ABCDEFGHIJKLMNO"          // sua chave Token Bot

#define CHAT_ID "123456789" // Para obter o Chat ID, acesse Telegram => usuario IDBot => comando /getid
               
// cliente SSL necessario para a Biblioteca
WiFiClientSecure client;

UniversalTelegramBot bot(BOT_TOKEN, client);

String ipAddress = "";
volatile bool telegramSensorPressedFlag = false;
const int ledPin = 16;    // GPIO_16 do LED azul
int alarmeAtive = 0;      // Alarme Ativado
int alarmeTriggered = 0;  // Alarme disparado
int Bot_mtbs = 1000;      // tempo entre a leitura das mensagens
long Bot_lasttime;        // ultima mensagem lida
//bool Start = false;

void setup()
{
  Serial.begin(115200);

  // Inicializa o botao e o LED
  pinMode(TELEGRAM_SENSOR_PIN, INPUT);      // define o Sensor de disparo como entrada
  pinMode(ledPin, OUTPUT);                  // configura o pino do LED como saida
  delay(10);
  digitalWrite(ledPin, HIGH);               // inicializa o LED como apagado

  // interrupcao no Sensor pino D5 dispara Alarme
  attachInterrupt(TELEGRAM_SENSOR_PIN, telegramSensorPressed, RISING);

  WiFi.mode(WIFI_STA);      // Configura o WIFI do NodeMCU para modo estação
  WiFi.disconnect();        // desconecta o WIFI
  delay(100);               // atraso de 100 milisegundos

  Serial.print("Conectando no Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)    // aguardando a conexão WEB
  {
    Serial.print(".");
    delay(500);                            // atraso de 0,5 segundos
  }
  Serial.println("");
  Serial.println("WiFi conectado !");        // WIFI conectado
  Serial.print("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);                      // imprime Endereco IP
  ipAddress = ip.toString();

}

void telegramSensorPressed()
{
  if  (alarmeAtive)          // Se o alarme estiver ativado
  {
    Serial.println("Alarme disparado!");
    int sensor = digitalRead(TELEGRAM_SENSOR_PIN);  // verifica estado do sensor
    if (sensor == HIGH)
    {
      telegramSensorPressedFlag = true;     // Sensor do Alarme foi acionado
      digitalWrite(ledPin, LOW);            // acende LED azul do Alarme
      alarmeTriggered = 1;                  // Alarme disparado
    }
  }
  return;
}

void sendTelegramMessage() {
  String message = "Alarme Residencial disparado !";
  message.concat("\n");
  message.concat("Zona 1 - portão da garagem");
  message.concat("\n");
  message.concat("SSID: ");
  message.concat(ssid);
  message.concat(" IP: ");
  message.concat(ipAddress);
  message.concat("\n");
  if (bot.sendMessage(CHAT_ID, message, "Markdown"))
  {
    Serial.println("Mensagem Telegram enviada com sucesso");
  }
  telegramSensorPressedFlag = false;
}

void handleNewMessages(int numNewMessages)
{
  Serial.print("Mensagem recebida = ");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    if (text == "/alarmeon")                                //  comando Ativa Alarme
    {
      alarmeAtive = 1;                                      // ativa alarme
      alarmeTriggered = 0;                                  // desliga disparo do Alarme
      bot.sendMessage(chat_id, "Alarme foi Ativado !", "");     // envia mensagem
    }

    if (text == "/alarmeoff")                               //  comando Desativa Alarme
    {
      alarmeAtive = 0;                                      // desativa alarme
      alarmeTriggered = 0;                                  // desliga disparo do Alarme
      digitalWrite(ledPin, HIGH);                           // apaga LED azul Alarme
      bot.sendMessage(chat_id, "Alarme foi Desativado !", "");  // envia mensagem
    }

    if (text == "/status")                                  // comando estado do Alarme
    {
      if (alarmeAtive)                                      // Se o alarme estiver ativado
      {
        bot.sendMessage(chat_id, "Alarme está Ativado", "");
      }
      else
      {
        bot.sendMessage(chat_id, "Alarme está Desativado", "");
      }
      if (alarmeTriggered) bot.sendMessage(chat_id, "Alarme está Disparado !", "");
    }

    if (text == "/start")                                  // comando comeca
    {
      alarmeAtive = 1;                                      // ativa alarme
      String welcome = "Alarme Residencial IOT operacional, " + from_name + ".\n";
      welcome += "Alarme foi Ativado!\n\n";
      welcome += "/alarmeon : para ativar Alarme\n";
      welcome += "/alarmeoff : para desativar Alarme\n";
      welcome += "/status : mostra o estado do Alarme\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}

void loop()
{
  if ( telegramSensorPressedFlag )
  {
    sendTelegramMessage();
  }
  if (millis() > Bot_lasttime + Bot_mtbs)    // controlando as mensagens
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)                  // numero de novas mensagens
    {
      Serial.println("Mensagem recebida do Telegram");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    Bot_lasttime = millis();
  }
}
