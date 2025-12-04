/**
 * IIS Project
 * @brief Hero section, overview of tournaments, teams and players
 * @author Dmitrii Ivanushkin
 */
import TournamentCard from "@/components/cards/TournamentCard";
import Hero from "@/components/Hero";
import { Link } from "react-router";
import PlayerCard from "@/components/cards/PlayerCard";
import TeamCard from "@/components/cards/TeamCard";
import { useQuery } from "@tanstack/react-query";
import { getOverview } from "@/lib/api/overview/getOverview";
import LoadingScreen from "./misc/LoadingScreen";
import ErrorScreen from "./misc/ErrorScreen";

export default function Home() {
  const {
    data: overview,
    isLoading,
    error,
  } = useQuery({
    queryFn: getOverview,
    queryKey: ["overview"],
  });

  if (isLoading || (!overview && !error)) {
    return <LoadingScreen />;
  }

  if (error) {
    return <ErrorScreen message={error.message} />;
  }

  return (
    <main className="flex-1 flex flex-col bg-gray-100 gap-16 py-16">
      <Hero
        tournamentsCount={overview.tournaments_count}
        teamsCount={overview.teams_count}
        playersCount={overview.players_count}
      />

      <section className="md:mx-48 mx-8 flex flex-col gap-6">
        <div className="flex items-center justify-between">
          <h2 className="heading-1">Tournaments</h2>
          <Link to="/tournaments" className="link">
            View all -&gt;
          </Link>
        </div>

        <ul className="grid gap-6 grid-cols-1 md:grid-cols-2 lg:grid-cols-3 xl:grid-cols-4 place-items-center">
          {overview.tournaments.map((t) => (
            <li key={t.id} className="w-full">
              <TournamentCard tournament={t} />
            </li>
          ))}
        </ul>
      </section>

      <div className="flex flex-col md:flex-row justify-center md:mx-48 mx-8 gap-12">
        <section className="w-full md:w-1/2 flex flex-col gap-6">
          <div className="flex items-center justify-between">
            <h2 className="heading-1">Players</h2>
            <Link to="/players" className="link">
              View all -&gt;
            </Link>
          </div>

          <ul className="flex flex-col gap-4">
            {overview.players.map((p) => (
              <li key={p.id}>
                <PlayerCard player={p} />
              </li>
            ))}
          </ul>
        </section>

        <section className="w-full md:w-1/2 flex flex-col gap-6">
          <div className="flex items-center justify-between">
            <h2 className="heading-1">Teams</h2>
            <Link to="/teams" className="link">
              View all -&gt;
            </Link>
          </div>

          <ul className="flex flex-col gap-4">
            {overview.teams.map((team) => (
              <li key={team.id}>
                <TeamCard team={team} />
              </li>
            ))}
          </ul>
        </section>
      </div>
    </main>
  );
}
