#include <Wire.h>
#include <Adafruit_ADS1X15.h>

// --- Declaração de Objeto e Constantes Globais ---
Adafruit_ADS1115 ads; // Objeto para o sensor ADS1115

const float BURDEN_RESISTOR_OHMS = 22.0f;        // Valor do resistor de carga (burden) em Ohms
const float SCT_TURNS_RATIO = 2000.0f;           // Relação de espiras do SCT (ex: 2000 para 100A:50mA) - VERIFICAR MODELO!
const float FINAL_CALIBRATION_ADJUSTMENT = 1.0f; // Fator de ajuste fino para calibração final (começar com 1.0)

const float V_REF_UNUSED_FOR_BIAS_REMOVAL = 3.3f; // Tensão de referência (não usada ativamente se a remoção de bias abaixo estiver comentada)

const int NUM_SAMPLES = 1000;    // Número de amostras para cálculo RMS
unsigned long interval = 1000; // Intervalo entre medições em milissegundos
unsigned long lastMillis = 0;    // Variável para controle de tempo

void setup() {
  // --- Configurações Iniciais (Setup) ---
  Serial.begin(115200); // Inicializa a comunicação serial

  // Inicializa o sensor ADS1115
  if (!ads.begin()) { // Endereço padrão 0x48 (ADDR no GND)
    Serial.println("Falha ao inicializar o ADS.");
    while (1); // Trava se não conseguir inicializar
  }
  Serial.println("ADS1115 inicializado com sucesso!");

  // Configura o ganho do ADS1115
  // GAIN_SIXTEEN para faixa de +/- 0.256V (ideal para sinais pequenos, como V_pico ~17mV no seu caso)
  ads.setGain(GAIN_SIXTEEN); // +/- 0.256V de range. Ajuste se necessário.
  
  // Imprime o cabeçalho para dados CSV via Serial
  Serial.println("timestamp_ms,ultima_leitura_bruta,ultima_tensao_instant_mV,corrente_RMS_A");
}

void loop() {
  // --- Loop Principal ---

  // Controle de tempo para executar a medição no intervalo definido
  if (millis() - lastMillis >= interval) {
    lastMillis = millis(); // Atualiza o tempo da última medição
    
    // --- Aquisição de Amostras ---
    float sumOfSquares = 0;           // Soma dos quadrados das tensões instantâneas
    int16_t rawValue;                 // Valor bruto lido do ADC
    float instantaneousVoltage_V;     // Tensão instantânea em Volts
    
    for (int i = 0; i < NUM_SAMPLES; i++) {
      rawValue = ads.readADC_Differential_0_1();         // Lê o valor diferencial entre AIN0 e AIN1
      instantaneousVoltage_V = ads.computeVolts(rawValue); // Converte o valor bruto para Volts (considerando o ganho)

      // A linha abaixo para remover bias DC está comentada, pois o sinal do SCT já deve ser AC puro.
      // Mantenha comentada a menos que tenha certeza que um bias DC precisa ser removido aqui.
      // instantaneousVoltage_V -= V_REF_UNUSED_FOR_BIAS_REMOVAL / 2.0f; 

      sumOfSquares += instantaneousVoltage_V * instantaneousVoltage_V; // Acumula o quadrado da tensão
    }

    // --- Cálculo da Tensão RMS ---
    float meanSquare = sumOfSquares / NUM_SAMPLES;      // Calcula a média dos quadrados
    float rmsVoltage_V = sqrt(meanSquare);              // Calcula a raiz quadrada da média (tensão RMS no burden)

    // --- Cálculo da Corrente Primária RMS ---
    float Isecondary_rms_A = rmsVoltage_V / BURDEN_RESISTOR_OHMS;    // Corrente RMS no secundário do SCT
    float Iprimary_rms_A_raw = Isecondary_rms_A * SCT_TURNS_RATIO; // Corrente RMS no primário (bruta)
    float currentRMS_A = Iprimary_rms_A_raw * FINAL_CALIBRATION_ADJUSTMENT; // Corrente RMS primária com ajuste fino

    // --- Saída de Dados via Serial ---
    Serial.print(millis());                             // Imprime o timestamp atual
    Serial.print(",");
    Serial.print(rawValue);                             // Imprime a última leitura bruta do ADC
    Serial.print(",");
    Serial.print(instantaneousVoltage_V * 1000.0f, 4);  // Imprime a última tensão instantânea em mV
    Serial.print(",");
    Serial.println(currentRMS_A, 3);                    // Imprime a corrente RMS primária calculada em Ampères
  }
}