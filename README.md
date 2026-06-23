# Apple2Esp32

An **Apple II / Apple IIe emulator** that runs on a single **ESP32** microcontroller.
It emulates the MOS 6502 CPU, the Apple II memory map and soft switches, the
Disk II floppy controller and a ProDOS hard‑disk interface, and drives a real
**VGA monitor** and a **PS/2 keyboard** directly from the ESP32's GPIO pins.
Disk images are loaded from a microSD card.

> The goal of the project is a self‑contained, low‑cost Apple II that boots
> real software (DOS 3.3, ProDOS, games, utilities) from `.dsk` / `.po` /
> `.hdv` images with nothing more than an ESP32, a handful of resistors for
> the VGA DAC, a PS/2 keyboard and an SD card.

---

## Features

- **6502 CPU emulation** based on Damian Peckett's `µ6502` core.
- **Apple II+ and Apple IIe** modes (switchable at runtime, choice stored in EEPROM).
- **Video output over VGA** at `320x240` using the `ESP32Lib` (6‑bit / 64‑color) VGA driver:
  - 40‑column text
  - Low‑resolution (LoRes) graphics
  - High‑resolution (HiRes) graphics
  - Page 1 / Page 2, full‑screen and mixed (split) display modes
  - Color and monochrome rendering
- **PS/2 keyboard** input with full scancode‑to‑Apple translation, including
  Shift and Ctrl modifiers and the arrow keys.
- **Disk II emulation** — boot and run `.dsk`, `.do` and `.po` floppy images
  with on‑the‑fly nibble encoding.
- **Hard disk / mass‑storage emulation** in slot 7 (ProDOS block device) for
  `.hdv`, `.po` and `.2mg` images.
- **Language Card** (bank‑switched RAM at `$D000–$FFFF`) and the Apple IIe
  auxiliary/aux‑bank memory.
- **Speaker** audio output (`$C030` toggle).
- **Emulated joystick / paddles** mapped to the keyboard arrow keys and modifier
  keys (can be toggled off).
- **microSD card** storage for disk and hard‑disk images, with an on‑screen
  file browser.
- **On‑screen Options menu** to switch machine type, swap disks, toggle speed,
  pause, etc.
- **Persistent settings** stored in the ESP32's EEPROM (selected machine,
  selected disk/HD image, speed and joystick options).
- **Optional serial video / serial keyboard** mode for headless use over the
  USB serial port.
- **Selectable CPU speed** (authentic ~1 MHz or "fast" mode); the ESP32 itself
  runs at 240 MHz.

---

## Hardware

### Target board

- **ESP32‑WROOM‑DA** module (Arduino board id `esp32:esp32:esp32da`).
  Any common ESP32 dev board with enough free GPIO should work, but the pin
  assignments below are the defaults used by the sketch.

### What you need

- ESP32 dev board (ESP32‑WROOM‑DA recommended)
- VGA connector + a simple resistor‑ladder DAC (2 bits per color channel)
- PS/2 keyboard (with level shifting to 3.3 V as appropriate)
- microSD card module (SPI)
- A small speaker / piezo or audio amp for sound
- VGA monitor

### Default pin assignments

These are defined at the top of [`Apple2Esp32.ino`](Apple2Esp32.ino).

#### VGA (6‑bit color, 2 bits per channel)

| Signal  | ESP32 GPIO |
|---------|------------|
| HSYNC   | 23 |
| VSYNC   | 15 |
| Red 0   | 21 |
| Red 1   | 22 |
| Green 0 | 18 |
| Green 1 | 19 |
| Blue 0  | 4  |
| Blue 1  | 5  |

#### PS/2 keyboard

| Signal | ESP32 GPIO |
|--------|------------|
| Data   | 32 |
| Clock (IRQ) | 33 |

#### microSD (SPI)

| Signal | ESP32 GPIO |
|--------|------------|
| SCK  | 14 |
| MISO | 16 |
| MOSI | 17 |
| CS   | 27 |

#### Other

| Signal  | ESP32 GPIO |
|---------|------------|
| Speaker / LED | 2 |

> If your wiring differs, just edit the pin constants near the top of
> `Apple2Esp32.ino` before building.

---

## Building and flashing

### Toolchain / dependencies

This is an **Arduino** sketch. The tested environment (see
[`install.txt`](install.txt)) is:

- **ESP32 Arduino core** by Espressif — version **2.0.17**
- **ESP32Lib** (VGA driver, by bitluni) — version **0.3.4**

Standard Arduino libraries used: `FS`, `SD`, `EEPROM` (all bundled with the
ESP32 core).

### Recommended board configuration

The sketch is set up to run with the following options (see
[`.vscode/arduino.json`](.vscode/arduino.json)):

