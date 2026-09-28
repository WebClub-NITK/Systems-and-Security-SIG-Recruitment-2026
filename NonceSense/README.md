# NonceSense: A Bare-Metal Hardware Authentication Token Inspired by YubiKeys

**Domain:** Bare-metal programming, hardware security

## Introduction

Hardware security keys (YubiKey, Google Titan) rely on two principles:

1. **Cryptographic proof without leakage:** The host sends a random challenge (nonce), and the key returns a response computed from a secret that never leaves the hardware.
2. **Physical proof-of-presence:** A human must press a button to authorize each transaction, which blocks silent malware on a compromised host.

You will build a bare-metal 2FA token from scratch in C, using direct register access, hardware timers and interrupts. The tasks build on each other and increase in difficulty. Submit as much as you complete, with a README and commented code.

## Hardware

- Any physical board (STM32, RP2040, AVR, etc.) or the Wokwi simulator. Cortex-M boards are recommended; ESP32 is discouraged.
- 1 LED (status/alert)
- 1 push button (user presence). A jumper to GND is acceptable if your board has no button.
- 1 USB-UART link

## Rules (all tasks)

1. **No IDEs.** Don't create or build the project in STM32CubeIDE, CubeMX, Keil, Arduino IDE, PlatformIO or similar tools. Edit in any text editor; build from the terminal with a GCC cross-toolchain.
2. **Makefile only.** It must support `make`, `make clean` and `make flash`, and compile with `-Wall -Wextra -O2 -nostdlib -nostartfiles` with zero warnings.
3. **Write your own startup file and linker script.** Vendor or template startup files, linker scripts and `system_*.c` are not allowed.
4. **No HAL, LL, SDK or Arduino APIs.** All peripherals are configured through direct register access.
5. **Wokwi users:** build the `.elf` locally with your Makefile and load it into the simulator. Don't use Wokwi's online compiler.
6.  **Debuggers are allowed.** You may use GDB, OpenOCD, or an editor's debug front-end (e.g. VS Code + Cortex-Debug) to flash and step through code, as long as the build itself is done by your Makefile.

## Task 0: Bare-Metal Blinky

- No libraries or headers of any kind, **including CMSIS**. Only `<stdint.h>` is allowed.
- Define every register address yourself from the reference manual.
- Blink the LED at 1 Hz. A busy-wait delay is fine.
- Keep this task in a separate `task0/` directory with its own Makefile, startup file and linker script.

## From Task 1 Onward

- CMSIS core and device **headers** are allowed. CMSIS startup and system files are not.
- The provided `uart.c`/`uart.h` and `crc32.c`/`crc32.h` may be used, adapted to your board.

## Task 1: UART Challenge-Response Handshake

- UART at 115200 baud, 8-N-1. Polling is fine.
- Define a 32-bit secret key and use the same key in your host test script.
- Input command: `AUTH:<8-HEX-CHARS>\n`
- Reply with `RESP:<8-HEX-CHARS>\n`, a CRC32-based response computed from the challenge and the key.
- Toggle the LED on every successful reply.

## Task 2: User-Presence Verification

- Standby: LED off, listening on UART.
- A valid `AUTH` request starts a 10 s countdown using a **hardware timer**, while the LED blinks at 5 Hz.
- Compute and send the response **only** after a button press.
- The button must be an **external interrupt**, and its ISR only sets a flag.
- On timeout, send a timeout error over UART; on an invalid state, send an error.
- Implement the token as a finite state machine and include its state diagram in the README.

## Task 3: Constant-Time Verification & Anti-Spam Lockout

- UART RX must be **interrupt-driven** into a ring buffer.
- Add a `VERIFY:<8-HEX-CHARS>\n` command. The token compares the value against the expected response to the last challenge and replies `OK\n` or `FAIL\n`.
- Do the comparison with a constant-time function:
```c
  bool constant_time_memcmp(const uint8_t *a, const uint8_t *b, size_t len);
```
- Show in your logs that the cycle count is identical for matching and non-matching inputs.
- Keep a `violation_count` for malformed commands, invalid hex and spam received during active operations:
  - **Violations 1–3:** discard UART input for 1 s.
  - **Violation 4+:** lock out for 10 s. The LED blinks at 10 Hz, all commands are ignored, and the token prints:
```
    [SECURITY ALERT] Malicious spam detected! Hardware locked for 10 seconds.
```

## Submission

Create a private repo and add the mentors as collaborators. Include:

- All source files, headers, the Makefile, startup file and linker script (`task0/` and the main firmware).
- A README with:
  - Target MCU and pin mapping (LED, button, UART TX/RX)
  - Register base addresses and offsets used
  - A short explanation of the boot flow from reset to `main()`
- Terminal logs or a demo video.

> Repos with IDE project files (`.ioc`, `.cproject`, `.uvprojx`, `platformio.ini`) or vendor startup/linker files will be marked non-compliant.

**Mentors:**
- Maanya Golash (`hyper-mania14`, 7975676357)
- Abhinav S Rao (`ABHINAV-S-RAO`, 8660033892)

## Resources

- Wokwi: https://docs.wokwi.com/
- State machines for MCUs: https://www.gammon.com.au/forum/threads/12316.html?id=12316
- Embedded systems: https://www.youtube.com/@inpyjamaarchieves
- CRC: https://www.youtube.com/watch?v=izG7qT0EpBw
- Constant-time comparison: https://security.stackexchange.com/questions/160808/why-should-memcmp-not-be-used-to-compare-security-critical-data
