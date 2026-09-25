"""Source contracts for the behavior-neutral Game Mode foundation refactor."""

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SOURCE = (ROOT / "CnC_firmware4.ino").read_text(encoding="utf-8")


class GameModeFoundationTests(unittest.TestCase):
    def test_game_mode_enum_and_standard_default_exist(self):
        self.assertIn("enum GameMode : uint8_t", SOURCE)
        self.assertIn("GAME_MULTIBALL_MAYHEM", SOURCE)
        self.assertIn("GameMode selectedGameMode = GAME_STANDARD;", SOURCE)

    def test_ball_save_duration_is_32_bit_and_split_by_context(self):
        self.assertIn("uint32_t ballsavetime", SOURCE)
        self.assertIn("void StartNormalBallSave()", SOURCE)
        self.assertIn("void StartMultiballBallSave()", SOURCE)
        self.assertNotIn("ballsavetime = (int)minimumMs", SOURCE)

    def test_owner_and_extra_ball_gates_exist(self):
        self.assertIn("uint8_t ScoreOwner()", SOURCE)
        self.assertIn("uint8_t ProgressOwner()", SOURCE)
        self.assertIn("boolean TryAwardExtraBall()", SOURCE)

    def test_direct_ball_save_starts_use_common_entry_points(self):
        self.assertEqual(SOURCE.count("ballsavetime = durationMs;"), 1)
        self.assertNotIn("ballsavetime = 15000", SOURCE)
        self.assertNotIn("ballsavetime = 30000", SOURCE)
        self.assertIn("StartNormalBallSave();", SOURCE)
        self.assertIn("StartMultiballBallSave();", SOURCE)


if __name__ == "__main__":
    unittest.main()
