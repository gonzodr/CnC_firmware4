"""Regression checks for highscore exit and bounded servo I2C handling."""

import re
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SKETCH = (ROOT / "CnC_firmware4.ino").read_text(encoding="utf-8")
LIGHTS = (ROOT / "d_light_effects.ino").read_text(encoding="utf-8")
GUI = (
    ROOT.parent / "CnC Pinball GUI V3 Python" / "src" / "state_machine.py"
).read_text(encoding="utf-8")


class HighscoreSerialI2CTests(unittest.TestCase):
    def test_gui_newline_exit_is_parsed_by_the_shared_serial_reader(self):
        self.assertIn('send_raw(variant + "\\n")', GUI)
        self.assertIn("PollControlSerial();", SKETCH)
        self.assertNotIn("if (intmon != 2) PollControlSerial();", SKETCH)
        self.assertNotIn("Serial.readString()", SKETCH)
        self.assertIn("if (c == '\\n' || c == '\\r')", LIGHTS)
        self.assertIn("HandleControlCmd(controlBuf);", LIGHTS)
        for variant in ("Exit", "Exit1", "Exit2"):
            self.assertIn(f'strcmp(s, "{variant}") == 0', LIGHTS)
        self.assertIn("digitalWrite(PIN_A13, LOW); // hardware reset", LIGHTS)

    def test_overlong_command_cannot_execute_a_valid_looking_suffix(self):
        self.assertIn("boolean controlOverflow = LOW;", LIGHTS)
        self.assertIn("controlLen > 0 && controlOverflow != HIGH", LIGHTS)
        self.assertIn("controlOverflow = HIGH;", LIGHTS)
        self.assertRegex(LIGHTS, r"controlLen = 0;\s*controlOverflow = LOW;")

    def test_servo_i2c_is_bounded_and_deferred(self):
        self.assertIn("Wire.setWireTimeout(25000UL, true);", SKETCH)
        self.assertIn("ServiceWeedMeter();", SKETCH)
        self.assertIn("if (player < 1 || player > 4) return;", SKETCH)
        self.assertIn("weedMeterPending = HIGH;", SKETCH)
        self.assertIn("Wire.getWireTimeoutFlag()", SKETCH)
        self.assertIn("Wire.clearWireTimeoutFlag()", SKETCH)
        self.assertIn("++weedMeterAttempts >= 3", SKETCH)
        self.assertIn('Serial.print(F("FAULT,SERVO_I2C,"));', SKETCH)
        send = re.search(r"void weedmetersend\(\) \{(.*?)\n\}", SKETCH, re.DOTALL)
        self.assertIsNotNone(send)
        self.assertNotIn("Wire.endTransmission()", send.group(1))

    def test_basement_audio_and_memory_layout_are_unchanged(self):
        effect_data = (ROOT / "effect_data.h").read_text(encoding="utf-8")
        self.assertIn('section(".fini1")', effect_data)
        self.assertIn("TRK_VO_MULTIBALL_ACAPULCO_A,", SKETCH)
        self.assertIn("TRK_VO_MULTIBALL_MICHOACAN_A,", SKETCH)
        self.assertIn("wTrig.trackPlayPoly(mbLoop[lvl]);", SKETCH)
        self.assertIn("wTrig.trackPlayPoly(firstHitSound);", SKETCH)


if __name__ == "__main__":
    unittest.main()
