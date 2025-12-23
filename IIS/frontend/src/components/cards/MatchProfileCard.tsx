/**
 * IIS Project
 * @brief Match info card
 * @author Albert Tikaiev, Dmitrii Ivanushkin
 */
import type { MatchDetailed } from "@/lib/api/types";
import dayjs from "dayjs";
import { Clock, Crown, Swords, Trophy } from "lucide-react";
import { Link } from "react-router";

interface MatchProfileCardProps {
  match: MatchDetailed;
}

export default function MatchProfileCard({ match }: MatchProfileCardProps) {
  const correspondingPage = match.type === "Person" ? "players" : "teams";

  return (
    <article className="w-full flex flex-col md:flex-row md:justify-between items-center p-4 gap-2">
      <div className="flex justify-between w-full md:w-auto gap-8">
        <section className="flex items-center">
          <Trophy className="w-4 h-4 text-gray-400" />
          <Link
            to={`/tournaments/${match.tournament_id}`}
            className="link font-medium ml-3"
          >
            {match.tournament_name}
          </Link>
        </section>

        <section className="flex items-center">
          <Clock className="w-4 h-4 text-gray-500" />
          <span className="ml-3 text-gray-500 font-medium">
            {match.date ? dayjs(match.date).format("LL") : "TBD"}
          </span>
        </section>
      </div>

      <section className="flex items-center justify-center gap-8">
        <div className="flex items-center gap-2">
          {match.first.is_winner && <Crown className="text-yellow-500" />}
          <Link
            to={`/${correspondingPage}/${match.first.id}`}
            className="link font-medium"
          >
            {match.first.name}
          </Link>
        </div>

        <div className="flex text-2xl gap-4 items-center justify-center">
          <span>{match.first.score}</span>
          <Swords className="w-7 h-7" />
          <span>{match.second.score}</span>
        </div>

        <div className="flex items-center gap-2 justify-end">
          <Link
            to={`/${correspondingPage}/${match.second.id}`}
            className="link font-medium"
          >
            {match.second.name}
          </Link>
          {match.second.is_winner && <Crown className="text-yellow-500" />}
        </div>
      </section>
    </article>
  );
}
