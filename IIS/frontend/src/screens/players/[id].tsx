/**
 * IIS Project
 * @brief Player detail, their matches and forms
 * @author Dmitrii Ivanushkin, Albert Tikaiev
 */
import { useParams } from "react-router";
import { keepPreviousData, useQuery } from "@tanstack/react-query";
import { getPlayerById } from "@/lib/api/players/getPlayerById";
import LoadingScreen from "../misc/LoadingScreen";
import ErrorScreen from "../misc/ErrorScreen";
import WinrateStats from "@/components/statistics/WinrateStats";
import DisciplineStats from "@/components/statistics/DisciplineStats";
import ActivityStats from "@/components/statistics/ActivityStats";
import { ITEMS_PER_PAGE_MINIMAL } from "@/lib/config/constants";
import { useState } from "react";
import { createQueryParams } from "@/lib/utils/createQueryParams";
import { getTeams } from "@/lib/api/teams/getTeams";
import TeamCard from "@/components/cards/TeamCard";
import Pagination from "@/components/Pagination";
import { getTournaments } from "@/lib/api/tournaments/getTournaments";
import TournamentCard from "@/components/cards/TournamentCard";
import { getMatches } from "@/lib/api/matches/getMatches";
import MatchProfileCard from "@/components/cards/MatchProfileCard";
import WinningsStats from "@/components/statistics/WinningsStats";

