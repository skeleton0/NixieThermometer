#include <OneWire.h>
#include <DallasTemperature.h>
#include <math.h>

// Pins
#define ONE_WIRE_BUS 10
#define FIRST_DIGIT_A_PIN 2
#define SECOND_DIGIT_A_PIN 6

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
long int currentTemp{0};
unsigned long lastCycle;
const unsigned long CYCLE_INTERVAL{1000ul * 60 * 30};

// Function assumes that BCD IC pins are connected to MCU pins contiguously (e.g. 2, 3, 4, 5)
void switchDigit(const int aPin, const int digit) {
	if (digit >= 0 && digit < 10) {
		for (int i = 0; i < 4; ++i) {
			digitalWrite(aPin + i, digit & lround(pow(2, i)));
		}
	}
}

void cycleDigits() {
	for (int i{9}; i >= 0; --i) {
		switchDigit(FIRST_DIGIT_A_PIN, i);
		switchDigit(SECOND_DIGIT_A_PIN, i);
		delay(1000);
	}
}

void displayCurrentTemp() {
	// Displaying negative temperatures as a subtraction from 100 (e.g. -1 degree displays as 99)
	auto adjustedTemp = currentTemp < 0 ? currentTemp + 100 : currentTemp;
	switchDigit(FIRST_DIGIT_A_PIN, currentTemp / 10);
	switchDigit(SECOND_DIGIT_A_PIN, currentTemp % 10);
}

void setup() {
	// put your setup code here, to run once:
	for (int i{0}; i < 4; ++i) {
		pinMode(FIRST_DIGIT_A_PIN + i, OUTPUT);
		pinMode(SECOND_DIGIT_A_PIN + i, OUTPUT);
	}

	cycleDigits();
	lastCycle = millis();

	sensors.begin();
}

void loop() {
	// put your main code here, to run repeatedly:
	if (millis() - lastCycle > CYCLE_INTERVAL) {
		cycleDigits();
		lastCycle = millis();
		// Redisplay previous temperature
		displayCurrentTemp();
	}

	sensors.requestTemperatures();
	auto temp = sensors.getTempCByIndex(0);

	auto roundedTemp = lround(temp);
	if (temp != DEVICE_DISCONNECTED_C && roundedTemp != currentTemp) {
		currentTemp = roundedTemp;
		displayCurrentTemp();
	}

	delay(30000);
}
