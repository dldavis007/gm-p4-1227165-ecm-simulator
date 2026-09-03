CC = gcc
CFLAGS = -std=c89 -Wall -Wextra -pedantic
TARGET = build/bua_step105
OUTPUT = build/bua_step105_output.txt

.PHONY: all run test clean

all: $(TARGET)

$(TARGET): main.c src/*.inc.h simulation/*.inc.h tests/*.inc.h
	mkdir -p build
	$(CC) $(CFLAGS) main.c -o $(TARGET)

run: $(TARGET)
	$(TARGET)

test: $(TARGET)
	$(TARGET) > $(OUTPUT)
	grep -q "Step-104 normal-operation behavioral signature: 4BA6B7C6" $(OUTPUT)
	grep -q "step-104 baseline-freeze regression result: PASS (10/10)" $(OUTPUT)
	grep -q "Step-105 transmission trace signature: 9732D09B" $(OUTPUT)
	grep -q "step-105 transmission-aware regression result: PASS (12/12)" $(OUTPUT)
	@if grep "FAIL" $(OUTPUT) | grep -v "FAIL 0"; then exit 1; fi
	@echo "Step-104 and Step-105 behavioral baselines pass."

clean:
	rm -f $(TARGET) $(OUTPUT) build/compiler_warnings.txt
