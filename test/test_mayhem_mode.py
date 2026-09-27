"""Source contracts for the standalone Multiball Mayhem lifecycle."""

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MAIN = (ROOT / "CnC_firmware4.ino").read_text(encoding="utf-8")
MAYHEM = (ROOT / "i_mayhem_mode.ino").read_text(encoding="utf-8")


class MayhemModeTests(unittest.TestCase):
    def test_reuses_existing_multiball_entry_point(self):
        self.assertIn(
            "StartWeedMultiball(mayhemStage, WEED_MB_FROM_CHALLENGE);",
            MAYHEM,
        )
        self.assertEqual(MAIN.count("boolean StartWeedMultiball("), 1)

    def test_all_four_ball_counts_and_central_balancing_exist(self):
        self.assertIn("BIP = lvl + 2;", MAIN)
        self.assertIn("MAYHEM_BASE_STAGE_MS = 25000UL", MAYHEM)
        self.assertIn("{ 3, 4, 5, 6 }", MAYHEM)
        self.assertIn("MAYHEM_MAX_BONUS_SECONDS = 10", MAYHEM)

    def test_normal_drain_lifecycle_is_blocked(self):
        self.assertIn(
            "if (!MayhemOwnsGameLifecycle() && ballsaversw == LOW",
            MAIN,
        )
        self.assertIn("MAYHEM_COMPLETE", MAYHEM)

    def test_stage_waits_for_stable_full_trough(self):
        self.assertIn("bisFiveReads >= BALL_DRAIN_CONFIRM_READS", MAYHEM)
        self.assertIn("millis() - bisFiveSince >= BALL_DRAIN_CONFIRM_MS", MAYHEM)
        self.assertIn("if (!MayhemTroughStable()) return;", MAYHEM)

    def test_jackpot_and_result_protocol_are_connected(self):
        self.assertIn("MayhemJackpotCollected();", MAIN)
        for message in (
            "MAYHEM_READY,", "MAYHEM_STAGE,", "MAYHEM_PROGRESS,",
            "MAYHEM_STAGE_END,", "MAYHEM_RESULT,", "MAYHEM_FINISH,",
        ):
            self.assertIn(message, MAYHEM)


if __name__ == "__main__":
    unittest.main()
