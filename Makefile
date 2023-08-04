BOARD ?= arduino:avr:uno

BUILD_PATH ?= build
INCLUDE_PATH ?= include

SERIAL_PORT ?= COM3

.PHONY: upload

build: Modulador2.ino
	arduino-cli compile -b $(BOARD) --build-path $(BUILD_PATH) --build-cache-path $(BUILD_PATH) --libraries $(INCLUDE_PATH)

upload: $(BUILD_PATH)
	arduino-cli upload --input-dir "$(BUILD_PATH)" --fqbn $(BOARD) -p $(SERIAL_PORT)

clear:
	rmdir $(BUILD_PATH) /S