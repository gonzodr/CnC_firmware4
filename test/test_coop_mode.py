"""Source contracts for the shared-state CO-OP ruleset milestone."""

import re
import unittest
from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[1] / "CnC_firmware4.ino").read_text(
    encoding="utf-8"
)


class CoopModeTests(unittest.TestCase):
    def test_score_and_progress_use_shared_zero_slot_only_in_coop(self):
        score_owner = re.search(
            r"uint8_t ScoreOwner\(\) \{(.*?)\n\}", SOURCE, re.DOTALL
        )
        progress_owner = re.search(
            r"uint8_t ProgressOwner\(\) \{(.*?)\n\}", SOURCE, re.DOTALL
        )
        self.assertIsNotNone(score_owner)
        self.assertIsNotNone(progress_owner)
        for body in (score_owner.group(1), progress_owner.group(1)):
            self.assertIn("runningGameMode == GAME_COOP", body)
            self.assertIn("return 0;", body)
            self.assertIn("return (uint8_t)player;", body)

    def test_running_mode_is_latched_once_at_game_start(self):
        self.assertIn("GameMode runningGameMode = GAME_STANDARD;", SOURCE)
        game_start = re.search(
            r"void SendGameStart\(\) \{(.*?)\n\}", SOURCE, re.DOTALL
        )
        self.assertIsNotNone(game_start)
        self.assertIn("runningGameMode = selectedGameMode;", game_start.group(1))
        self.assertIn("Serial.print((uint8_t)runningGameMode);", game_start.group(1))

    def test_all_coop_progress_fields_are_routed_through_owner(self):
        for field in (
            "beerCredits",
            "jointStack",
            "weedQualified",
            "cncCollectionLit",
            "cheechCollectives",
            "chongCollectives",
            "weedm",
            "weedmeter",
        ):
            self.assertNotRegex(SOURCE, rf"{field}\[player\]")

    def test_coop_has_short_normal_save_but_keeps_multiball_save(self):
        self.assertIn("NORMAL_BALL_SAVE_MS_COOP = 5000UL", SOURCE)
        self.assertRegex(
            SOURCE,
            r"(?s)NormalBallSaveDurationMs\(\).*?GAME_COOP.*?"
            r"NORMAL_BALL_SAVE_MS_COOP",
        )
        self.assertIn("StartBallSave(MULTIBALL_BALL_SAVE_MS);", SOURCE)

    def test_team_extra_ball_is_limited_once_and_stays_on_current_turn(self):
        self.assertIn("boolean coopTeamExtraBallAwarded = LOW;", SOURCE)
        self.assertIn("coopTeamExtraBallAwarded = HIGH;", SOURCE)
        self.assertIn("coopTeamExtraBallAwarded = LOW;", SOURCE)
        self.assertRegex(
            SOURCE,
            r"(?s)TryAwardExtraBall\(\).*?coopTeamExtraBallAwarded.*?"
            r"extraball = 1;",
        )
        self.assertNotIn("coopExtraBallOwner", SOURCE)

    def test_serial_score_uses_the_team_owner(self):
        self.assertRegex(
            SOURCE,
            r'(?s)"score,%lu,%d,%d,%d,%lu,%d,%u".*?'
            r'score\[ScoreOwner\(\)\].*?runningGameMode',
        )

    def test_coop_never_steals_score_from_a_teammate(self):
        self.assertEqual(
            SOURCE.count("numofplayers == 1 || runningGameMode == GAME_COOP"),
            2,
        )

    def test_shared_qualification_survives_player_changes(self):
        init_table = re.search(
            r"void inittable\(\) \{(.*?)\n\}", SOURCE, re.DOTALL
        )
        self.assertIsNotNone(init_table)
        self.assertRegex(
            init_table.group(1),
            r"(?s)runningGameMode != GAME_COOP.*?"
            r"weedQualified\[ProgressOwner\(\)\] = LOW",
        )

    def test_held_team_joint_settles_only_on_the_last_team_turn(self):
        self.assertIn(
            "runningGameMode != GAME_COOP || player == numofplayers", SOURCE
        )


if __name__ == "__main__":
    unittest.main()
