"""Source contracts for attract-mode cabinet service controls."""

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SKETCH = (ROOT / "CnC_firmware4.ino").read_text(encoding="utf-8")
LIGHTS = (ROOT / "d_light_effects.ino").read_text(encoding="utf-8")


class ServiceMenuControlTests(unittest.TestCase):
    def test_attract_combo_enters_service_without_starting_game(self):
        self.assertIn("static const uint8_t sequence[4] = { 1, 2, 3, 4 };", SKETCH)
        self.assertIn('Serial.println(F("SERVICE_MENU_ENTER"));', SKETCH)
        self.assertIn("if (PollAttractServiceCombo()) return;", SKETCH)
        self.assertIn("intmon = 4;", SKETCH)

    def test_service_mode_emits_all_four_cabinet_controls(self):
        for command in (
            "SERVICE_LEFT", "SERVICE_RIGHT", "SERVICE_CONFIRM", "SERVICE_BACK",
        ):
            self.assertIn(f'Serial.println(F("{command}"));', SKETCH)
        self.assertIn("if (intmon == 4)", SKETCH)
        self.assertIn("ServiceMenuInputPoll();", SKETCH)

    def test_gameplay_entry_requires_five_starts_then_the_service_code(self):
        self.assertIn("SERVICE_GAME_START_WINDOW_MS", SKETCH)
        self.assertIn("if (serviceGameStartCount == 5)", SKETCH)
        self.assertIn('Serial.println(F("SERVICE_ARMED"));', SKETCH)
        self.assertIn("if (PollServiceCode()) {\n    BeginServiceAbort();", SKETCH)

    def test_service_abort_stops_coils_but_does_not_hold_the_gui_for_drain(self):
        self.assertIn("DisableGameplayCoilsForService();", SKETCH)
        self.assertIn("serviceAbortUfoPulse", SKETCH)
        self.assertIn("SimDigitalRead(shooterLaneSwitch) == LOW", SKETCH)
        self.assertIn('Serial.println(F("SERVICE_DRAINED"));', SKETCH)

    def test_gui_can_release_firmware_back_to_attract(self):
        self.assertIn('strcmp(s, "SERVICE_EXIT") == 0', LIGHTS)
        self.assertIn("intmon = 1;", LIGHTS)

    def test_attract_keeps_rendering_without_external_arduino_reset(self):
        self.assertIn("static int attractLightIndex = 0;", SKETCH)
        self.assertIn("FillLEDsFromPaletteColors(attractLightIndex, 0);", SKETCH)
        self.assertIn("ResetLightEffectsForAttract();", LIGHTS)
        self.assertNotIn("digitalWrite(PIN_A13, LOW);", SKETCH)


if __name__ == "__main__":
    unittest.main()
