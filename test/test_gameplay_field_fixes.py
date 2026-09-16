"""Field regression checks for ball scope, mode LEDs, loop and audio."""

import re
import unittest
from pathlib import Path


SKETCH = (Path(__file__).resolve().parents[1] / "CnC_firmware4.ino").read_text(
    encoding="utf-8"
)


class GameplayFieldFixes(unittest.TestCase):
    def test_weed_qualification_ends_with_ball(self):
        init = re.search(r"void inittable\(\) \{(.*?)\n\}", SKETCH, re.DOTALL)
        self.assertIsNotNone(init)
        self.assertIn("weedQualified[player] = LOW;", init.group(1))
        self.assertNotIn("jointStack[player] = 0;", init.group(1))

    def test_multiball_meter_uses_corrected_physical_led_order(self):
        for name, led in (
            ("LED_ACAPULCO", 59),
            ("LED_MICHOAKAN", 60),
            ("LED_THAI", 63),
            ("LED_LABRADOR", 62),
        ):
            self.assertRegex(SKETCH, rf"#define {name}\s+{led}\b")
        self.assertRegex(
            SKETCH,
            r"weedMeterLeds\[4\] = \{\s*LED_ACAPULCO, "
            r"LED_MICHOAKAN, LED_THAI, LED_LABRADOR",
        )

    def test_loop_side_edge_is_not_masked_by_top_switch(self):
        self.assertIn("boolean loopSideLatched = LOW;", SKETCH)
        self.assertIn("loopSideLatched == LOW", SKETCH)
        self.assertIn("millis() - looptimer < 1000UL", SKETCH)
        self.assertNotIn("looptimer && loopsw == LOW", SKETCH)
        self.assertIn("Scoring::HIGH_LOOP_COMBO_SCR[multiball]", SKETCH)

    def test_collectible_hit_does_not_use_inactive_voice_pool(self):
        for func, voice in (("Chong_switch", "chongTracks"),
                            ("Cheech_switch", "cheechTracks")):
            body = re.search(rf"void {func}\(\) \{{(.*?)\n\}}", SKETCH, re.DOTALL)
            self.assertIsNotNone(body)
            self.assertRegex(body.group(1),
                             rf"else \{{\s*PlaySpeech\({voice}, \d+\);")

    def test_weed_completion_has_blast_audio(self):
        weed = re.search(r"void Weed\(\) \{(.*?)\n\}", SKETCH, re.DOTALL)
        self.assertIsNotNone(weed)
        self.assertIn("wTrig.trackPlayPoly(TRK_MULTIBALL_EXPLOSION);", weed.group(1))


if __name__ == "__main__":
    unittest.main()
