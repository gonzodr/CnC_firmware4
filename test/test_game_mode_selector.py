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

    def test_start_plays_mode_confirmation_and_matching_random_voice(self):
        self.assertIn("TRK_MODE_SELECTED       68", SOURCE)
        self.assertIn("MODE_SELECT_CONFIRM_HOLD_MS = 1600UL", SOURCE)
        self.assertIn("MODE_SELECT_CONFIRM_FADE_MS = 650UL", SOURCE)
        self.assertIn(
            "MODE_SELECT_CONFIRM_HOLD_MS + MODE_SELECT_CONFIRM_FADE_MS",
            SOURCE,
        )
        self.assertIn('Serial.print(F("GAME_MODE_CONFIRM,"));', SOURCE)
        self.assertIn("wTrig.trackPlayPoly(TRK_MODE_SELECTED);", SOURCE)
        for track in (
            "TRK_CHEECH_MODE_STANDARD", "TRK_CHEECH_MODE_COOP",
            "TRK_CHEECH_MODE_QUICK", "TRK_CHEECH_MODE_MULTIB",
            "TRK_CHONG_MODE_STANDARD", "TRK_CHONG_MODE_COOP",
            "TRK_CHONG_MODE_QUICK", "TRK_CHONG_MODE_MULTIB",
        ):
            self.assertIn(track, SOURCE)
        self.assertIn("const boolean useChong = random(0, 2) == 1;", SOURCE)
        munchies_case = SOURCE.split("case GAME_MUNCHIES:", 1)[1].split(
            "default:", 1
        )[0]
        self.assertNotIn("voiceTrack =", munchies_case)
        select_start = SOURCE.split(
            "// 2. start: jatek inditasa a kivalasztott jatekosszammal", 1
        )[1].split("// A groove egyszer fut le.", 1)[0]
        ordered_calls = (
            "SendGameModeConfirm();",
            "PlaySelectedGameModeConfirmation();",
            "delay(MODE_SELECT_CONFIRM_MS);",
            "SendGameStart();",
        )
        positions = [select_start.index(call) for call in ordered_calls]
        self.assertEqual(positions, sorted(positions))

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
        start = SOURCE.split("// 2. start: jatek inditasa", 1)[1].split(
            "// A groove egyszer fut le.", 1
        )[0]
        self.assertLess(start.index("SendGameStart();"),
                        start.index('Serial.println("Zero");'))

    def test_arcade_has_two_games_and_explicit_submenu_protocol(self):
        self.assertIn("ARCADE_MUNCHIES = 0", SOURCE)
        self.assertIn("ARCADE_PUFF_N_RIFF", SOURCE)
        for command in ("ARCADE_ENTER", "ARCADE_STATE", "ARCADE_CONFIRM"):
            self.assertIn(f'SendArcadeState(F("{command}"));', SOURCE)
        self.assertIn('Serial.println(F("ARCADE_EXIT"));', SOURCE)
        self.assertIn("StepSelectedArcadeGame(-1);", SOURCE)
        self.assertIn("StepSelectedArcadeGame(1);", SOURCE)
        mode = (Path(__file__).resolve().parents[1] / "g_munchies_mode.ino").read_text(encoding="utf-8")
        self.assertIn('F("GUITAR_SOLO_START,")', mode)
        self.assertIn("StartStandaloneMunchiesChallenge();", SOURCE)
        self.assertIn("runningArcadeGame = selectedArcadeGame;", SOURCE)


if __name__ == "__main__":
    unittest.main()
