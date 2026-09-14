# Step history index

This index links the maintained development record from the frozen modular
baseline through behavioral closure and documentation consolidation. Numbered
audits remain historical checkpoints; the current conclusion is summarized in
[Current project status](CURRENT_PROJECT_STATUS.md).

## Behavioral integration and closure, Steps 104-122

- [104 — baseline freeze](STEP104_BASELINE_FREEZE.txt) and [modular refactor audit](STEP104_MODULAR_REFACTOR_AUDIT.txt)
- [105 — transmission-aware simulation](STEP105_TRANSMISSION_AUDIT.txt)
- [106 — listing coverage](STEP106_LISTING_COVERAGE_AUDIT.txt)
- [107 — diagnostics stage 1](STEP107_DIAGNOSTICS_STAGE1_AUDIT.txt)
- [108 — diagnostic qualification](STEP108_DIAGNOSTIC_QUALIFICATION_AUDIT.txt)
- [109 — Segment-D integration](STEP109_SEGMENT_D_INTEGRATION_AUDIT.txt)
- [110 — whole-image coverage](STEP110_WHOLE_IMAGE_COVERAGE_AUDIT.txt)
- [111 — ignition shutdown](STEP111_IGNITION_SHUTDOWN_AUDIT.txt)
- [112 — reset/startup](STEP112_RESET_STARTUP_AUDIT.txt)
- [113 — retained memory](STEP113_RETAINED_MEMORY_AUDIT.txt)
- [114 — normal startup](STEP114_NORMAL_STARTUP_AUDIT.txt)
- [115 — 8192-baud SCI](STEP115_SCI_8192_AUDIT.txt)
- [116 — Mode-4 lifecycle](STEP116_MODE4_LIFECYCLE_AUDIT.txt)
- [117 — factory-test control](STEP117_FACTORY_TEST_CONTROL_AUDIT.txt)
- [118 — factory-test execution](STEP118_FACTORY_TEST_EXECUTION_AUDIT.txt)
- [119 — vector boundaries](STEP119_VECTOR_BOUNDARY_AUDIT.txt)
- [120 — power-on integration](STEP120_POWER_ON_INTEGRATION_AUDIT.txt)
- [121 — lifecycle integration](STEP121_LIFECYCLE_INTEGRATION_AUDIT.txt)
- [122 — original whole-image closure](STEP122_WHOLE_IMAGE_CLOSURE_AUDIT.txt)

## Evidence and hardware cross-reference, Steps 123-139

- [123 — evidence inventory](STEP123_LIBRARY_EVIDENCE_INVENTORY.txt)
- [124 — controlled evidence import](STEP124_CONTROLLED_EVIDENCE_IMPORT.txt)
- [125 — historical package audit](STEP125_HISTORICAL_PACKAGE_AUDIT.txt)
- [126 — MEMCAL/source provenance closure](STEP126_MEMCAL_AND_SOURCE_PROVENANCE_CLOSURE_AUDIT.txt)
- [127 — hardware/HAL fidelity](STEP127_HARDWARE_HAL_FIDELITY_AUDIT.txt)
- [128 — explicit HAL interface](STEP128_EXPLICIT_HAL_INTERFACE_AUDIT.txt)
- [129 — MEMCAL functional networks](STEP129_MEMCAL_FUNCTIONAL_NETWORK_AUDIT.txt)
- [130 — MEMCAL/motherboard/firmware cross-reference](STEP130_MEMCAL_MOTHERBOARD_FIRMWARE_CROSS_REFERENCE_AUDIT.txt)
- [131 — ignition/injection signal chain](STEP131_IGNITION_INJECTION_SIGNAL_CHAIN_AUDIT.txt)
- [132 — MAP/load signal chain](STEP132_MAP_LOAD_SIGNAL_CHAIN_AUDIT.txt)
- [133 — A/D sensor acquisition](STEP133_ADC_SENSOR_ACQUISITION_AUDIT.txt)
- [134 — OSC/CYL/reference timing](STEP134_OSC_CYL_REFERENCE_TIMING_AUDIT.txt)
- [135 — ESC/knock signal chain](STEP135_ESC_KNOCK_SIGNAL_CHAIN_AUDIT.txt)
- [136 — custom-device boundary](STEP136_CUSTOM_DEVICE_FUNCTIONAL_BOUNDARY_AUDIT.txt)
- [137 — U9 register window](STEP137_U9_REGISTER_WINDOW_AUDIT.txt)
- [138 — U9 register-window closure](STEP138_U9_REGISTER_WINDOW_CLOSURE_AUDIT.txt)
- [139 — processor/memory/custom-peripheral architecture](STEP139_PROCESSOR_MEMORY_CUSTOM_PERIPHERAL_ARCHITECTURE_AUDIT.txt)

