"""Source-level jackpot audio contract; no Arduino or audio hardware required.

Checks the sparse track allocation and both gameplay call sites. Runtime
randomness, audible mixing and switch behavior still need cabinet testing.
"""

import re
import unittest
from pathlib import Path


SKETCH = (Path(__file__).resolve().parents[1] / "CnC_firmware4.ino").read_text(
    encoding="utf-8"
)
PAIRS = {
    10000: (209, 210),
    15000: (211, 212),
    20000: (213, 214),
    25000: (221, 222),
    30000: (299, 300),
    50000: (305, 306),
    100000: (307, 308),
}


def function_body(name):
    match = re.search(
        rf"^(?:void|unsigned long) {name}\([^\n]*\) \{{(.*?)^\}}",
        SKETCH,
        re.MULTILINE | re.DOTALL,
    )
    if match is None:
        raise AssertionError(f"Missing firmware function: {name}")
    return match.group(1)


class JackpotAudioContractTests(unittest.TestCase):
    def test_all_seven_amounts_use_the_allocated_adjacent_speaker_tracks(self):
        constants = {
            int(amount): int(track)
            for amount, track in re.findall(
                r"^#define TRK_VO_JACKPOT_(\d+)_A\s+(\d+)$", SKETCH, re.MULTILINE
            )
        }
        self.assertEqual(constants, {amount: pair[0] for amount, pair in PAIRS.items()})
        for amount, (cheech, chong) in PAIRS.items():
            self.assertEqual(chong, cheech + 1)
        self.assertEqual(len({track for pair in PAIRS.values() for track in pair}), 14)
        cases = re.findall(
            r"case (\d+)UL:\s+firstTrack = TRK_VO_JACKPOT_(\d+)_A;",
            function_body("PlayJackpotFeedback"),
        )
        self.assertEqual({int(a): int(b) for a, b in cases}, {n: n for n in PAIRS})

    def test_exactly_one_of_two_voices_and_16_bit_track_numbers(self):
        body = function_body("PlayJackpotFeedback")
        self.assertIn("uint16_t firstTrack", body)
        self.assertEqual(body.count("PlaySpeechRange(firstTrack, 2);"), 1)
        self.assertRegex(body, r"if \(firstTrack != 0\).*?", "Missing known-track guard")
        self.assertRegex(body, r"else\s*\{[^}]*wTrig.trackPlayPoly\(TRK_JACKPOT\);")
        speech = function_body("PlaySpeechRange")
        self.assertIn("uint16_t track", speech)
        self.assertIn("random(0, count)", speech)
        self.assertEqual(speech.count("wTrig.trackPlayPoly"), 1)

    def test_video_voice_and_score_share_the_final_promoted_amount(self):
        body = function_body("PlayJackpotFeedback")
        self.assertIn("unsigned long points = JackpotScorePoints(basePoints);", body)
        self.assertIn("Serial.println(points);", body)
        self.assertIn("switch (points)", body)
        self.assertIn("JackpotScorePoints(scr)", function_body("ScoreJackpot"))
        points = function_body("JackpotScorePoints")
        self.assertIn("DirectScorePoints(basePoints)", points)
        self.assertIn("Scoring::JACKPOT_TIERS[i] >= points", points)
        self.assertIn("return Scoring::JACKPOT_TIERS[7];", points)

    def test_loop_and_both_bridges_use_shared_feedback_without_old_voice_overlay(self):
        self.assertIn(
            "PlayJackpotFeedback(Scoring::LOOP_JACKPOT_POINTS, 1);",
            function_body("Loopshoot"),
        )
        # BridgeCommon has a multiline signature, so inspect the call in the sketch.
        self.assertEqual(SKETCH.count("PlayJackpotFeedback(jpScr[multiball], 19);"), 1)
        # Only the unknown-amount fallback may still play the old generic voice.
        self.assertEqual(SKETCH.count("wTrig.trackPlayPoly(TRK_JACKPOT);"), 1)
        self.assertNotIn("SendJackpotVideo(", SKETCH)


if __name__ == "__main__":
    unittest.main()
