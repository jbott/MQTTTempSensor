BOARD_TAG    = uno # Actual target is dfrobot x-board v2
ARDUINO_LIBS = \
	DallasTemperature \
	Ethernet \
	OneWire \
	PString \
	PubSubClient \
	SPI

include ../Arduino-Makefile/Arduino.mk

