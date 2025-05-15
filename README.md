Student ID: F423738
My project : microcontroller and an analog temperature sensor (like LM35 or TMP36). The system dynamically adjusts its sampling rate and power mode based on real-time analysis of temperature variation and signal characteristics.

Key Objectives
Efficiently monitor ambient temperature.

Analyze temperature trends in real-time.

Conserve power through adaptive sampling and mode switching.

Detect periodic patterns using frequency analysis (DFT).

Core Functionalities
Temperature Sampling

Reads analog temperature data from a sensor.

Collects data at a configurable sampling rate (0.5 to 4 Hz).

Stores readings in a buffer for analysis.

Variation Calculation

Measures the short-term variation in temperature by summing absolute differences between consecutive samples.

Stores recent variations to track trends over time.

Digital Signal Analysis (DFT)

Performs a Discrete Fourier Transform (DFT) on the collected data to identify the dominant frequency of temperature fluctuations.

This helps detect periodic environmental changes (like HVAC cycles or outdoor temperature oscillations).

Adaptive Sampling Rate

Adjusts the sampling frequency dynamically based on the detected frequency content.

Ensures higher resolution during active temperature changes and lower resolution during steady-state conditions to save power.

Power Mode Management

Predicts future temperature activity using a moving average of recent variation values.

Switches between three power states:

ACTIVE: frequent sampling for high variation.

IDLE: moderate sampling for low variation.

POWER_DOWN: minimal activity during prolonged stability.

System Feedback

Continuously prints system status to the Serial Monitor:

Predicted variation

Dominant frequency

Adjusted sampling rate

Current power mode

Tasks Completed
Designed and implemented analog temperature reading logic.

Buffered real-time sensor data for temporal analysis.

Developed a variation tracking and moving average system.

Implemented a basic DFT to extract frequency components.

Created an adaptive control algorithm for sampling rate.

Designed a finite state machine for power mode transitions.

Integrated serial monitoring for debugging and analysis.

Potential Applications
Environmental monitoring

Smart thermostats

Energy-efficient sensor networks

IoT devices with power constraints
