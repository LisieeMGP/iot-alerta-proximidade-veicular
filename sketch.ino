#include <WiFi.h>
#include <PubSubClient.h>

#define TRIG_PIN 5
#define ECHO_PIN 18

#define LED_VERDE 25
#define LED_AMARELO 26
#define LED_VERMELHO 27

#define BUZZER_PIN 14

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

const char* TOPICO_DISTANCIA = "projeto/proximidade/distancia";
const char* TOPICO_STATUS = "projeto/proximidade/status";
const char* TOPICO_ECO = "projeto/proximidade/eco";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long inicioMQTT = 0;
unsigned long tempoMQTT = 0;

bool aguardandoEco = false;
String mensagemEco;


// Recebe a mensagem que voltou do broker
void callback(char* topic, byte* payload, unsigned int length) {

  String mensagem = "";

  for (unsigned int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }

  if (String(topic) == TOPICO_ECO &&
      aguardandoEco &&
      mensagem == mensagemEco) {

    tempoMQTT = micros() - inicioMQTT;
    aguardandoEco = false;

    Serial.print("Tempo MQTT ida e volta: ");
    Serial.print(tempoMQTT);
    Serial.println(" us");
  }
}


void conectarMQTT() {

  while (!client.connected()) {

    Serial.print("Conectando ao MQTT...");

    String clientId = "ESP32-Proximidade-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {

      Serial.println(" conectado!");

      // ESP32 escuta o tópico usado para medir o retorno
      client.subscribe(TOPICO_ECO);

    } else {

      Serial.print(" erro=");
      Serial.println(client.state());

      delay(2000);
    }
  }
}


void setup() {

  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);

  pinMode(BUZZER_PIN, OUTPUT);


  Serial.print("Conectando ao WiFi");

  WiFi.begin(ssid, password, 6);

  while (WiFi.status() != WL_CONNECTED) {

    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi conectado!");

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  Serial.println("Sistema iniciado");
}


void loop() {

  if (!client.connected()) {
    conectarMQTT();
  }

  client.loop();


  // =========================
  // LEITURA DO SENSOR
  // =========================

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  long duracao = pulseIn(ECHO_PIN, HIGH, 30000);

  Serial.print("Tempo HC-SR04: ");
  Serial.print(duracao);
  Serial.println(" us");

  float distancia;

  if (duracao == 0) {
    distancia = 999;
  } else {
    distancia = duracao / 58.0;
  }


  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AMARELO, LOW);
  digitalWrite(LED_VERMELHO, LOW);

  String status;

  unsigned long inicioAtuador;
  unsigned long fimAtuador;


  // =========================
  // NORMAL
  // =========================

  if (distancia > 100) {

    status = "NORMAL";

    inicioAtuador = micros();

    digitalWrite(LED_VERDE, HIGH);

    fimAtuador = micros();

    Serial.print("Tempo LED VERDE: ");
    Serial.print(fimAtuador - inicioAtuador);
    Serial.println(" us");

    noTone(BUZZER_PIN);
  }


  // =========================
  // ATENÇÃO
  // =========================

  else if (distancia >= 50) {

    status = "ATENCAO";

    inicioAtuador = micros();

    digitalWrite(LED_AMARELO, HIGH);

    fimAtuador = micros();

    Serial.print("Tempo LED AMARELO: ");
    Serial.print(fimAtuador - inicioAtuador);
    Serial.println(" us");


    unsigned long inicioBuzzer = micros();

    tone(BUZZER_PIN, 1000);

    unsigned long fimBuzzer = micros();

    Serial.print("Tempo BUZZER: ");
    Serial.print(fimBuzzer - inicioBuzzer);
    Serial.println(" us");

    delay(150);
    noTone(BUZZER_PIN);
  }


  // =========================
  // CRÍTICO
  // =========================

  else {

    status = "PROXIMIDADE_CRITICA";

    inicioAtuador = micros();

    digitalWrite(LED_VERMELHO, HIGH);

    fimAtuador = micros();

    Serial.print("Tempo LED VERMELHO: ");
    Serial.print(fimAtuador - inicioAtuador);
    Serial.println(" us");


    unsigned long inicioBuzzer = micros();

    tone(BUZZER_PIN, 1500);

    unsigned long fimBuzzer = micros();

    Serial.print("Tempo BUZZER: ");
    Serial.print(fimBuzzer - inicioBuzzer);
    Serial.println(" us");

    delay(150);
    noTone(BUZZER_PIN);
  }


  // =========================
  // PUBLICAÇÃO NORMAL MQTT
  // =========================

  char distanciaTexto[10];

  dtostrf(
    distancia,
    1,
    2,
    distanciaTexto
  );

  client.publish(
    TOPICO_DISTANCIA,
    distanciaTexto
  );

  client.publish(
    TOPICO_STATUS,
    status.c_str()
  );


  // =========================
  // TESTE DE TEMPO MQTT
  // =========================

  mensagemEco = String(millis());

  aguardandoEco = true;

  inicioMQTT = micros();

  client.publish(
    TOPICO_ECO,
    mensagemEco.c_str()
  );


  // Espera a mensagem voltar do broker
  unsigned long limite = millis();

  while (aguardandoEco &&
         millis() - limite < 2000) {

    client.loop();
  }


  // =========================
  // RESULTADO
  // =========================

  Serial.println();

  Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.println(" cm");

  Serial.print("Status: ");
  Serial.println(status);

  Serial.println("--------------------------");

  delay(1000);
}