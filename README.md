# Sistema IoT para detecção frontal de obstáculos e alerta de proximidade veicular

Projeto desenvolvido para a disciplina de Objetos Inteligentes Conectados do curso de Análise e Desenvolvimento de Sistemas da Universidade Presbiteriana Mackenzie.

O protótipo utiliza um ESP32, um sensor ultrassônico HC-SR04, LEDs e um buzzer para identificar diferentes níveis de proximidade de um obstáculo localizado à frente de um veículo representado em ambiente de simulação. Os dados de distância e o estado do sistema também são enviados pela Internet utilizando o protocolo MQTT.

O projeto está relacionado ao Objetivo de Desenvolvimento Sustentável 3, especialmente à meta 3.6, relacionada à redução de mortes e ferimentos causados por acidentes de trânsito.

## Funcionamento

O sensor HC-SR04 realiza continuamente a medição da distância entre o sensor e um obstáculo.

O ESP32 processa a distância obtida e classifica o estado do sistema em três faixas:

- Distância superior a 100 cm: estado `NORMAL`, LED verde ligado e buzzer desligado.
- Distância entre 50 cm e 100 cm: estado `ATENCAO`, LED amarelo ligado e buzzer com sinais sonoros intermitentes.
- Distância inferior a 50 cm: estado `PROXIMIDADE_CRITICA`, LED vermelho ligado e buzzer com sinais sonoros mais frequentes.

As distâncias utilizadas possuem finalidade demonstrativa e não representam distâncias reais de segurança veicular.

## Hardware utilizado

| Componente | Quantidade | Função |
|---|---:|---|
| ESP32 | 1 | Processamento e conexão Wi-Fi |
| HC-SR04 | 1 | Medição da distância frontal |
| LED verde | 1 | Indicação de condição normal |
| LED amarelo | 1 | Indicação de atenção |
| LED vermelho | 1 | Indicação de maior proximidade |
| Buzzer piezoelétrico | 1 | Alerta sonoro |
| Resistores | 3 | Limitação de corrente dos LEDs |
| Protoboard | 1 | Representação da montagem |
| Cabos jumper | Conforme necessário | Interligação dos componentes |

## Conexões

### HC-SR04

- VCC → 5 V
- GND → GND
- TRIG → GPIO 5
- ECHO → GPIO 18

### LEDs

- LED verde → GPIO 25
- LED amarelo → GPIO 26
- LED vermelho → GPIO 27

Os LEDs utilizam resistores limitadores de corrente.

### Buzzer

- Terminal positivo → GPIO 14
- Terminal negativo → GND

Todos os componentes utilizam referência comum de GND.

## Software

O software foi desenvolvido em C/C++ utilizando ambiente compatível com Arduino para ESP32.

O código realiza:

- leitura do sensor HC-SR04;
- cálculo da distância;
- classificação do nível de proximidade;
- acionamento dos LEDs;
- controle do buzzer;
- conexão à rede Wi-Fi;
- conexão ao broker MQTT;
- publicação dos valores de distância;
- publicação do estado do sistema;
- medições auxiliares de tempo de execução e comunicação.

O código-fonte está disponível no arquivo:

`sketch.ino`

A montagem utilizada no Wokwi está definida no arquivo:

`diagram.json`

## Ambiente de simulação

O protótipo foi desenvolvido e testado na plataforma Wokwi.

A rede virtual utilizada para acesso à Internet foi:

`Wokwi-GUEST`

O Wokwi permite alterar manualmente a distância simulada do HC-SR04 durante a execução, possibilitando testar as três condições de proximidade do sistema.

## Comunicação com a Internet

O ESP32 utiliza sua interface Wi-Fi integrada para comunicação via TCP/IP com um broker MQTT.

### Broker MQTT

Broker público HiveMQ:

`broker.hivemq.com`

Porta:

`1883`

### Cliente MQTT

Para validar o recebimento das mensagens foi utilizado o MQTTX.

## Tópicos MQTT

### Distância

`projeto/proximidade/distancia`

Publica o valor da distância medida pelo sensor HC-SR04.

### Estado

`projeto/proximidade/status`

Valores possíveis:

- `NORMAL`
- `ATENCAO`
- `PROXIMIDADE_CRITICA`

### Tópico auxiliar de teste

`projeto/proximidade/eco`

Esse tópico foi utilizado exclusivamente para medir o tempo de ida e volta das mensagens pelo broker MQTT.

## Bibliotecas utilizadas

O projeto utiliza as seguintes bibliotecas principais:

- `WiFi.h`
- `PubSubClient.h`

A biblioteca PubSubClient é utilizada para implementação da comunicação MQTT.

## Como reproduzir o projeto

1. Acesse o Wokwi.
2. Crie um projeto utilizando ESP32.
3. Copie o conteúdo de `sketch.ino` para o arquivo `sketch.ino` do projeto.
4. Copie o conteúdo de `diagram.json` para o arquivo `diagram.json`.
5. Adicione a biblioteca `PubSubClient`.
6. Inicie a simulação.
7. Aguarde a conexão do ESP32 à rede `Wokwi-GUEST`.
8. Aguarde a conexão ao broker HiveMQ.
9. Altere a distância do sensor HC-SR04 para testar as diferentes condições.
10. Opcionalmente, conecte o MQTTX ao broker `broker.hivemq.com` e assine o tópico:

`projeto/proximidade/#`

Dessa forma, será possível visualizar as mensagens de distância, estado e teste publicadas pelo ESP32.

## Testes realizados

Foram testadas três condições principais:

| Condição | Distância aproximada | LED | Buzzer |
|---|---:|---|---|
| Normal | 153 cm | Verde | Desligado |
| Atenção | 82 cm | Amarelo | Intermitente |
| Proximidade crítica | 31,4 cm | Vermelho | Intermitente mais frequente |

Também foram realizadas quatro medições do sensor, dos atuadores e da comunicação MQTT para avaliação dos tempos de resposta e execução.

Os tempos médios de comunicação MQTT de ida e volta pelo broker foram aproximadamente:

- Normal: 172,69 ms
- Atenção: 174,82 ms
- Proximidade crítica: 182,19 ms

## Limitações

O protótipo utiliza apenas a distância medida pelo sensor ultrassônico HC-SR04 e não identifica o tipo de obstáculo.

As faixas utilizadas são exclusivamente demonstrativas e não devem ser interpretadas como distâncias reais de segurança veicular.

O sistema também foi desenvolvido em ambiente de simulação e utiliza um broker MQTT público, podendo apresentar variações nos tempos de comunicação.

## Arquivos do repositório

`README.md` — documentação do projeto.

`sketch.ino` — código-fonte do ESP32.

`diagram.json` — representação do circuito utilizado no Wokwi.
