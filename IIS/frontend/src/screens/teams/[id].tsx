/**
 * IIS Project
 * @brief Team detail, its matches and forms
 * @author Dmitrii Ivanushkin, Albert Tikaiev
 */
import PlayerCard from "@/components/cards/PlayerCard";
import { getTeamById } from "@/lib/api/teams/getTeamById";
import { keepPreviousData, useMutation, useQuery } from "@tanstack/react-query";
import { useParams } from "react-router";
import LoadingScreen from "../misc/LoadingScreen";
import ErrorScreen from "../misc/ErrorScreen";
import Avatar from "@/components/Avatar";
import { useRef, useState, type ChangeEvent } from "react";
import toast from "react-hot-toast";
import { useUser } from "@/lib/contexts/useUser";
import Modal from "@/components/Modal";
import TeamForm from "@/components/forms/TeamForm";
import { putTeam } from "@/lib/api/teams/putTeam";
import { Pen } from "lucide-react";
import WinrateStats from "@/components/statistics/WinrateStats";
import DisciplineStats from "@/components/statistics/DisciplineStats";
import ActivityStats from "@/components/statistics/ActivityStats";
import { ITEMS_PER_PAGE_MINIMAL } from "@/lib/config/constants";
import { createQueryParams } from "@/lib/utils/createQueryParams";
import { getTournaments } from "@/lib/api/tournaments/getTournaments";
import TournamentCard from "@/components/cards/TournamentCard";
import Pagination from "@/components/Pagination";
import { getMatches } from "@/lib/api/matches/getMatches";
import MatchProfileCard from "@/components/cards/MatchProfileCard";
import { putTeamAvatar } from "@/lib/api/teams/putTeamAvatar";
import WinningsStats from "@/components/statistics/WinningsStats";
import TeamManager from "@/components/TeamManager";
import { getProfileDetails } from "@/lib/api/users/getProfileDetails";

export default function Team() {
  const { id } = useParams();
  const { user, isLoading } = useUser();
  const [isModalOpen, setIsModalOpen] = useState(false);
  const [currentTournamentPage, setCurrentTournamentPage] = useState(1);
  const [currentMatchPage, setCurrentMatchPage] = useState(1);
  const fileInput = useRef<HTMLInputElement | null>(null);

  const {
    data: team,
    isLoading: teamLoading,
    error: teamError,
    refetch,
  } = useQuery({
    queryFn: () => getTeamById(id!),
    queryKey: ["team", id],
    enabled: Boolean(id),
  });

  const tournamentParams = createQueryParams({
    team: team?.id.toString(),
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
    enabled: Boolean(team),
  });

  const matchParams = createQueryParams({
    team: team?.id.toString(),
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
    enabled: Boolean(team),
  });

  const detailParams = createQueryParams({
    detail: "teams",
  });

  const { data: details, isLoading: detailsLoading } = useQuery({
    queryFn: () => getProfileDetails(detailParams),
    queryKey: ["profile-details", detailParams],
    enabled: Boolean(user && team && user.id === team.manager.id),
  });

  const { mutate: updateMutate, isPending: isUpdating } = useMutation({
    mutationFn: putTeam,
    onSuccess: () => {
      toast.success("Team updated successfully");
      refetch();
      setIsModalOpen(false);
    },
  });

  const { mutate: avatarMutate } = useMutation({
    mutationFn: putTeamAvatar,
    onSuccess: () => {
      toast.success("Successfully changed avatar");
      refetch();
    },
  });

  const updateAvatar = async (e: ChangeEvent<HTMLInputElement>) => {
    e.preventDefault();
    e.stopPropagation();
    avatarMutate({ file: e.target.files![0], id: id! });
  };

  if (
    teamLoading ||
    (!team && !teamError) ||
    tournamentsLoading ||
    (!paginatedTournaments && !tournamentsError) ||
    matchesLoading ||
    (!paginatedMatches && !matchesError) ||
    isLoading
  ) {
    return <LoadingScreen />;
  }

  if (teamError) {
    return <ErrorScreen message={teamError.message} />;
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
        <div className="flex flex-col sm:flex-row items-center gap-6">
          <Avatar
            image={team.image}
            name={team.name}
            width={100}
            onClick={
              user && user.id === team.manager.id
                ? () => fileInput.current?.click()
                : undefined
            }
            height={100}
          />
          <input
            type="file"
            ref={fileInput}
            className="hidden"
            onChange={updateAvatar}
          />
          <h1 className="text-4xl md:text-6xl text-gray-50 font-extrabold leading-tight tracking-tight">
            {team.name}
          </h1>
          {user && user.id === team.manager.id && (
            <button
              className="flex font-medium p-3 cursor-pointer text-center items-center justify-center text-gray-400 hover:text-gray-500 border-b border-dashed"
              onClick={() => setIsModalOpen(true)}
            >
              <Pen className="w-6 h-6" />
              <span className="ml-4">Edit</span>
            </button>
          )}
        </div>
      </section>

      <div className="max-w-6xl mx-auto flex flex-wrap justify-center gap-12 p-8 md:p-16">
        <section className="flex-1 min-w-[300px] max-w-lg flex flex-col gap-4">
          {team.description && team.description !== "" && (
            <article className="flex flex-col gap-4 mb-5">
              <h2 className="heading-2">Description</h2>
              <article className="w-full card flex flex-col p-4 text-lg break-words">
                <span>{team.description}</span>
              </article>
            </article>
          )}
          <article className="flex flex-col gap-4">
            <h2 className="heading-2">Statistics</h2>
            <WinningsStats value={team.winnings} />
            <WinrateStats data={team.winrate} />
            <DisciplineStats data={team.disciplines ?? []} />
            <ActivityStats data={team.activity ?? []} personal={false} />
          </article>
        </section>

        <section className="flex-1 min-w-[300px] max-w-lg flex flex-col gap-8">
          <article className="flex flex-col gap-4">
            <h2 className="heading-2">Team members</h2>
            <PlayerCard player={team.manager} isManager />
            {user &&
            user.id === team.manager.id &&
            details?.managing &&
            details.managing.find((t) => t.id === team.id) &&
            !detailsLoading ? (
              <TeamManager
                initialTeam={details.managing.find((t) => t.id === team.id)!}
                withoutHeader
              />
            ) : (
              <ul className="divide-y divide-gray-200 bg-white rounded-lg shadow">
                <li>
                  <PlayerCard player={team.manager} variant="ghost" isManager />
                </li>
                {team.players &&
                  team.players.map((player) => (
                    <li key={player.id}>
                      <PlayerCard player={player} variant="ghost" />
                    </li>
                  ))}
              </ul>
            )}
            {team.players?.length === 0 && (
              <span className="w-full text-center py-4 text-gray-500">
                No members
              </span>
            )}
          </article>

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

      {user && user.id === team.manager.id && (
        <Modal
          open={isModalOpen}
          onClose={setIsModalOpen}
          title="Edit Tournament"
        >
          <TeamForm
            initialData={{
              name: team.name,
              description: team.description,
              email_invites: [],
            }}
            onSubmit={(formData) =>
              updateMutate({ id: team.id, data: { ...formData } })
            }
            isSubmitting={isUpdating}
            actionName={"Save"}
          />
        </Modal>
      )}
    </main>
  );
}