export default function Player() {
  const { id } = useParams();
  const [currentTeamPage, setCurrentTeamPage] = useState(1);
  const [currentTournamentPage, setCurrentTournamentPage] = useState(1);
  const [currentMatchPage, setCurrentMatchPage] = useState(1);

  const {
    data: player,
    isLoading: playerLoading,
    error: playerError,
  } = useQuery({
    queryFn: () => getPlayerById(id!),
    queryKey: ["player", id],
    enabled: Boolean(id),
  });

  const teamsParams = createQueryParams({
    user: player?.id.toString(),
    page: currentTeamPage.toString(),
    limit: ITEMS_PER_PAGE_MINIMAL.toString(),
  });

  const {
    data: paginatedTeams,
    isLoading: teamsLoading,
    error: teamsError,
  } = useQuery({
    queryFn: () => getTeams(teamsParams),
    queryKey: ["teams", teamsParams],
    placeholderData: keepPreviousData,
    enabled: Boolean(player),
  });

  const tournamentParams = createQueryParams({
    user: player?.id.toString(),
    page: currentTournamentPage.toString(),
    limit: ITEMS_PER_PAGE_MINIMAL.toString(),
  });

  const {
    data: paginatedTournaments,
    isLoading: tournamentsLoading,
    error: tournamentsError,
  } = useQuery({
    queryFn: () => getTournaments(tournamentParams),
    queryKey: ["tournaments", tournamentParams],
    placeholderData: keepPreviousData,
    enabled: Boolean(player),
  });

  const matchParams = createQueryParams({
    user: player?.id.toString(),
    page: currentMatchPage.toString(),
    limit: ITEMS_PER_PAGE_MINIMAL.toString(),
  });

  const {
    data: paginatedMatches,
    isLoading: matchesLoading,
    error: matchesError,
  } = useQuery({
    queryFn: () => getMatches(matchParams),
    queryKey: ["matches", matchParams],
    placeholderData: keepPreviousData,
    enabled: Boolean(player),
  });

  if (
    playerLoading ||
    (!player && !playerError) ||
    teamsLoading ||
    (!paginatedTeams && !teamsError) ||
    tournamentsLoading ||
    (!paginatedTournaments && !tournamentsError) ||
    matchesLoading ||
    (!paginatedMatches && !matchesError)
  ) {
    return <LoadingScreen />;
  }

  if (playerError) {
    return <ErrorScreen message={playerError.message} />;
  }

  if (teamsError) {
    return <ErrorScreen message={teamsError.message} />;
  }

  if (tournamentsError) {
    return <ErrorScreen message={tournamentsError.message} />;
  }

  if (matchesError) {
    return <ErrorScreen message={matchesError.message} />;
  }

  return (
    <main className="flex-1 bg-gray-100 pt-16">
      <section className="bg-black flex flex-col justify-center items-center text-center px-4 py-16 md:py-24">
        <h1 className="text-4xl md:text-6xl text-gray-50 font-extrabold leading-tight tracking-tight">
          {player.name} {player.surname}
        </h1>
      </section>

      <div className="max-w-6xl mx-auto flex flex-wrap justify-center gap-12 p-8 md:p-16">
        <section className="flex-1 min-w-[300px] max-w-lg flex flex-col gap-8">
          <article className="flex flex-col gap-4">
            <h2 className="heading-2">Statistics</h2>
            <WinningsStats value={player.winnings} />
            <WinrateStats data={player.winrate} />
            <DisciplineStats data={player.disciplines ?? []} />
            <ActivityStats data={player.activity ?? []} personal={true} />
          </article>
        </section>

        <section className="flex-1 min-w-[300px] max-w-lg flex flex-col gap-8">
          <article className="flex flex-col gap-4">
            <h2 className="heading-2">Tournaments history</h2>
            <div className="w-full flex flex-col">
              <ul className="divide-y divide-gray-200 bg-white rounded-tl-lg rounded-tr-lg shadow">
                {paginatedTournaments.data &&
                  paginatedTournaments.data.map((tournament) => (
                    <li key={tournament.id}>
                      <TournamentCard
                        tournament={tournament}
                        variant="ghost"
                        layout="horizontal"
                      />
                    </li>
                  ))}
              </ul>

              {paginatedTournaments.total_pages > 0 ? (
                <Pagination
                  totalItems={paginatedTournaments.total_records}
                  itemsPerPage={ITEMS_PER_PAGE_MINIMAL}
                  currentPage={paginatedTournaments.page}
                  onPageChange={setCurrentTournamentPage}
                />
              ) : (
                <span className="w-full text-center py-4 text-gray-500">
                  No tournaments
                </span>
              )}
            </div>
          </article>

          <article className="flex flex-col gap-4">
            <h2 className="heading-2">Member of teams</h2>
            <div className="w-full flex flex-col">
              <ul className="divide-y divide-gray-200 bg-white rounded-tl-lg rounded-tr-lg shadow">
                {paginatedTeams.data &&
                  paginatedTeams.data.map((team) => (
                    <li key={team.id}>
                      <TeamCard team={team} variant="ghost" />
                    </li>
                  ))}
              </ul>

              {paginatedTeams.total_pages > 0 ? (
                <Pagination
                  totalItems={paginatedTeams.total_records}
                  itemsPerPage={ITEMS_PER_PAGE_MINIMAL}
                  currentPage={paginatedTeams.page}
                  onPageChange={setCurrentTeamPage}
                />
              ) : (
                <span className="w-full text-center py-4 text-gray-500">
                  No teams
                </span>
              )}
            </div>
          </article>
        </section>
      </div>

      <article className="max-w-6xl mx-auto flex flex-col pt-0 md:pt-0 p-8 md:p-16">
        <h2 className="heading-2 mb-4">Matches</h2>
        <ul className="divide-y divide-gray-200 bg-white rounded-tl-lg rounded-tr-lg shadow">
          {paginatedMatches.data.map((value) => (
            <li key={value.id}>
              <MatchProfileCard match={value} />
            </li>
          ))}
        </ul>
        {paginatedMatches.total_records > 0 ? (
          <Pagination
            totalItems={paginatedMatches.total_records}
            itemsPerPage={ITEMS_PER_PAGE_MINIMAL}
            currentPage={paginatedMatches.page}
            onPageChange={setCurrentMatchPage}
          />
        ) : (
          <span className="w-full text-center py-4 text-gray-500">
            No matches
          </span>
        )}
      </article>
    </main>
  );
}
