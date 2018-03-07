#include "ESP8266WiFi.h"

void setup()
{
  Serial.begin(115200);      // configura monitor serial 115200 Bps
  Serial.println();          // imprime uma linha 
  WiFi.mode(WIFI_STA);       // configura rede no modo estacao 
  WiFi.disconnect();         // desconecta rede WIFI
  delay(100);                // atraso de 100 milisegundos
}

void prinScanResult(int networksFound)
{
  Serial.printf("\n");                                      // imprime uma linha 
  Serial.printf("%d redes encontradas\n", networksFound);   // imprime numero de redes encontradas
  for (int i = 0; i < networksFound; i++)                   // contagem das redes encontradas
  {
    Serial.printf("%d: %s, Ch:%d (%ddBm) %s\n", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == ENC_TYPE_NONE ? "aberta" : "");
  }
}

void loop()
{
  WiFi.scanNetworksAsync(prinScanResult);        // imprime o resultado 
  delay(500);                                    // atraso de 0,5 segundos
}
