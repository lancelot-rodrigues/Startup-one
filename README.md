# EnergiClima

**EnergiClima** é uma solução integrada de hardware e software para previsão horária de consumo de energia em prédios comerciais, combinando:

- Aquisição de corrente em tempo real via Arduino + ADS1115 + transformador de corrente (SCT-013)  
- Pipeline de Machine Learning (LightGBM, XGBoost, LSTM) para previsão horária baseada em dados IoT e variáveis climáticas :contentReference[oaicite:0]{index=0}:contentReference[oaicite:1]{index=1}

---

## Conteúdo do Repositório

- `hardware/`  
  – Código-fonte Arduino para medição de corrente RMS via ADS1115  
- `software/data/`  
  – Scripts de ingestão e pré-processamento de dados IoT e meteorológicos  
- `software/models/`  
  – Notebooks e scripts de treinamento e avaliação de modelos (GB, XGBoost, LSTM)  
- `docs/`  
  – Documentação complementar, referências e estudo de fontes de dados  

---

## 1. Contexto e Motivação

Prédios comerciais demandam previsão precisa de consumo energético para:
- Reduzir desperdício e custos operacionais  
- Diminuir pegada de carbono e atender ODS da ONU (7, 9, 11, 12 e 13) :contentReference[oaicite:2]{index=2}:contentReference[oaicite:3]{index=3}  
- Planejar picos de demanda e otimizar contratos de energia  

A crise climática e a crescente demanda por climatização tornam essencial antecipar variações de carga hora a hora :contentReference[oaicite:4]{index=4}:contentReference[oaicite:5]{index=5}.

---

## 2. Componentes de Hardware

1. **Placa Arduino** (Uno, Nano ou equivalente)  
2. **Módulo ADS1115** (I²C, endereço padrão 0x48) – ADC 16 bits  
3. **Transformador de corrente SCT-013** (ex: 2000 : 50 mA)  
4. **Resistor de burden** (ex: 22 Ω)  
5. Fios, protoboard e fonte de alimentação 5 V  

---

## 3. Firmware Arduino

### Principais constantes (em `hardware/measure_current.ino`):

```cpp
const float BURDEN_RESISTOR_OHMS = 22.0f;
const float SCT_TURNS_RATIO       = 2000.0f;
const float FINAL_CALIBRATION_ADJ = 1.0f;
const int   NUM_SAMPLES           = 1000;
unsigned long interval            = 1000; // ms