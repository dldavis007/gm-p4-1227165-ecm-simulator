CC = gcc
CFLAGS = -std=c89 -Wall -Wextra -pedantic
TARGET = build/bua_step117
OUTPUT = build/bua_step117_output.txt

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
	grep -q "step-106 listing-coolant regression result: PASS (12/12)" $(OUTPUT)
	grep -q "step-107 diagnostic-stage1 regression result: PASS (17/17)" $(OUTPUT)
	grep -q "step-108 diagnostic-qualification regression result: PASS (22/22)" $(OUTPUT)
	grep -q "Step-109 integrated Segment-D signature: F357A5F2" $(OUTPUT)
	grep -q "step-109 Segment-D integration regression result: PASS (40/40)" $(OUTPUT)
	grep -q "Step-110 integrated output-stage signature: FAADF8A6" $(OUTPUT)
	grep -q "step-110 Segment-1 output regression result: PASS (22/22)" $(OUTPUT)
	grep -q "Step-111 ignition lifecycle signature: 16D17C9C" $(OUTPUT)
	grep -q "step-111 ignition shutdown regression result: PASS (40/40)" $(OUTPUT)
	grep -q "step-112 reset/startup regression result: PASS (19/19)" $(OUTPUT)
	grep -q "step-113 retained-memory regression result: PASS (10/10)" $(OUTPUT)
	grep -q "step-114 normal-startup regression result: PASS (14/14)" $(OUTPUT)
	grep -q "step-115 SCI regression result: PASS (20/20)" $(OUTPUT)
	grep -q "step-116 Mode-4 lifecycle regression result: PASS (16/16)" $(OUTPUT)
	grep -q "step-117 factory-test control regression result: PASS (24/24)" $(OUTPUT)
	@if grep "FAIL" $(OUTPUT) | grep -v "FAIL 0"; then exit 1; fi
	@echo "Step-104/105 baselines and Step-106 through Step-117 regressions pass."

clean:
	rm -f $(TARGET) $(OUTPUT) build/compiler_warnings.txt
