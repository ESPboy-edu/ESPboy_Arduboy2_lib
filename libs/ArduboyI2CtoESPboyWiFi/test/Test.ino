/*
MIT License

Copyright (c) 2024-2026 sub1inear

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <Arduboy2.h>
#define I2C_IMPLEMENTATION
#include <ArduboyI2C.h>

#undef assert
#define assert(cond, a, b) do { \
		if (!(cond)) { \
			Serial.print(F("Assertion failed: " #cond ", ")); \
			Serial.print(a); \
			Serial.print(F(", ")); \
			Serial.println(b); \
			return false; \
		} \
	} while (0)
#define assert_eq(a, b) do { \
		if ((a) != (b)) { \
			Serial.print(F("Assertion failed: " #a " == " #b ", ")); \
			Serial.print(a); \
			Serial.print(F(", ")); \
			Serial.println(b); \
			return false; \
		} \
	} while (0)
#define assert_cb(cond, var, assigna, assignb) do { \
		if (!(cond)) { \
			var = F(#cond); \
			assigna; \
			assignb; \
		} \
	} while (0)
#define assert_cb_ok(var, a, b) do { \
	    if (var) { \
			Serial.print(F("Assertion failed: ")); \
			Serial.print(var); \
			Serial.print(F(", ")); \
			Serial.print(a); \
			Serial.print(F(", ")); \
			Serial.println(b); \
			return false; \
		} \
	} while (0)


constexpr uint8_t bufferExpected[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

Arduboy2 arduboy;

volatile bool readCallbackCalled = false;

volatile bool writeCallbackCalled = false;
const __FlashStringHelper *volatile writeCallbackError = nullptr;
volatile int writeCallbackA = 0;
volatile uint8_t writeCallbackB = 0;

bool allTestsPassed = true;

void displayTest(const __FlashStringHelper *name, bool result) {
	arduboy.print(name);
	arduboy.print(F(": "));
	if (result) {
		arduboy.println(F("PASS"));
	} else {
		arduboy.println(F("FAIL"));
		allTestsPassed = false;
	}
	arduboy.display();
}

void displayRole(I2C::Role role) {
	int16_t cursorX = arduboy.getCursorX();
	int16_t cursorY = arduboy.getCursorY();

	arduboy.setCursor(WIDTH - 1 * 5, 0);

	if (role == I2C::Role::Controller) {
		arduboy.print(F("C"));
	} else {
		arduboy.print(F("T"));
	}

	arduboy.setCursor(cursorX, cursorY);
}

bool testBeginEnd() {
	I2C::begin();
	assert_eq(PRR0 & _BV(PRTWI), 0);
	assert_eq(TWCR, (_BV(TWEN) | _BV(TWIE) | _BV(TWEA)));
	assert_eq(TWSR, 248); // 0b11111000 -> TW idle, prescaler = 0
	assert_eq(TWBR, (F_CPU / I2C_FREQUENCY - 16) / 2);
	assert_eq((I2C_PORT & (_BV(I2C_SDA_BIT) | _BV(I2C_SCL_BIT))), (_BV(I2C_SDA_BIT) | _BV(I2C_SCL_BIT)));
	assert_eq((I2C_DDR & (_BV(I2C_SDA_BIT) | _BV(I2C_SCL_BIT))), 0);
	I2C::end();
	assert_eq(TWCR, 0);
	assert_eq(PRR0 & _BV(PRTWI), _BV(PRTWI));
	assert_eq((I2C_PORT & (_BV(I2C_SDA_BIT) | _BV(I2C_SCL_BIT))), 0);
	I2C::begin();
	return true;
}

bool testAddress() {
	I2C::setAddress(0x34);
	assert_eq(TWAR, 0x34 << 1);
	assert_eq(I2C::getAddress(), 0x34);
	return true;
}

void testReadCallback() {
	I2C::reply(bufferExpected);
	readCallbackCalled = true;
}

bool testRead(I2C::Role role) {
	if (role == I2C::Role::Controller) {
		uint8_t buffer[16];
		I2C::read(I2C::targetAddress, buffer);
		assert_eq((uint8_t)I2C::getError(), (uint8_t)I2C::Error::None);
		assert_eq(memcmp(buffer, bufferExpected, 16), 0);
	} else {
		uint32_t start = millis();
		while (!readCallbackCalled) {
			if (millis() - start > 1000) {
				Serial.println(F("Read callback timeout"));
				return false;
			}
		}
	}
	return true;
}

uint8_t debugBuffer[16];

void testWriteCallback() {
	writeCallbackCalled = true;
	assert_cb(I2C::getBufferSize() == 16, writeCallbackError, writeCallbackA = I2C::getBufferSize(), writeCallbackB = 16);

	int memcmpResult = memcmp(I2C::getBuffer(), bufferExpected, 16);
	assert_cb(memcmpResult == 0, writeCallbackError, writeCallbackA = memcmpResult, writeCallbackB = 0);
	memcpy(debugBuffer, I2C::getBuffer(), 16);
}

bool testWrite(I2C::Role role) {
	if (role == I2C::Role::Controller) {
		I2C::write(I2C::targetAddress, bufferExpected, I2C::Mode::Sync);
		assert_eq((uint8_t)I2C::getError(), (uint8_t)I2C::Error::None);
	} else {
		uint32_t start = millis();
		while (!writeCallbackCalled) {
			if (millis() - start > 1000) {
				Serial.println(F("Write callback timeout"));
				return false;
			}
		}
		for (uint8_t i = 0; i < 16; i++) {
			Serial.println(debugBuffer[i]);
		}
		assert_cb_ok(writeCallbackError, writeCallbackA, writeCallbackB);
	}
	return true;
}

bool testCheckCableFlipped() {
	I2C::checkCableFlipped([]() {});
	return true;
}

bool testHandshake() {
	return true;
}

void setup() {
    arduboy.begin();
	arduboy.clear();

	while (!Serial) {
		if (arduboy.anyPressed(A_BUTTON | B_BUTTON)) {
			break;
		}
	}
	arduboy.waitNoButtons();

	displayTest(F("beginEnd"), testBeginEnd());
	displayTest(F("setAddress"), testAddress());
	displayTest(F("ccFlipped"), testCheckCableFlipped());

	I2C::Role role = I2C::handshake();
	I2C::onReceive(testWriteCallback);
	I2C::onRequest(testReadCallback);

	displayRole(role);

	displayTest(F("handshake"), testHandshake());

	displayTest(F("read"), testRead(role));
	displayTest(F("write"), testWrite(role));

	if (allTestsPassed) {
		arduboy.digitalWriteRGB(GREEN_LED, RGB_ON);
	} else {
		arduboy.digitalWriteRGB(RED_LED, RGB_ON);
	}

}

void loop() {
	if (arduboy.anyPressed(A_BUTTON | B_BUTTON)) {
		arduboy.exitToBootloader();
	}
}
