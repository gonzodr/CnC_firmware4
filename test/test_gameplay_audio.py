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

    def test_thai_stick_and_labrador_use_their_assigned_music_tracks(self):
        self.assertRegex(SKETCH, r"#define TRK_MUS_THAI_STICK\s+63")
        self.assertRegex(SKETCH, r"#define TRK_MUS_LABRADOR\s+64")
        loop_table = re.search(r"mbLoop\[4\]\s*=\s*\{(.*?)\};", SKETCH, re.DOTALL)
        self.assertIsNotNone(loop_table)
        self.assertIn("TRK_MUS_THAI_STICK, TRK_MUS_LABRADOR", loop_table.group(1))

    def test_hurry_cashouts_randomly_use_cheech_or_chong_for_each_value(self):
        for amount, cheech, chong in (
            (15000, 323, 326), (25000, 324, 327), (30000, 325, 328),
        ):
            self.assertRegex(SKETCH, rf"TRK_VO_CHEECH_HURRY_{amount}\s+{cheech}")
            self.assertRegex(SKETCH, rf"TRK_VO_CHONG_HURRY_{amount}\s+{chong}")
        self.assertIn("random(0, 2) == 0 ? cheechTrack : chongTrack", SKETCH)
        self.assertIn("PlayHurryCashoutVoice(30000UL);", SKETCH)
        self.assertGreaterEqual(
            SKETCH.count("wTrig.trackPlayPoly(TRK_MULTIBALL_EXPLOSION);"), 3
        )
        self.assertIn(
            "PlayHurryCashoutVoice(hurryScr * Scoring::HURRY_UP_MULTIPLIER);",
            SKETCH,
        )

    def test_ufo_point_steal_uses_a_dedicated_voice_for_each_victim(self):
        for player, track in enumerate(range(124, 128), start=1):
            self.assertRegex(
                SKETCH, rf"TRK_UFO_MINUS_PLAYER{player}\s+{track}"
            )
        self.assertIn("static const uint16_t tracks[4]", SKETCH)
        self.assertIn("wTrig.trackPlayPoly(tracks[victim - 1]);", SKETCH)
        presentation_start = SKETCH.index("void BeginUfoLotteryPresentation")
        award_start = SKETCH.index("void AwardUfoLottery")
        voice_at = SKETCH.index("PlayUfoMinusVoice(ufoMinus);")
        self.assertGreater(voice_at, presentation_start)
        self.assertLess(voice_at, award_start)

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

    def test_space_coke_music_loops_until_multiball_end(self):
        self.assertRegex(
            SKETCH,
            r"trackLoop\(TRK_MUS_SPACECOKE, 1\);\s*"
            r"wTrig\.trackPlayPoly\(TRK_MUS_SPACECOKE\);",
        )

    def test_space_coke_audio_is_cued_from_ufo9_explosion_frame(self):
        presentation = re.search(
            r"void BeginUfoLotteryPresentation\(boolean playLegacyVideo\) \{(.*?)\n\}",
            SKETCH,
            re.DOTALL,
        )
        self.assertIsNotNone(presentation)
        body = presentation.group(1)
        self.assertIn("if (lottery == 7)", body)
        self.assertIn("wTrig.stopAllTracks();", body)
        self.assertIn("wTrig.trackPlayPoly(TRK_HAPPYUFO);", body)
        self.assertIn("spaceCokeAudioPending = HIGH;", body)
        self.assertRegex(SKETCH, r"SPACECOKE_EXPLOSION_CUE_MS\s*=\s*3500UL")
        cue = SKETCH[SKETCH.index("void UpdateSpaceCokeAudioCue() {"):]
        for track in (
            "TRK_FIREWORK", "TRK_MUS_SPACECOKE", "TRK_CHEECH_SPACECOKE",
        ):
            self.assertIn(track, cue)

    def test_weed_full_adds_track_72_to_the_blast(self):
        weed = re.search(
            r"void OnWeedCompleted\(\) \{(.*?)\n\}", SKETCH, re.DOTALL
        )
        self.assertIsNotNone(weed)
        self.assertIn("wTrig.trackPlayPoly(TRK_WEEDFULL);", weed.group(1))

    def test_ufo_no_weed_eject_restores_legacy_effects(self):
        self.assertRegex(SKETCH, r"#define TRK_UFO_EJECT_BALL\s+34")
        self.assertRegex(SKETCH, r"#define TRK_UFO_GET_OUT\s+43")
        self.assertRegex(
            SKETCH,
            r"(?s)void StartUfoEjectBallSave\(.*?trackPlayPoly\(TRK_UFO_EJECT_BALL\)",
        )
        self.assertRegex(
            SKETCH,
            r"(?s)if \(ufoshoot == 2\).*?trackPlayPoly\(TRK_UFO_GET_OUT\).*?"
            r"PlaySpeechRange\(TRK_VO_UFO_NO_WEED_EJECT_A\)",
        )
        self.assertRegex(
            SKETCH,
            r"trackPause\(TRK_MUS_SPACECOKE\);\s*"
            r"wTrig\.trackLoop\(TRK_MUS_SPACECOKE, 0\);",
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