## Integrated theory and verification, Steps 140-154

- [140 — interrupt/scheduler architecture](STEP140_INTERRUPT_SCHEDULER_ARCHITECTURE_AUDIT.txt)
- [141 — reference/RPM/dwell/spark](STEP141_REFERENCE_RPM_DWELL_SPARK_THEORY_AUDIT.txt)
- [142 — sensor acquisition/filtering](STEP142_SENSOR_ACQUISITION_FILTERING_THEORY_AUDIT.txt)
- [143 — airflow/fuel/injector](STEP143_AIRFLOW_FUEL_INJECTOR_THEORY_AUDIT.txt)
- [144 — idle-air control](STEP144_IDLE_AIR_CONTROL_THEORY_AUDIT.txt)
- [145 — emissions/accessory control](STEP145_EMISSIONS_ACCESSORY_CONTROL_THEORY_AUDIT.txt)
- [146 — transmission/TCC](STEP146_TRANSMISSION_TCC_THEORY_AUDIT.txt)
- [147 — diagnostics/ALDL communication](STEP147_DIAGNOSTICS_ALDL_COMMUNICATION_THEORY_AUDIT.txt)
- [148 — output/electrical interfaces](STEP148_OUTPUT_STAGING_ELECTRICAL_INTERFACES_AUDIT.txt)
- [149 — startup/shutdown/exceptional modes](STEP149_STARTUP_SHUTDOWN_EXCEPTIONAL_MODES_AUDIT.txt)
- [150 — C-port architecture and migration](STEP150_C_PORT_ARCHITECTURE_EMBEDDED_MIGRATION_AUDIT.txt)
- [151 — simulation verification](STEP151_SIMULATION_VERIFICATION_AUDIT.txt)
- [152 — post-integration reference audit](STEP152_POST_INTEGRATION_REFERENCE_AUDIT.txt)
- Step 153 has no standalone audit file in the repository.
- [154 — Strict C89 CI regression](STEP154_STRICT_C89_CI_REGRESSION_AUDIT.txt)

## Raw HAL completion and reclosure, Steps 155-171

- [155 — named raw-input HAL](STEP155_NAMED_RAW_INPUT_HAL_AUDIT.txt)
- [156 — CTS/MAT raw HAL](STEP156_CTS_MAT_RAW_HAL_AUDIT.txt)
- [157 — complete U10 raw HAL](STEP157_COMPLETE_U10_RAW_HAL_AUDIT.txt)
- [158 — normal FMD raw HAL](STEP158_NORMAL_FMD_RAW_HAL_AUDIT.txt)
- [159 — raw-HAL lifecycle integration](STEP159_RAW_HAL_LIFECYCLE_INTEGRATION_AUDIT.txt)
- [160 — full raw-HAL lifecycle](STEP160_FULL_RAW_HAL_LIFECYCLE_AUDIT.txt)
- [161 — raw-HAL output lifecycle](STEP161_RAW_HAL_OUTPUT_LIFECYCLE_AUDIT.txt)
- [162 — raw-VOLT output safety](STEP162_RAW_VOLT_OUTPUT_SAFETY_AUDIT.txt)
- [163 — raw-HAL power-on composition](STEP163_RAW_HAL_POWER_ON_COMPOSITION_AUDIT.txt)
- [164 — raw-HAL factory IRQ](STEP164_RAW_HAL_FACTORY_IRQ_AUDIT.txt)
- [165 — raw-HAL factory A/D](STEP165_RAW_HAL_FACTORY_ADC_AUDIT.txt)
- [166 — 8192-baud SCI raw HAL](STEP166_SCI8192_RAW_HAL_AUDIT.txt)
- [167 — 160-baud completeness correction](STEP167_ALDL160_COMPLETENESS_AUDIT.txt)
- [168 — diagnostic 160-baud manager](STEP168_ALDL160_DIAGNOSTIC_MANAGER.txt)
- [169 — factory 160-baud manager](STEP169_ALDL160_FACTORY_MANAGER.txt)
- [170 — complete 160-baud raw HAL](STEP170_ALDL160_RAW_HAL_AUDIT.txt)
- [171 — whole-image reclosure](STEP171_WHOLE_IMAGE_RECLOSURE_AUDIT.txt)

## Documentation consolidation

- [172 — documentation/navigation consolidation](STEP172_DOCUMENTATION_NAVIGATION_AUDIT.txt)
- [173 — simulator/build/testing guide](STEP173_SIMULATOR_TESTING_GUIDE_AUDIT.txt)
- [174 — embedded/HIL implementation roadmap](STEP174_EMBEDDED_HIL_ROADMAP_AUDIT.txt)
