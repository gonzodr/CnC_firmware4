"""Contracts for shared feature entry points used by future game modes."""

import re
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MAIN = (ROOT / "CnC_firmware4.ino").read_text(encoding="utf-8")
MUNCHIES = (ROOT / "g_munchies_mode.ino").read_text(encoding="utf-8")


class SharedFeatureEntryPointTests(unittest.TestCase):
    def test_weed_targets_delegate_completion_to_policy_entry_point(self):
        weed = re.search(r"void Weed\(\) \{(.*?)\n\}", MAIN, re.DOTALL)
        self.assertIsNotNone(weed)
        self.assertIn("OnWeedCompleted();", weed.group(1))
        self.assertIn("void OnWeedCompleted()", MAIN)

    def test_spinner_delegates_multiball_start(self):
        spinner = re.search(r"void Weedspinner\(\) \{(.*?)\n\}", MAIN, re.DOTALL)
        self.assertIsNotNone(spinner)
        self.assertIn(
            "StartWeedMultiball((uint8_t)lvl, WEED_MB_FROM_QUALIFICATION);",
            spinner.group(1),
        )
        self.assertIn("boolean StartWeedMultiball(uint8_t level, uint8_t context)", MAIN)
        self.assertIn("WEED_MB_FROM_CHALLENGE", MAIN)

    def test_ufo_reward_selection_is_separate_from_execution(self):
        self.assertIn("int DrawStandardUfoLottery(uint8_t tier)", MAIN)
        self.assertIn("int DrawUfoRewardForMode(uint8_t tier)", MAIN)
        self.assertIn("lottery = DrawUfoRewardForMode(ufoAwardTier);", MAIN)
        self.assertIn("void ApplyUfoLotteryEntryAward(boolean playLegacyVideo)", MAIN)
        self.assertIn("void AwardUfoLottery()", MAIN)

    def test_current_munchies_callers_explicitly_use_vuk_context(self):
        self.assertEqual(MAIN.count("StartMunchiesMode(MUNCHIES_FROM_VUK);"), 2)
        self.assertNotIn("StartMunchiesMode();", MAIN)
        self.assertIn("void StartMunchiesMode(uint8_t context)", MUNCHIES)
        self.assertIn("MUNCHIES_STANDALONE_CHALLENGE", MAIN)
        self.assertIn(
            "munchiesStartContext == MUNCHIES_STANDALONE_CHALLENGE", MUNCHIES
        )


if __name__ == "__main__":
    unittest.main()
