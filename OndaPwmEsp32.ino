// Configuração do PWM
const int pwmPin = 16;              // Pino de saída PWM no ESP32
const int pwmChannel = 0;           // Canal do PWM 
const int pwmFreq = 5000;           // Frequência do PWM (filtrada com RC)
const int pwmResolution = 8;        // Resolução do PWM em bits (8 bits)
const int maxDuty = 255;            // Valor máximo do duty cycle (100% em 8 bits)


// Parâmetros da forma de onda desejada
const float f = 90.0;               // Frequência da onda final
const float T = 1000000.0 / f;      // ~11.111 µs

const int stepsSenoide = 40;        // Quantidade de passos para gerar a parte senoidal
const int stepsNivelAlto = 80;      // Passos com duty fixo em 100%
const int stepsRampa = 40;          // Passos para descer linearmente até 0%

void setup() {
  // Configura o canal PWM do ESP32
  ledcSetup(pwmChannel, pwmFreq, pwmResolution);

  // Conecta o canal PWM ao pino desejado
  ledcAttachPin(pwmPin, pwmChannel);
}

void loop() {
  
  //calcula o tempo que cada parte vai ser executado
  
  unsigned long cicloStart = micros(); // Marca o início de um ciclo da onda
  int totalSteps = stepsSenoide + stepsNivelAlto + stepsRampa; // Calcula o número total de passos em um ciclo completo
  float stepTime = T / totalSteps; // Duração de cada passo em microssegundos (para dividir os ~11ms do ciclo total) 

  for (int i = 0; i < totalSteps; i++) {
    int duty;

    if (i < stepsSenoide) {
      
      // Fase 1: Senoide
      
      float progress = (float)i / (stepsSenoide - 1);
      float angle = progress * PI / 2.0;
      duty = int(sin(angle) * maxDuty);
    } else if (i < stepsSenoide + stepsNivelAlto) {
      
      // Fase 2: Nível constante
      
      duty = maxDuty;
    } else {
      
      // Fase 3: Rampa
      
      float progress = (float)(i - stepsSenoide - stepsNivelAlto) / (stepsRampa - 1);
      duty = int((1.0 - progress) * maxDuty);
    }

    // Aplica o duty calculado ao canal PWM, gera o PWM
    ledcWrite(pwmChannel, duty);

    // Espera ativa até o tempo do próximo passo
    unsigned long targetTime = cicloStart + i * stepTime;
    while (micros() < targetTime) {
      // Espera
    }
  }

  // Espera passiva para sincronizar com exatidão ao final do ciclo
  while (micros() - cicloStart < T) {
    // Espera até completar exatamente 1 ciclo de 11.111 µs
  }
}
