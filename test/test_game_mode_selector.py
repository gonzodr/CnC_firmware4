"""Source contracts for the cabinet Game Mode selector and protocol."""

import re
import unittest
from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[1] / "CnC_firmware4.ino").read_text(
    encoding="utf-8"
)


class GameModeSelectorTests(unittest.TestCase):
    def test_selector_audio_uses_one_shot_groove_and_layered_navigation_fx(self):
        self.assertIn("TRK_MUS_MODE_SELECT     66", SOURCE)
        self.assertIn("TRK_MODE_SELECT_WOOSH   67", SOURCE)
        self.assertIn("wTrig.trackPlayPoly(TRK_MUS_MODE_SELECT);", SOURCE)
        self.assertNotIn("wTrig.trackLoop(TRK_MUS_MODE_SELECT", SOURCE)
        self.assertIn("wTrig.trackPlayPoly(TRK_KEYLEFT);", SOURCE)
        self.assertIn("wTrig.trackPlayPoly(TRK_KEYRIGHT);", SOURCE)
        self.assertGreaterEqual(
            SOURCE.count("wTrig.trackPlayPoly(TRK_MODE_SELECT_WOOSH);"), 2
        )
        self.assertIn("wTrig.isTrackPlaying(TRK_MUS_MODE_SELECT)", SOURCE)
        self.assertIn("modeSelectGrooveEnded", SOURCE)

    def test_one_player_mask_excludes_only_coop(self):
        body = re.search(
            r"uint8_t AvailableGameModeMask\(uint8_t playerCount\) \{(.*?)\n\}",
            SOURCE,
            re.DOTALL,
        )
        self.assertIsNotNone(body)
        self.assertIn("mask &= ~(1U << GAME_COOP);", body.group(1))
        self.assertIn("GAME_MODE_MASK_ALL", body.group(1))

    def test_invalid_or_unavailable_selection_falls_back_to_standard(self):
        self.assertIn("void NormalizeSelectedGameMode()", SOURCE)
        self.assertIn("selectedGameMode = GAME_STANDARD;", SOURCE)
        self.assertIn("mode >= GAME_MODE_COUNT", SOURCE)

    def test_flippers_step_mode_and_shooter_steps_players(self):
        select = re.search(
            r"if \(intmon == 3\) \{(.*?)\n\s*if \(intmon == 2\)",
            SOURCE,
            re.DOTALL,
        )
        self.assertIsNotNone(select)
        body = select.group(1)
        self.assertIn("StepSelectedGameMode(-1);", body)
        self.assertIn("StepSelectedGameMode(1);", body)
        self.assertIn("numofplayers = numofplayers + 1;", body)

    def test_protocol_sends_snapshots_and_explicit_start(self):
        self.assertIn('Serial.print(F("GAME_MODE,"));', SOURCE)
        self.assertIn('Serial.print(F("GAME_START,"));', SOURCE)
        self.assertIn("GAME_MODE_SNAPSHOT_MS = 500UL", SOURCE)
        self.assertRegex(
            SOURCE,
            r"SendGameStart\(\);\s*Serial\.println\(\"Zero\"\);",
        )


if __name__ == "__main__":
    unittest.main()
