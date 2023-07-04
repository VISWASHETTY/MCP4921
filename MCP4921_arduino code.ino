#include <SPI.h>

// SPI pins
const int SPI_SCK_PIN = 18;     // GPIO2 (GP2) as SPI SCK
const int SPI_MOSI_PIN = 19;    // GPIO3 (GP3) as SPI MOSI
const int SPI_CS_PIN = 17;      // GPIO4 (GP4) as SPI CS

// MCP4921 commands
const byte CMD_WRITE = 0x30;   // Write to DAC with Vref buffered

// Reference voltage for MCP4921 (in Volts)
const float REFERENCE_VOLTAGE = 5.5;

// Define MCP4921 input range
const int DAC_MIN_VALUE = 0;
const int DAC_MAX_VALUE = 4095;

// Define initial MCP4921 value
int dac_value = 4095;

// Function to write data to MCP4921
void write_mcp4921(int value) {
  byte highByte = CMD_WRITE | ((value >> 8) & 0x0F);
  byte lowByte = value & 0xFF;

  digitalWrite(SPI_CS_PIN, LOW);    // Set CS pin low to start SPI transfer
  SPI.transfer(highByte);
  SPI.transfer(lowByte);
  digitalWrite(SPI_CS_PIN, HIGH);   // Set CS pin high to end SPI transfer
}

// Function to calculate output voltage based on digital input value
float calculate_output_voltage(int digital_input) {
  return (digital_input / (float)DAC_MAX_VALUE) * REFERENCE_VOLTAGE;
}

void setup() {
  pinMode(SPI_SCK_PIN, OUTPUT);
  pinMode(SPI_MOSI_PIN, OUTPUT);
  pinMode(SPI_CS_PIN, OUTPUT);

  SPI.begin();

  write_mcp4921(dac_value);

  Serial.begin(9600); // Initialize the Serial communication
}

void loop() {
  if (Serial.available()) {
    char key = Serial.read(); // Read the input from the Serial monitor

    if (key == 'a') {
      if (dac_value < DAC_MAX_VALUE) {
        dac_value++;
      }
    } else if (key == 's') {
      if (dac_value > DAC_MIN_VALUE) {
        dac_value--;
      }
    }
  }

  write_mcp4921(dac_value);
  delay(200);  // Delay for smooth transition

  float output_voltage = calculate_output_voltage(dac_value);
  float input_percentage = (dac_value / (float)DAC_MAX_VALUE) * 100.0;

  Serial.print("Output Voltage: ");
  Serial.println(output_voltage, 3); // Print with 3 decimal places

  Serial.print("Input Percentage: ");
  Serial.println(input_percentage, 2); // Print with 2 decimal places

  delay(1000);  // Delay before next iteration
}
