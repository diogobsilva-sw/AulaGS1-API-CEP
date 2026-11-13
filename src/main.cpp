#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>

void conexaoWifi();
void reConexaoWifi();
String conAPICEP(String cep);

const char *SSID = "SENAI EDUCACIONAL";
const char *SENHA = "info@134";

const unsigned long tempoConexao = 10000;
const unsigned long tempoReConexao = 5000;

const String API = "https://brasilapi.com.br/api/cep/v2/";

void setup()
{
  Serial.begin(115200);

  conexaoWifi();

  if (WiFi.status() == WL_CONNECTED)
  {
    String cep;

    Serial.println("\nDigite o CEP:");

    while (cep == "")
    {
      if (Serial.available())
      {
        cep = Serial.readStringUntil('\n');
        cep.trim();
      }
    }

    if (cep.length() != 8)
    {
      Serial.println("CEP invalido! Digite exatamente 8 digitos.");
      return;
    }

    for (int i = 0; i < cep.length(); i++)
    {
      if (!isDigit(cep[i]))
      {
        Serial.println("CEP invalido! Digite apenas numeros.");
        return;
      }
    }

    String respostaApiCep = conAPICEP(cep);

    if (respostaApiCep != "")
    {
      JsonDocument doc;

      DeserializationError erro = deserializeJson(doc, respostaApiCep);

      if (!erro)
      {
        String rua = doc["street"] | "Nao informado";
        String bairro = doc["neighborhood"] | "Nao informado";
        String cidade = doc["city"] | "Nao informado";
        String estado = doc["state"] | "Nao informado";

        Serial.println();
        Serial.println("DADOS DO ENDERECO");
        Serial.println("-----------------");
        Serial.println("CEP: " + cep);
        Serial.println("Rua: " + rua);
        Serial.println("Bairro: " + bairro);
        Serial.println("Cidade: " + cidade);
        Serial.println("Estado: " + estado);
      }
      else
      {
        Serial.println("Erro ao interpretar o JSON");
      }
    }
  }
}

void loop()
{
  reConexaoWifi();
}

String conAPICEP(String cep)
{
  HTTPClient http;
  http.begin(API + cep);

  int codHTTP = http.GET();
  String resposta = "";

  if (codHTTP == 200)
  {
    resposta = http.getString();
  }
  else if (codHTTP == 404)
  {
    Serial.println("CEP nao encontrado!");
  }
  else if (codHTTP < 0)
  {
    Serial.println("Erro na requisicao HTTP: " + String(codHTTP));
  }
  else
  {
    Serial.println("Erro. Codigo HTTP: " + String(codHTTP));
  }

  http.end();
  return resposta;
}

void conexaoWifi()
{
  Serial.printf("Conectando ao Wi-Fi: %s\n", SSID);

  WiFi.begin(SSID, SENHA);

  unsigned long tempoWifi = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - tempoWifi < tempoConexao)
  {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nWi-Fi conectado com sucesso!");
    Serial.print("Endereco IP: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("\nFalha ao se conectar.");
  }
}

void reConexaoWifi()
{
  static unsigned long tempoUltimaConexao = 0;

  unsigned long tempoAtual = millis();

  if (tempoAtual - tempoUltimaConexao >= tempoReConexao)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("Conexao Wi-Fi perdida!");
      conexaoWifi();
    }

    tempoUltimaConexao = millis();
  }
}