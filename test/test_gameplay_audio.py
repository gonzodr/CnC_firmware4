"""Source-level contracts for ball, combo and weed-multiball audio."""

import re
import unittest
from pathlib import Path


SKETCH = (Path(__file__).resolve().parents[1] / "CnC_firmware4.ino").read_text(
    encoding="utf-8"
)


class GameplayAudioContractTests(unittest.TestCase):
    def test_chong_hit_random_pool_includes_track_92(self):
        self.assertRegex(
            SKETCH,
            r"chongTracks\[11\]\s*=\s*\{[^}]*\b92\s*\};",
        )
        self.assertRegex(SKETCH, r"else \{\s*PlaySpeech\(chongTracks, 11\);")

    def test_multiball_identity_order_matches_ball_count(self):
        voice_table = re.search(
            r"mbVoice\[4\]\s*=\s*\{(.*?)\};", SKETCH, re.DOTALL
        )
        self.assertIsNotNone(voice_table)
        self.assertEqual(
            re.findall(r"TRK_VO_MULTIBALL_[A-Z_]+", voice_table.group(1)),
            [
                "TRK_VO_MULTIBALL_ACAPULCO_A",
                "TRK_VO_MULTIBALL_MICHOACAN_A",
                "TRK_VO_MULTIBALL_THAISTICK_A",
                "TRK_VO_MULTIBALL_LABRADOR_A",
            ],
        )
        self.assertIn("BIP = lvl + 2;", SKETCH)
        self.assertRegex(
            SKETCH, r"mbLightEffect\[4\]\s*=\s*\{\s*40,\s*20,\s*20,\s*41\s*\}"
        )

    def test_multiball_loop_is_enabled_and_started(self):
        self.assertRegex(
            SKETCH,
            r"trackLoop\(mbLoop\[lvl\], 1\);[\s\S]*?trackPlayPoly\(mbLoop\[lvl\]\);",
        )

    def test_combo_uses_finishing_bridge_character_voice(self):
        self.assertNotIn("TRK_COMBO1", SKETCH)
        self.assertNotIn("TRK_COMBO2", SKETCH)
        self.assertRegex(SKETCH, r"#define TRK_VO_CHEECH_COMBO_A\s+317")
        self.assertRegex(SKETCH, r"#define TRK_VO_CHONG_COMBO_A\s+320")
        self.assertIn("PlaySpeechRange(comboVoiceTrack);", SKETCH)
        self.assertRegex(
            SKETCH,
            r"&comboTimerH, &comboTimerL, 9,\s*TRK_VO_CHONG_COMBO_A",
        )
        self.assertRegex(
            SKETCH,
            r"&comboTimerL, &comboTimerH, 36,\s*TRK_VO_CHEECH_COMBO_A",
        )

    def test_new_round_returns_to_the_speaking_launch_path(self):
        wrap = re.search(
            r"if \(player > numofplayers\).*?if \(ball == 4\).*?else \{(.*?)\n\s*\}",
            SKETCH,
            re.DOTALL,
        )
        self.assertIsNotNone(wrap)
        self.assertIn("shoot = 0;", wrap.group(1))
        self.assertNotIn("shoot = 1;", wrap.group(1))
        self.assertIn("PlaySpeechRange(TRK_VO_CHEECH_BALL_LAUNCH_A);", SKETCH)


if __name__ == "__main__":
    unittest.main()
