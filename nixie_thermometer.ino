#include <OneWire.h>
#include <DallasTemperature.h>
#include <math.h>

// Pins
#define ONE_WIRE_BUS 10
#define FIRST_DIGIT_A_PIN 2
#define SECOND_DIGIT_A_PIN 6

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
long int currentTemp = 0;

// Function assumes that BCD IC pins are connected to MCU pins contiguously (e.g. 2, 3, 4, 5)
void switchDigit(const int aPin, const int digit) {
	if (digit >= 0 && digit < 10) {
		for (int i = 0; i < 4; ++i) {
			digitalWrite(aPin + i, digit & lround(pow(2, i)));
			Serial.print("i: ");
			Serial.print(i);
			Serial.print(", digit: ");
			Serial.print(digit);
			Serial.print(", power: ");
			Serial.println(lround(pow(2, i)));

			Serial.print("Setting pin ");
			Serial.print(aPin + i);
			Serial.print(" to ");
			Serial.println(digit & lround(pow(2, i)));
		}
	}
}

void setup() {
	// put your setup code here, to run once:
	for (int i = 0; i < 4; ++i) {
		pinMode(FIRST_DIGIT_A_PIN + i, OUTPUT);
		pinMode(SECOND_DIGIT_A_PIN + i, OUTPUT);
	}

	switchDigit(FIRST_DIGIT_A_PIN, 0);
	switchDigit(SECOND_DIGIT_A_PIN, 0);

	Serial.begin(9600);
	sensors.begin();
}

void loop() {
	// put your main code here, to run repeatedly:
	Serial.print("Requesting termperatures...");
	sensors.requestTemperatures();
	Serial.println("Done");

	auto temp = sensors.getTempCByIndex(0);
	Serial.print("Sensor returned: ");
	Serial.println(temp);

	auto roundedTemp = lround(temp);
	if (temp != DEVICE_DISCONNECTED_C && roundedTemp != currentTemp) {
		currentTemp = roundedTemp;
		// Displaying negative temperatures as a subtraction from 100 (e.g. -1 degree displays as 99)
		if (roundedTemp < 0) {
			roundedTemp += 100;
		}

		auto firstDigit = roundedTemp / 10;
		auto secondDigit = roundedTemp % 10;

		Serial.print("First digit: ");
		Serial.println(firstDigit);
		Serial.print("Second digit: ");
		Serial.println(secondDigit);

		switchDigit(FIRST_DIGIT_A_PIN, firstDigit);
		switchDigit(SECOND_DIGIT_A_PIN, secondDigit);
	}

	delay(1000);
}
