CC = gcc
CFLAGS = -std=c89 -Wall -Wextra -pedantic
TARGET = build/bua_step121
OUTPUT = build/bua_step121_output.txt

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
	grep -q "step-118 factory-test execution regression result: PASS (31/31)" $(OUTPUT)
	grep -q "step-119 vector-boundary regression result: PASS (19/19)" $(OUTPUT)
	grep -q "step-120 power-on-dispatch regression result: PASS (24/24)" $(OUTPUT)
	grep -q "step-121 ignition-lifecycle integration result: PASS (18/18)" $(OUTPUT)
	grep -q "step-128 HAL-interface regression result: PASS (15/15)" $(OUTPUT)
	grep -q "step-155 named-input HAL regression result: PASS (4/4)" $(OUTPUT)
	grep -q "step-156 CTS/MAT HAL regression result: PASS (4/4)" $(OUTPUT)
	grep -q "step-157 remaining-U10 HAL regression result: PASS (5/5)" $(OUTPUT)
	grep -q "step-158 normal-FMD HAL regression result: PASS (7/7)" $(OUTPUT)
	grep -q "step-159 raw-HAL lifecycle regression result: PASS (8/8)" $(OUTPUT)
	grep -q "Step-160 full raw-HAL lifecycle signature: 2E92563C" $(OUTPUT)
	grep -q "step-160 full raw-HAL lifecycle regression result: PASS (13/13)" $(OUTPUT)
	grep -q "Step-161 raw-HAL output lifecycle signature: B22B98ED" $(OUTPUT)
	grep -q "step-161 raw-HAL output lifecycle regression result: PASS (9/9)" $(OUTPUT)
	grep -q "Step-162 raw-VOLT output-safety signature: DB86F332" $(OUTPUT)
	grep -q "step-162 raw-VOLT output-safety regression result: PASS (10/10)" $(OUTPUT)
	grep -q "Step-163 raw-HAL power-on signature: 040E3A8B" $(OUTPUT)
	grep -q "step-163 raw-HAL power-on regression result: PASS (10/10)" $(OUTPUT)
	grep -q "Step-164 raw-HAL factory IRQ signature: 0194B153" $(OUTPUT)
	grep -q "step-164 raw-HAL factory IRQ regression result: PASS (10/10)" $(OUTPUT)
	grep -q "Step-165 raw-HAL factory A/D signature: 0EACFF4F" $(OUTPUT)
	grep -q "step-165 raw-HAL factory A/D regression result: PASS (9/9)" $(OUTPUT)
	grep -q "Step-166 8192-baud SCI raw-HAL signature: 52396771" $(OUTPUT)
	grep -q "step-166 8192-baud SCI raw-HAL regression result: PASS (9/9)" $(OUTPUT)
	grep -q "step-168 diagnostic-160 manager regression result: PASS (6/6)" $(OUTPUT)
	@if grep "FAIL" $(OUTPUT) | grep -v "FAIL 0"; then exit 1; fi
	@echo "Step-104/105 baselines, Step-106 through Step-121 regressions, and Step-128/155/156/157/158/159/160/161/162/163/164/165/166 HAL regressions pass."

clean:
	rm -f $(TARGET) $(OUTPUT) build/compiler_warnings.txt
