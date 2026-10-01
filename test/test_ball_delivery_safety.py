"""Regression contracts for physical ball accounting and flipper handoff."""

import re
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MAIN = (ROOT / "CnC_firmware4.ino").read_text(encoding="utf-8")
GUARD = (ROOT / "e_coil_guard.ino").read_text(encoding="utf-8")


class BallDeliverySafetyTests(unittest.TestCase):
    def test_trough_feed_uses_virtual_count_and_settle_window(self):
        self.assertIn("BALL_TROUGH_SETTLE_MS = 1200UL", MAIN)
        self.assertIn("boolean troughMeasurementInhibited = LOW;", MAIN)
        begin = re.search(
            r"boolean BeginTroughFeed\(\) \{(.*?)\n\}", MAIN, re.DOTALL
        )
        self.assertIsNotNone(begin)
        self.assertIn("BIS--;", begin.group(1))
        self.assertIn("troughMeasurementInhibited = HIGH;", begin.group(1))
        self.assertRegex(
            MAIN,
            r"if \(troughMeasurementInhibited == HIGH\) \{\s*"
            r"if \(millis\(\) - troughMeasurementInhibitAt < "
            r"BALL_TROUGH_SETTLE_MS\) \{\s*return;",
        )

    def test_dave_lane_save_still_feeds_immediately(self):
        self.assertRegex(
            MAIN,
            r"if \(sidelaneBallsaverSw == HIGH && BIS != 0\) \{\s*"
            r"sidelaneBallsaverSw = LOW;\s*"
            r"if \(BeginTroughFeed\(\)\)",
        )

    def test_drain_requires_armed_stable_full_trough(self):
        self.assertIn("BALL_DRAIN_CONFIRM_MS = 400UL", MAIN)
        self.assertIn("BALL_DRAIN_ARM_MS = 250UL", MAIN)
        self.assertRegex(
            MAIN,
            r"millis\(\) - bisFiveSince >= BALL_DRAIN_CONFIRM_MS &&\s*"
            r"ballDrainArmed == HIGH",
        )
        self.assertIn("!BallDrainTemporarilyBlocked()", MAIN)

    def test_known_ball_locations_veto_false_drain(self):
        blocked = re.search(
            r"boolean BallDrainTemporarilyBlocked\(\) \{(.*?)\n\}",
            MAIN,
            re.DOTALL,
        )
        self.assertIsNotNone(blocked)
        for state in (
            "troughMeasurementInhibited",
            "shooterLaneSwitch",
            "ufoshoot",
            "ufoDetectStartedAt",
            "ufoWheelWaiting",
            "MunchiesOwnsGameLoop",
        ):
            self.assertIn(state, blocked.group(1))

    def test_regular_ufo_eject_is_one_guarded_50ms_pulse(self):
        self.assertIn("UFO_COIL_PULSE_MS = 50UL", MAIN)
        start = re.search(
            r"void StartUfoEjectBallSave\(.*?\) \{(.*?)\n\}",
            MAIN,
            re.DOTALL,
        )
        self.assertIsNotNone(start)
        self.assertIn("StartUfoCoilPulse();", start.group(1))
        ufo = MAIN.split("void UFOO() {", 1)[1].split("//// End UFO Rutin", 1)[0]
        self.assertIn("UpdateUfoCoilPulse();", ufo)
        self.assertNotIn("digitalWrite(ufoCoil, HIGH);", ufo)

    def test_flippers_are_not_registered_with_coil_guard(self):
        self.assertIn("#define GUARD_COILS 8", GUARD)
        self.assertIn("*c.outReg &= ~c.mask", GUARD)
        self.assertIn("CoilGuardAdd(7, ufoCoil, 200)", GUARD)
        self.assertNotIn("leftFlipperBat", GUARD)
        self.assertNotIn("rightFlipperBat", GUARD)
        self.assertNotIn('"flipL"', GUARD)
        self.assertNotIn('"flipR"', GUARD)

    def test_trough_filter_uses_median_hysteresis_and_temporal_confirmation(self):
        analog = (ROOT / "h_analog_test.ino").read_text(encoding="utf-8")
        self.assertIn("int samples[7];", analog)
        self.assertIn("return samples[3];", analog)
        self.assertIn("TROUGH_SENSOR_HYSTERESIS = 12U", MAIN)
        self.assertIn("TROUGH_SENSOR_CONFIRM_READS = 4", MAIN)
        self.assertIn("TROUGH_SENSOR_CONFIRM_MS = 100UL", MAIN)
        self.assertIn("FilterTroughPresence(0, ball1, now)", MAIN)
        self.assertIn("ResetTroughSensorFilters();", MAIN)

    def test_next_ball_waits_for_gui_summary_done(self):
        control = (ROOT / "d_light_effects.ino").read_text(encoding="utf-8")
        self.assertIn("SUMMARY_WAIT_TIMEOUT_MS = 15000UL", MAIN)
        self.assertIn("summaryWaitActive = HIGH;", MAIN)
        self.assertIn('Serial.print(F("Next,"));', MAIN)
        self.assertIn("Serial.println(summarySession);", MAIN)
        self.assertNotIn("delay(4500);", MAIN)
        self.assertRegex(
            MAIN,
            r"firstplay == HIGH && summaryWaitActive == LOW && shoot == 0",
        )
        self.assertIn('strncmp(s, "SUMMARY_DONE,", 13) == 0', control)
        self.assertIn("session == summarySession", control)
        self.assertIn('Serial.print(F("SUMMARY_ACK,"))', control)
        self.assertIn("summaryWaitStartedAt = 0;", MAIN)

    def test_extra_ball_uses_the_same_summary_gate(self):
        self.assertIn("boolean nextBallIsExtra = LOW;", MAIN)
        launch_branch = re.search(
            r"if \(nextBallIsExtra == HIGH\) \{(.*?)\n\s*\}",
            MAIN,
            re.DOTALL,
        )
        self.assertIsNotNone(launch_branch)
        self.assertIn(
            "wTrig.trackPlaySolo(TRK_VO_CHONG_EXTRA_BALL_A)",
            launch_branch.group(1),
        )
        extra_branch = re.search(
            r"else \{\s*extraball = extraball - 1;(.*?)\n\s*\}",
            MAIN,
            re.DOTALL,
        )
        self.assertIsNotNone(extra_branch)
        self.assertIn("nextBallIsExtra = HIGH;", extra_branch.group(1))
        self.assertNotIn("BeginTroughFeed();", extra_branch.group(1))


if __name__ == "__main__":
    unittest.main()
