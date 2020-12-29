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

// Function assumes that BCD IC pins are connected to MCU pins contiguously (e.g. 2, 3, 4, 5)
void switchDigit(const int aPin, const int digit) {
	if (digit >= 0 && digit < 10) {
		for (int i = 0; i < 4; ++i) {
			digitalWrite(aPin + i, digit & lround(pow(2, i)));
		}
	}
}

void setup() {
	// put your setup code here, to run once:
	for (int i{0}; i < 4; ++i) {
		pinMode(FIRST_DIGIT_A_PIN + i, OUTPUT);
		pinMode(SECOND_DIGIT_A_PIN + i, OUTPUT);
	}

	for (int i{9}; i >= 0; --i) {
		switchDigit(FIRST_DIGIT_A_PIN, i);
		switchDigit(SECOND_DIGIT_A_PIN, i);
		delay(500);
	}

	Serial.begin(9600);
	sensors.begin();
}

void loop() {
	// put your main code here, to run repeatedly:
	sensors.requestTemperatures();
	auto temp = sensors.getTempCByIndex(0);

	auto roundedTemp = lround(temp);
	if (temp != DEVICE_DISCONNECTED_C && roundedTemp != currentTemp) {
		currentTemp = roundedTemp;
		// Displaying negative temperatures as a subtraction from 100 (e.g. -1 degree displays as 99)
		if (roundedTemp < 0) {
			roundedTemp += 100;
		}

		switchDigit(FIRST_DIGIT_A_PIN, roundedTemp / 10);
		switchDigit(SECOND_DIGIT_A_PIN, roundedTemp % 10);
	}

	delay(1000);
}
