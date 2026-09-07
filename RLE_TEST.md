# Independent-frame RLE trial

Branch: `codex/light-rle`. Stable pre-trial commit: `f42f7e6`.

The editor's opt-in command exports codec v1. Legacy raw banks remain supported
by this firmware. RLE banks reject compilation with firmware lacking
`FX_CODEC_SUPPORTED=1`. Do not run the legacy mask script on this already-masked bank.

Each effect begins with XOR-masked little-endian uint16 frame offsets. Each
packet starts with mode 0 (204 raw RGB bytes) or mode 1 (count,R,G,B runs
totaling 68 pixels). Offsets are relative to the effect, with far-flash access.
Every frame is independent. Black and transparent magenta remain distinct.

Verified all 19 effects / 742 frames and metadata by editor import/export
round-trip. Data: 151368 -> 74382 bytes. Mega release build: 130716 bytes
flash, 4021 bytes static RAM (prior release: 207632 / 4021).

Hardware timing is NOT yet measured. Optional compiler flag
`--build-property compiler.cpp.extra_flags=-DFX_PROFILE` reports
`FX_US_MAX,<microseconds>` once per second, measuring the game effect rendering
section (not FastLED.show or service light test). Compare raw and compressed
banks with the same firmware and effects, including Hurry Up + overlay overlap.
Use a separate build directory for profiling and rebuild without the flag for
normal play. A successful build does not prove identical game-loop latency.

On-device checks: loop/outro boundaries, interrupted effects, overlay black vs
transparency, service light test, and switch responsiveness during dense effects.
The main branch has not been changed by this trial.
