# 🛠️ Tools

Utility tools and helpers for the OceanLabz AI Explorer S3 Board.

## Contents

| Tool | Description |
|------|-------------|
| [`pin_configuration/`](pin_configuration/) | Pin definition header files for Arduino sketches |
| [`board_test/`](board_test/) | Board bring-up test sketches |
| [`flashing_helpers/`](flashing_helpers/) | Scripts and utilities for flashing firmware |

## Pin Configuration

The [`pins_ai_explorer_s3.h`](pin_configuration/pins_ai_explorer_s3.h) file provides all fixed pin definitions. Include it in any sketch:

```cpp
#include "pins_ai_explorer_s3.h"
```

This ensures your code is portable across board revisions and avoids hard-coded magic numbers.

## Board Test

The board test tool runs diagnostics on all peripherals — use it to verify a new board is functioning correctly.

## Flashing Helpers

Includes `esptool` wrappers for erasing flash, flashing bootloaders, and bulk programming.