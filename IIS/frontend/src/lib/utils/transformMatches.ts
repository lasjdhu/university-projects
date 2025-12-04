/**
 * IIS Project
 * @brief Converts BE data to the data for Bracketry
 * @author Dmitrii Ivanushkin
 */
import type { BracketMatch } from "@/lib/api/types";
import dayjs from "dayjs";

export function transformMatches(matches: BracketMatch[]) {
  const roundsMap = new Map<string, BracketMatch[]>();
  matches.forEach((m) => {
    if (!roundsMap.has(m.tournament_round_text)) {
      roundsMap.set(m.tournament_round_text, []);
    }
    roundsMap.get(m.tournament_round_text)!.push(m);
  });

  const sortedRounds = Array.from(roundsMap.entries()).sort(
    ([a], [b]) => Number(a) - Number(b),
  );

  const rounds = sortedRounds.map(([roundText]) => ({
    name: roundText,
  }));

  const contestants: Record<string, unknown> = {};
  let placeholderIdCounter = 1;

  const bracketMatches = sortedRounds.flatMap(([, roundMatches], roundIndex) =>
    roundMatches.map((m, order) => ({
      originalId: m.id,
      roundIndex,
      order,
      sides: m.participants.map((p) => {
        const contestantId =
          p.id !== null
            ? p.id.toString()
            : `placeholder_${placeholderIdCounter++}`;
        const playerName = p.name ?? "TBD";
        contestants[contestantId] = { players: [{ title: playerName }] };

        return {
          contestantId,
          currentScore: p.result_text || "0",
          isWinner: !!p.is_winner,
        };
      }),
      matchStatus:
        dayjs(m.date).format("LL") == "Invalid Date"
          ? "TBD"
          : dayjs(m.date).format("LL"),
    })),
  );

  return {
    rounds,
    matches: bracketMatches,
    contestants,
  };
}
