"""Source contracts for the standalone, multiplayer Munchies challenge."""

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MAIN = (ROOT / "CnC_firmware4.ino").read_text(encoding="utf-8")
MODE = (ROOT / "g_munchies_mode.ino").read_text(encoding="utf-8")
CHALLENGE = (ROOT / "j_munchies_challenge.ino").read_text(encoding="utf-8")


class MunchiesChallengeTests(unittest.TestCase):
    def test_selector_starts_standalone_context(self):
        self.assertIn("StartStandaloneMunchiesChallenge();", MAIN)
        self.assertIn(
            "StartMunchiesMode(MUNCHIES_STANDALONE_CHALLENGE);", CHALLENGE
        )
        self.assertEqual(MODE.count("void StartMunchiesMode("), 1)

    def test_challenge_owns_lifecycle_and_never_ejects_a_phantom_ball(self):
        self.assertIn("StandaloneMunchiesOwnsGameLoop()", MAIN)
        self.assertIn(
            "munchiesStartContext == MUNCHIES_STANDALONE_CHALLENGE", MODE
        )
        standalone = MODE.split(
            "if (munchiesStartContext == MUNCHIES_STANDALONE_CHALLENGE)", 1
        )[1].split("unsigned long now", 1)[0]
        self.assertIn("digitalWrite(ufoCoil, LOW);", standalone)
        self.assertIn("StandaloneMunchiesRunFinished();", standalone)

    def test_each_player_starts_immediately_then_reports_result_and_winner(self):
        self.assertNotIn("STANDALONE_MUNCHIES_COUNTDOWN_SECONDS", CHALLENGE)
        self.assertNotIn('F("MUNCHIES_READY,")', CHALLENGE)
        self.assertIn('F("MUNCHIES_PLAYER,")', CHALLENGE)
        player_start = CHALLENGE.split("void BeginStandaloneMunchiesPlayer()", 1)[1]
        self.assertLess(
            player_start.index('F("MUNCHIES_PLAYER,")'),
            player_start.index("StartMunchiesMode(MUNCHIES_STANDALONE_CHALLENGE);"),
        )
        self.assertIn('F("MUNCHIES_RESULT,")', CHALLENGE)
        self.assertIn('F("MUNCHIES_FINISH,")', CHALLENGE)
        self.assertIn("if (player < numofplayers)", CHALLENGE)
        self.assertIn("player++;", CHALLENGE)


if __name__ == "__main__":
    unittest.main()