- Board: **ESP32 Dev Module (esp32da)**
- CPU Frequency: **240 MHz**
- Flash Mode: **QIO**, Flash Frequency: **80 MHz**, Flash Size: **4 MB**
- Partition Scheme: **Default**
- Upload Speed: **921600**

### Build with the Arduino IDE

1. Install the **ESP32 by Espressif** boards package (`2.0.17`) via the
   Boards Manager.
2. Install the **ESP32Lib** library (`0.3.4`) via the Library Manager (or
   download bitluni's `ESP32Lib` from GitHub).
3. Open `Apple2Esp32.ino` (all the `.ino` files in this folder are part of one
   sketch and compile together).
4. Select your ESP32 board and the settings listed above.
5. Connect the board and click **Upload**.

### Build with arduino-cli

```bash
arduino-cli core install esp32:esp32@2.0.17
arduino-cli lib install ESP32Lib@0.3.4

arduino-cli compile \
  --fqbn esp32:esp32:esp32da:CPUFreq=240,FlashSize=4M,UploadSpeed=921600 \
  .

arduino-cli upload \
  --fqbn esp32:esp32:esp32da \
  -p /dev/ttyUSB0 \
  .
```

(Replace `/dev/ttyUSB0` with your serial port, e.g. `COM8` on Windows.)

---

## Preparing the SD card

1. Format a microSD card as **FAT32**.
2. Copy your Apple II disk images to the root of the card:
   - Floppies: `.dsk`, `.do`, `.po`
   - Hard‑disk / mass‑storage images: `.hdv`, `.po`, `.2mg`
3. Insert the card and power on the ESP32. Use the on‑screen **Options** menu
   (see below) to choose the image to boot.

---

## Usage / keyboard shortcuts

The emulator boots into the selected machine. Special host keys are intercepted
to control the emulator:

| Keys | Action |
|------|--------|
| **Ctrl + Esc** | Open / close the on‑screen **Options** window |
| **Ctrl + F12** | 6502 **reset** (warm reset) |
| **Ctrl + F5**  | **Reboot** the ESP32 |
| **Ctrl + Enter** | Save the current selection to EEPROM and reboot |

### Inside the Options window

| Key | Action |
|-----|--------|
| **Up / Down arrows** | Move through the list of images on the SD card |
| **Enter** | Select the highlighted disk / hard‑disk image |
| **Esc** | Close the Options window |
| **F1** | Toggle between **Floppy (Disk II)** and **Hard Disk** mode |
| **F2** | Toggle between **Apple II+** and **Apple IIe** |
| **F3** | Toggle **fast / 1 MHz** CPU speed |
| **F4** | **Pause** / resume the emulation |
| **F5** | Toggle the **joystick** emulation on/off |

The selected machine type, disk/HD image and option flags are written to the
ESP32 EEPROM so they are restored on the next boot.

### Joystick / paddles

When joystick emulation is enabled, the **arrow keys** drive paddle 0/1 (X/Y),
and **Shift / Ctrl** act as the two push buttons (PB0/PB1).

---

## Project layout

| File | Responsibility |
|------|----------------|
| `Apple2Esp32.ino` | Main sketch: setup, globals, pin map, EEPROM, options window, main loop |
| `cpu.ino`         | 6502 (`µ6502`) CPU emulation core |
| `memory.ino`      | Main RAM read/write and memory dispatch |
| `softswitches.ino`| Apple II soft switches (`$C0xx`) |
| `languagecard.ino`| Language Card / bank‑switched RAM |
| `video.ino`       | VGA rendering (text, LoRes, HiRes) |
| `serialvideo.ino` | Optional text rendering / keyboard over serial |
| `keyboard.ino`    | PS/2 keyboard decoding and Apple keycode translation |
| `joystick.ino`    | Emulated joystick / paddle timing |
| `speaker.ino`     | Speaker (`$C030`) output |
| `disk.ino`        | Disk II floppy controller emulation and nibble encoding |
| `hd.ino`          | ProDOS hard‑disk / block device emulation (slot 7) |
| `sd.ino`          | microSD card / filesystem helpers |
| `rom.h`           | Apple II / IIe ROMs and character set fonts |

---

## Credits

- **6502 core:** `µ6502` — a barebones 6502 emulator by
  [Damian Peckett](https://dpeckett.com).
- **VGA & PS/2 driver:** [`ESP32Lib`](https://github.com/bitluni/ESP32Lib)
  by bitluni.
- Apple II/IIe is a trademark of Apple Inc. The ROM images included here are
  the property of their respective copyright holders and are provided for
  use with this emulator only.

---

## Disclaimer

This is a hobby / educational project. Apple II ROMs and any disk images are
subject to their own copyrights — make sure you have the right to use any
software you load. Provided **as‑is**, without warranty of any kind.
