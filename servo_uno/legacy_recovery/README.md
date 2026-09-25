# Legacy Uno firmware recovery

This directory documents the firmware read from the installed ATmega328P Uno
on 2026-09-24. The original source was not available; the readable sketch is a
functional reconstruction from the AVR machine code.

## Verified backup

The two independent reads are identical:

- flash SHA-256: `A7A7723DBFFA825567784D808469B040174073E63CFCCC92CD4FFA99F5310FB5`
- EEPROM SHA-256: `A82978DDF95301C0891CA3476CFAECA89DFF5A1FE307298783D8A7C9AE9E3E01`
- MCU signature: `1E 95 0F` (`ATmega328P`)
- flash read: 32768 bytes
- EEPROM read: 1024 bytes, entirely `0xFF`

The original, restorable backup is `../UnoBackup.zip`. Do not replace that ZIP
with a recompiled version of the reconstructed source.

## Recovered active program

The active sketch occupies approximately `0x0000..0x18D9`. Bytes above it
contain remnants of still older uploads plus the bootloader starting around
`0x7800`; they are not part of the active sketch.

Recovered hardware/protocol details:

- Uno is an I2C slave at address `8` (`0x08`).
- Mega sends command `a` followed by one raw byte (`0..180`).
- servo 1 signal: Uno `D4`
- servo 2 signal: Uno `D5`
- servo 3 signal: Uno `D6`
- relay control: Uno `D7`, driven HIGH during movement
- serial diagnostics: `9600` baud
- after a valid command, only servo 1 is attached and moved
- movement power/signal window: `250 ms`
- servo signal is detached and relay is driven LOW after the movement

At startup all three Servo objects are attached, commanded to `190`, `0`, `0`,
held for `800 ms`, then detached. Arduino's Servo library clamps `190` to
`180`.

## Important limitations

Compilation removes source names, formatting, and comments. The reconstructed
sketch preserves the observed pins, constants, command flow, timings, and
library calls, but it is not claimed to be byte-identical source code.

The recovered firmware uses the default Servo library pulse range and commands
the full `0..180` angle range. It also powers the servo only for 250 ms. These
choices should be reviewed before using a new servo under mechanical load.
