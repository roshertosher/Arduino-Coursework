#include <Arduino.h>
#include <math.h>

#define MAX_BUFFER_SIZE 240    // Max samples at 4Hz over 60 seconds
#define MAX_HISTORY 10         // For moving average

float tempReadings[MAX_BUFFER_SIZE];
float variationLog[MAX_HISTORY];
int cycleIndex = 0;

float currentSamplingRate = 2.0;  // Initial rate in Hz
const float MIN_SAMPLING_RATE = 0.5;
const float MAX_SAMPLING_RATE = 4.0;

enum PowerState { STATE_ACTIVE, STATE_IDLE, STATE_POWER_DOWN };
PowerState powerState = STATE_ACTIVE;
int consecutiveIdleCycles = 0;

// Sensor reading: LM35 or TMP36 on A0
float readTemperatureCelsius() {
  int adcValue = analogRead(A0);
  float voltage = adcValue * (5.0 / 1023.0); // Adjust if using 3.3V
  float temperatureC = voltage * 100.0;      // For LM35: 10mV per °C
  return temperatureC;
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  int totalSamples = 60 * currentSamplingRate;
  totalSamples = constrain(totalSamples, 1, MAX_BUFFER_SIZE);

  sampleTemperature(totalSamples, currentSamplingRate);

  float variationNow = calculateVariation(totalSamples);
  recordVariation(variationNow);

  float dominantFrequency = computeDFT(totalSamples);
  float predictedVariation = computeMovingAverage(variationLog, cycleIndex);

  powerState = evaluatePowerState(predictedVariation);
  currentSamplingRate = updateSamplingRate(dominantFrequency, powerState);

  logSystemStatus(predictedVariation, dominantFrequency, currentSamplingRate, powerState);

  delay(1000); // Wait 1 second before next cycle
}

void sampleTemperature(int samples, float rate) {
  for (int i = 0; i < samples; i++) {
    tempReadings[i] = readTemperatureCelsius();
    delay((unsigned long)(1000.0 / rate));
  }
}

float calculateVariation(int samples) {
  float totalVariation = 0;
  for (int i = 1; i < samples; i++) {
    totalVariation += abs(tempReadings[i] - tempReadings[i - 1]);
  }
  return totalVariation;
}

void recordVariation(float newVariation) {
  if (cycleIndex < MAX_HISTORY) {
    variationLog[cycleIndex++] = newVariation;
  } else {
    for (int i = 0; i < MAX_HISTORY - 1; i++) {
      variationLog[i] = variationLog[i + 1];
    }
    variationLog[MAX_HISTORY - 1] = newVariation;
  }
}

float computeDFT(int N) {
  float peakMagnitude = 0;
  int peakIndex = 1;
  for (int k = 1; k < N / 2; k++) {
    float real = 0, imag = 0;
    for (int n = 0; n < N; n++) {
      float angle = 2 * PI * k * n / N;
      real += tempReadings[n] * cos(angle);
      imag -= tempReadings[n] * sin(angle);
    }
    float magnitude = sqrt(real * real + imag * imag);
    if (magnitude > peakMagnitude) {
      peakMagnitude = magnitude;
      peakIndex = k;
    }
  }
  return (float)peakIndex / 60.0;  // Frequency in Hz
}

float computeMovingAverage(float *data, int count) {
  float sum = 0;
  int validEntries = min(count, MAX_HISTORY);
  for (int i = 0; i < validEntries; i++) {
    sum += data[i];
  }
  return sum / validEntries;
}

PowerState evaluatePowerState(float avgVariation) {
  const float ACTIVE_THRESHOLD = 3.0;
  const float IDLE_THRESHOLD = 1.0;

  if (avgVariation > ACTIVE_THRESHOLD) {
    consecutiveIdleCycles = 0;
    return STATE_ACTIVE;
  } else if (avgVariation > IDLE_THRESHOLD) {
    consecutiveIdleCycles = 0;
    return STATE_IDLE;
  } else {
    consecutiveIdleCycles++;
    if (consecutiveIdleCycles >= 5) {
      return STATE_POWER_DOWN;
    }
    return STATE_IDLE;
  }
}

float updateSamplingRate(float freq, PowerState state) {
  float updatedRate = max(2.0 * freq, MIN_SAMPLING_RATE);
  if (state == STATE_POWER_DOWN) return MIN_SAMPLING_RATE;
  if (state == STATE_IDLE) return constrain(updatedRate, MIN_SAMPLING_RATE, 1.5);
  return constrain(updatedRate, MIN_SAMPLING_RATE, MAX_SAMPLING_RATE);
}

void logSystemStatus(float variation, float frequency, float rate, PowerState state) {
  Serial.print("Predicted Variation: ");
  Serial.print(variation, 2);
  Serial.print(", Dominant Frequency: ");
  Serial.print(frequency, 2);
  Serial.print(" Hz, Sampling Rate: ");
  Serial.print(rate, 2);
  Serial.print(" Hz, Power Mode: ");
  if (state == STATE_ACTIVE) Serial.println("ACTIVE");
  else if (state == STATE_IDLE) Serial.println("IDLE");
  else Serial.println("POWER_DOWN");
}