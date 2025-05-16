#include <math.h>

const int B = 4275000;
const int R0 = 100000;
const int pinTempSensor = A0;

const int DURATION_SECONDS = 180;          // 3 minutes = 180 seconds
const float SAMPLING_INTERVAL = 1.0;       // 1 Hz = 1 sample/sec (Active Mode)
const int SAMPLE_COUNT = DURATION_SECONDS; // 180 samples for Active Mode

float temperatureData[SAMPLE_COUNT];
float frequencyComponents[SAMPLE_COUNT / 2];
float magnitudeSpectrum[SAMPLE_COUNT / 2];

void setup() {
  Serial.begin(9600);
  delay(2000); // Allow time for Serial Monitor
  collect_temperature_data();
  apply_dft(temperatureData, SAMPLE_COUNT, frequencyComponents, magnitudeSpectrum);
  send_data_to_pc();
  decide_power_mode();
}

void loop() {
  // One-shot execution, or restart if needed
}

// Task 3.1 - Collect temperature for 3 minutes at 1 Hz
void collect_temperature_data() {
  Serial.println("Collecting temperature data for 3 minutes...");

  for (int i = 0; i < SAMPLE_COUNT; i++) {
    int a = analogRead(pinTempSensor);
    float R = 1023.0 / a - 1.0;
    R = R0 * R;
    float temperature = 1.0 / (log(R / R0) / B + 1 / 298.15) - 273.15;
    temperatureData[i] = temperature;
    delay(SAMPLING_INTERVAL * 1000);
  }

  Serial.println("Data collection complete.");
}

// Task 3.2 - Apply DFT on temperature data
void apply_dft(float* signal, int N, float* frequencies, float* magnitudes) {
  float Fs = 1.0 / SAMPLING_INTERVAL;
  int k_max = N / 2;

  for (int k = 0; k < k_max; k++) {
    float real = 0, imag = 0;

    for (int n = 0; n < N; n++) {
      float angle = 2 * PI * k * n / N;
      real += signal[n] * cos(angle);
      imag -= signal[n] * sin(angle);
    }

    real /= N;
    imag /= N;

    magnitudes[k] = sqrt(real * real + imag * imag); // Eq. 3.5
    frequencies[k] = k * Fs / N;                     // Eq. 3.2
  }
}

// Task 3.3 - Send data via Serial Monitor
void send_data_to_pc() {
  Serial.println("Time,Temperature,Frequency,Magnitude");

  for (int i = 0; i < SAMPLE_COUNT / 2; i++) {
    float time = i * SAMPLING_INTERVAL;
    float temp = (i < SAMPLE_COUNT) ? temperatureData[i] : 0;
    Serial.print(time, 2);
    Serial.print(",");
    Serial.print(temp, 2);
    Serial.print(",");
    Serial.print(frequencyComponents[i], 4);
    Serial.print(",");
    Serial.println(magnitudeSpectrum[i], 4);
  }
}

// Task 3.4 - Decide power mode based on average frequency
void decide_power_mode() {
  float avgFreq = 0;
  for (int i = 1; i < SAMPLE_COUNT / 2; i++) { // ignore DC at k=0
    avgFreq += frequencyComponents[i];
  }
  avgFreq /= (SAMPLE_COUNT / 2 - 1);

  Serial.print("Average Frequency: ");
  Serial.println(avgFreq, 4);

  if (avgFreq > 0.5) {
    Serial.println("Power Mode: ACTIVE (High sampling rate)");
  } else if (avgFreq > 0.1) {
    Serial.println("Power Mode: IDLE (Medium sampling rate)");
  } else {
    Serial.println("Power Mode: POWER DOWN (Low sampling rate)");
  }
}