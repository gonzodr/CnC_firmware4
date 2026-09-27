"""Source contracts for the fast, beginner-friendly Quick Game ruleset."""

import re
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SOURCE = (ROOT / "CnC_firmware4.ino").read_text(encoding="utf-8")
MUNCHIES = (ROOT / "g_munchies_mode.ino").read_text(encoding="utf-8")


class QuickModeTests(unittest.TestCase):
    def test_normal_ball_save_profile_orders_quick_standard_coop(self):
        values = {
            name: int(value)
            for name, value in re.findall(
                r"NORMAL_BALL_SAVE_MS_(STANDARD|COOP|QUICK) = (\d+)UL", SOURCE
            )
        }
        self.assertGreater(values["QUICK"], values["STANDARD"])
        self.assertGreater(values["STANDARD"], values["COOP"])
        self.assertRegex(
            SOURCE,
            r"(?s)NormalBallSaveDurationMs\(\).*?GAME_COOP.*?"
            r"GAME_QUICK.*?NORMAL_BALL_SAVE_MS_STANDARD",
        )
        self.assertIn("StartBallSave(MULTIBALL_BALL_SAVE_MS);", SOURCE)

    def test_quick_ufo_is_always_available_and_has_no_score_steal(self):
        tier = re.search(
            r"uint8_t CurrentUfoPartyTier\(\) \{(.*?)\n\}", SOURCE, re.DOTALL
        )
        quick_draw = re.search(
            r"int DrawQuickUfoLottery\(uint8_t tier\) \{(.*?)\n\}",
            SOURCE,
            re.DOTALL,
        )
        self.assertIsNotNone(tier)
        self.assertIsNotNone(quick_draw)
        self.assertIn("UFO_PARTY_QUICK_RANDOM", tier.group(1))
        self.assertIn("UFO_PARTY_FEATURE_WHEEL", tier.group(1))
        self.assertIn("int result = random(3, 10);", quick_draw.group(1))
        self.assertIn("if (result == 8) result = 10;", quick_draw.group(1))
        self.assertNotRegex(quick_draw.group(1), r"result\s*=\s*1\b")
        self.assertNotRegex(quick_draw.group(1), r"result\s*=\s*2\b")
        self.assertNotIn("result = 8", quick_draw.group(1))

    def test_weed_qualifies_existing_feature_wheel_in_quick(self):
        weed = re.search(
            r"void OnWeedCompleted\(\) \{(.*?)\n\}", SOURCE, re.DOTALL
        )
        self.assertIsNotNone(weed)
        self.assertIn("runningGameMode == GAME_QUICK", weed.group(1))
        self.assertIn('"QUICK_WHEEL_READY"', weed.group(1))
        self.assertIn("TRK_VO_UFO_FEATURE_WHEEL_A", weed.group(1))
        self.assertIn("StartUfoWheelPresentation();", SOURCE)
        consume = re.search(
            r"void ConsumeUfoPartyReward\(uint8_t tier\) \{(.*?)\n\}",
            SOURCE,
            re.DOTALL,
        )
        self.assertIsNotNone(consume)
        self.assertRegex(
            consume.group(1),
            r"(?s)GAME_QUICK && tier == UFO_PARTY_FEATURE_WHEEL.*?"
            r"spinnersw = 1;",
        )

    def test_quick_spinner_targets_are_four_five_six_eight_turns(self):
        helper = re.search(
            r"uint8_t SpinnerMeterDecrement\(uint8_t level\) \{(.*?)\n\}",
            SOURCE,
            re.DOTALL,
        )
        self.assertIsNotNone(helper)
        self.assertRegex(
            helper.group(1),
            r"quickDecrement\[4\].*?\{ 45, 36, 30, 25 \}",
        )
        self.assertIn("SpinnerMeterDecrement((uint8_t)lvl)", SOURCE)
        self.assertEqual([180 // value for value in (45, 36, 30)], [4, 5, 6])
        self.assertEqual((180 + 25 - 1) // 25, 8)

    def test_quick_ufo_restores_after_rewards_and_munchies(self):
        self.assertIn("RestorePartyShotsForPlayer();", MUNCHIES)
        lottery_finish = re.search(
            r"if \(ufoshoot == 4.*?ResumeUfoLotteryAudio\(\);(.*?)\n\s*\}",
            SOURCE,
            re.DOTALL,
        )
        self.assertIsNotNone(lottery_finish)
        self.assertIn("RestorePartyShotsForPlayer();", lottery_finish.group(1))


if __name__ == "__main__":
    unittest.main()
