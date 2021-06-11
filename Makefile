# Makefile to build an Arduino project
# Thanks to <http://theonlineshed.com/arduino-without-the-ide-an-intro-to-unix-make>
# Determine path of arduino core libraries
ARDUINO_HW  := /usr/share/arduino/hardware
# On Arch Linux, the path is /usr/share/arduino/hardware/archlinux-arduino, so
# we use a wildcard to handle this case
# TODO: allow overriding ARDUINO_LIB from env variables
ARDUINO_LIB := $(firstword $(wildcard ${ARDUINO_HW}/*arduino))

# Arduino core libraries
VPATH       := \
	${ARDUINO_LIB}/avr/cores/arduino \
	${ARDUINO_LIB}/avr/libraries/Wire/src \

VARIANTS    := ${ARDUINO_LIB}/avr/variants/standard
# Only used for VSCode Intellisense configuration
AVR_INC     := /usr/avr/include

# Include all subdirectories from libraries we need
VPATH       := $(foreach path, ${VPATH}, ${path} $(shell find ${path} -type d))

# Add the local directory into VPATH. Potential local subdirectories also go here
# This is to prevent the command above from including directories like .git into
# the VPATH list
VPATH       += ${PWD} \
	${PWD}/ArduinoMIDI/src \
	${PWD}/TimerCounter \

# Build parameters
BUILD_DIR   := out
CC          := avr-gcc
CXX         := avr-g++
OBJCOPY     := avr-objcopy
AVRDUDE     := avrdude
MMCU        := -mmcu=atmega328p
INCS        := \
	$(foreach path, ${VPATH}, -I ${path}) \
	$(foreach path, ${VARIANTS}, -I ${path}) \

CFLAGS      := -O3 -DF_CPU=16000000UL -DARDUINO=1000 ${MMCU} ${INCS}
# Generate sections for each function and variable
# so that LD can eliminate unused functions and variables
CFLAGS      += -ffunction-sections -fdata-sections
# GCC LD options to eliminate unused sections
LDFLAGS     := ${MMCU} -Wl,--gc-sections

# List all source files
SOURCES := $(foreach path, $(VPATH), \
	$(wildcard ${path}/*.c) $(wildcard ${path}/*.cpp) $(wildcard ${path}/*.S))
# Remove main.cpp from the Arduino core libraries (instead of the local project)
# This allows us to use the proper main() function instead of setup() and loop()
SOURCES := $(filter-out ${ARDUINO_HW}%main.cpp, ${SOURCES})
# Strip path from the file names, since they are in VPATH, which means
# we don't need to include the full paths
SOURCES := $(foreach file, ${SOURCES}, $(notdir ${file}))
# Translate everything to target object files
OBJS    := ${SOURCES}
OBJS    := $(patsubst %.c, ${BUILD_DIR}/%.o, ${OBJS})
OBJS    := $(patsubst %.cpp, ${BUILD_DIR}/%.o, ${OBJS})
OBJS    := $(patsubst %.S, ${BUILD_DIR}/%_asm.o, ${OBJS})

PROGRAM     := arduino-otp

# Build rules for different source types
${BUILD_DIR}/%.o: %.c pitch_table.h
	mkdir -p ${BUILD_DIR}
	${CC} ${CFLAGS} -c $< -o $@ 
 
${BUILD_DIR}/%.o: %.cpp pitch_table.h
	mkdir -p ${BUILD_DIR}
	${CXX} ${CFLAGS} -c $< -o $@ 

${BUILD_DIR}/%_asm.o: %.S
	mkdir -p ${BUILD_DIR}
	${CC} ${CFLAGS} -c $< -o $@ 

${BUILD_DIR}/${PROGRAM}.elf: ${OBJS}
	${CC} ${LDFLAGS} $^ -o $@
 
${BUILD_DIR}/${PROGRAM}.hex: ${BUILD_DIR}/${PROGRAM}.elf
	${OBJCOPY} -O ihex -R .eeprom $< $@

pitch_table.h: gen_pitch_table.py
	python gen_pitch_table.py > pitch_table.h

define to-json-array
	$(shell echo '$(foreach path, $1, "${path}",)' | sed "s/,$$//")
endef

# vscode intellisense configuration file
# note that vscode will automatically format the generated file
.vscode/c_cpp_properties.json: c_cpp_properties.json.template
	mkdir -p .vscode
	sed 's@__includePaths__@$(call to-json-array, ${AVR_INC} ${VPATH} ${VARIANTS})@' c_cpp_properties.json.template > .vscode/c_cpp_properties.json

.PHONY: all clean upload vscode

all: ${BUILD_DIR}/${PROGRAM}.hex

clean:
	rm -rf ${BUILD_DIR}
	rm -rf pitch_table.h

upload: all
ifndef PORT
	$(error PORT is undefined)
endif
	${AVRDUDE} -F -V -c arduino -p ATMEGA328P -P ${PORT} -b 115200 -D -U flash:w:${BUILD_DIR}/${PROGRAM}.hex

vscode:
	rm -rf .vscode/c_cpp_properties.json
	make .vscode/c_cpp_properties.json