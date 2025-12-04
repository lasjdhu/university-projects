/**
 * IIS Project
 * @brief Tournament detail, bracket and forms
 * @author Dmitrii Ivanushkin, Albert Tikaiev
 */
import { useState } from "react";
import { useParams } from "react-router";
import { useQuery, useMutation } from "@tanstack/react-query";
import toast from "react-hot-toast";

import PlayerCard from "@/components/cards/PlayerCard";
import TournamentInfoCard from "@/components/cards/TournamentInfoCard";
import LoadingScreen from "../misc/LoadingScreen";
import ErrorScreen from "../misc/ErrorScreen";
import Modal from "@/components/Modal";
import TournamentForm from "@/components/forms/TournamentForm";

import { getTournamentById } from "@/lib/api/tournaments/getTournamentById";
import { putTournament } from "@/lib/api/tournaments/putTournament";
import { useUser } from "@/lib/contexts/useUser";
import { Pen, Play, UserCheck2 } from "lucide-react";
import { getTournamentBracket } from "@/lib/api/tournaments/getTournamentBracket";
import TournamentBracket from "@/components/TournamentBracket";
import ParticipantCard from "@/components/cards/ParticipantCard";
import { addTournamentParticipant } from "@/lib/api/tournaments/addTournamentParticipant";
import TournamentParticipationForm from "@/components/forms/TournamentParticipationForm";
import { startTournament } from "@/lib/api/tournaments/startTournament";
import { putTournamentBracket } from "@/lib/api/tournaments/putTournamentBracket";
import TournamentResultsForm from "@/components/forms/TournamentResultsForm";
import type { BracketMatch } from "@/lib/api/types";

export default function Tournament() {
  const { id } = useParams();
  const { user, isLoading: isLoading } = useUser();
  const [isModalForEditOpen, setIsModalForEditOpen] = useState(false);
  const [isModalForParticipationOpen, setIsModalForParticipationOpen] =
    useState(false);
  const [isModalForResultsOpen, setIsModalForResultsOpen] = useState(false);
  const [selectedMatch, setSelectedMatch] = useState<BracketMatch | null>(null);
  const [isMatchModalOpen, setIsMatchModalOpen] = useState(false);

  const {
    data: tournament,
    isLoading: tournamentLoading,
    refetch: tournamentRefetch,
    error: tournamentError,
  } = useQuery({
    queryKey: ["tournament", id],
    queryFn: () => getTournamentById(id!),
    enabled: Boolean(id),
  });

  const {
    data: tournamentBracket,
    isLoading: tournamentBracketLoading,
    refetch: tournamentBracketRefetch,
    error: tournamentBracketError,
  } = useQuery({
    queryKey: ["tournament-bracket", id],
    queryFn: () => getTournamentBracket(id!),
    enabled: Boolean(id),
  });

  const { mutate: updateMutate, isPending: isUpdating } = useMutation({
    mutationFn: putTournament,
    onSuccess: () => {
      toast.success("Tournament updated successfully");
      tournamentRefetch();
      setIsModalForEditOpen(false);
    },
  });

  const { mutate: participate, isPending: isParticipationSubmitting } =
    useMutation({
      mutationFn: addTournamentParticipant,
      onSuccess: () => {
        toast.success(
          "Request was submitted successfully! Tournament manager will review your submission",
        );
        tournamentRefetch();
        setIsModalForParticipationOpen(false);
      },
    });

  const { mutate: start, isPending: isTournamentStarting } = useMutation({
    mutationFn: startTournament,
    onSuccess: () => {
      toast.success("Congratulations! Tournament has begun!");
      tournamentBracketRefetch();
    },
  });

  const { mutate: updateBracket, isPending: isTournamentBracketUpdating } =
    useMutation({
      mutationFn: putTournamentBracket,
      onSuccess: () => {
        toast.success("Tournament bracket updated");
        tournamentBracketRefetch();
        setIsModalForResultsOpen(false);
      },
    });

  if (
    tournamentLoading ||
    (!tournament && !tournamentError) ||
    tournamentBracketLoading ||
    (!tournamentBracket && !tournamentBracketError) ||
    isLoading
  ) {
    return <LoadingScreen />;
  }

  if (tournamentError) return <ErrorScreen message={tournamentError.message} />;
  if (tournamentBracketError)
    return <ErrorScreen message={tournamentBracketError.message} />;

  const isManager = user?.id === tournament.manager.id;
  const hasMatches = !!tournamentBracket.matches;
  const hasParticipants = (tournament.participants?.length ?? 0) > 0;
  const participantsCount = tournament.participants?.length ?? 0;
  const userParticipates =
    tournament.participants?.some((p) => p.user_id === user?.id) ?? false;

  const canSeeParticipationButton =
    !!user &&
    participantsCount < tournament.expected_members &&
    !userParticipates &&
    !hasMatches;

  const canSeeStartButton = !!user && isManager && !hasMatches;

  const canSeeBracketUpdateButton = !!user && isManager && hasMatches;

  const startButtonDisabled =
    isTournamentStarting || !hasParticipants || participantsCount % 2 !== 0;

  return (
    <main className="flex-1 bg-gray-100 pt-16">
      <section className="bg-black flex flex-col justify-center items-center text-center px-4 py-16 md:py-24">
        <div className="flex flex-col sm:flex-row items-center gap-6">
          <h1 className="text-4xl md:text-6xl text-gray-50 font-extrabold leading-tight tracking-tight">
            {tournament.name}
          </h1>
          {user && user.id === tournament.manager.id && (
            <button
              className="flex font-medium p-3 cursor-pointer text-center items-center justify-center text-gray-400 hover:text-gray-500 border-b border-dashed"
              onClick={() => setIsModalForEditOpen(true)}
            >
              <Pen className="w-6 h-6" />
              <span className="ml-4">Edit</span>
            </button>
          )}
        </div>
      </section>

      <div className="max-w-6xl mx-auto flex flex-wrap justify-center gap-12 p-8 md:p-16">
        <section className="flex-1 min-w-[300px] max-w-lg flex flex-col gap-4">
          <h2 className="heading-2">Information</h2>
          <TournamentInfoCard tournament={tournament} />
        </section>

        <section className="flex-1 min-w-[300px] max-w-lg flex flex-col gap-4">
          {tournament.manager && (
            <>
              <h2 className="heading-2">Manager</h2>
              <PlayerCard player={tournament.manager} />
            </>
          )}
          {canSeeStartButton && (
            <button
              className="btn-primary flex items-center gap-1"
              disabled={startButtonDisabled}
              onClick={() => start(tournament.id)}
            >
              <Play className="w-4 h-4" /> Start tournament
            </button>
          )}
          {canSeeBracketUpdateButton && (
            <button
              className="btn-secondary flex items-center gap-1"
              onClick={() => setIsModalForResultsOpen(true)}
            >
              <Pen className="w-4 h-4" /> Update results
            </button>
          )}
        </section>
      </div>

      <section className="max-w-6xl mx-auto flex flex-col px-8 md:px-16">
        {tournamentBracket.matches ? (
          <TournamentBracket
            matches={tournamentBracket.matches}
            onMatchClick={(match) => {
              setSelectedMatch(match);
              setIsMatchModalOpen(true);
            }}
            canSeeMatchClick={canSeeBracketUpdateButton}
          />
        ) : (
          <span className="w-full text-center py-4 text-gray-500">
            No matches yet
          </span>
        )}
      </section>

      <div className="max-w-6xl flex flex-col mx-auto gap-4 p-8 md:p-16">
        <div className="flex justify-between items-center mb-4">
          <h2 className="heading-2">Registered participants</h2>
          {canSeeParticipationButton && (
            <button
              className="btn-primary flex items-center gap-1"
              onClick={() => setIsModalForParticipationOpen(true)}
            >
              <UserCheck2 className="w-4 h-4" /> Participate in tournament
            </button>
          )}
        </div>
        {tournament.participants && tournament.participants.length > 0 ? (
          <>
            {(() => {
              const mid = Math.ceil(tournament.participants.length / 2);
              const firstHalf = tournament.participants.slice(0, mid);
              const secondHalf = tournament.participants.slice(mid);

              return (
                <section className="flex flex-wrap justify-center gap-12">
                  <article className="flex-1 min-w-[300px] max-w-lg flex flex-col gap-4">
                    <ul className="divide-y divide-gray-200 bg-white rounded-lg shadow">
                      {firstHalf.map((participant) => (
                        <li key={participant.id}>
                          <ParticipantCard participant={participant} />
                        </li>
                      ))}
                    </ul>
                  </article>

                  <article className="flex-1 min-w-[300px] max-w-lg flex flex-col gap-4">
                    <ul className="divide-y divide-gray-200 bg-white rounded-lg shadow">
                      {secondHalf.map((participant) => (
                        <li key={participant.id}>
                          <ParticipantCard participant={participant} />
                        </li>
                      ))}
                    </ul>
                  </article>
                </section>
              );
            })()}
          </>
        ) : (
          <span className="w-full text-center py-4 text-gray-500">
            No participants
          </span>
        )}
      </div>

      {user && user.id === tournament.manager.id && (
        <Modal
          open={isModalForEditOpen}
          onClose={setIsModalForEditOpen}
          title="Edit Tournament"
        >
          <TournamentForm
            initialData={{
              name: tournament.name,
              discipline: tournament.discipline,
              expected_members: tournament.expected_members,
              type: tournament.type,
              prize: tournament.prize,
              min_limit:
                tournament.type === "Team" ? tournament.min_limit : null,
              max_limit:
                tournament.type === "Team" ? tournament.max_limit : null,
            }}
            capacityEditDisabled={tournamentBracket.matches?.length !== 0}
            typeDisabled={true}
            onSubmit={(formData) =>
              updateMutate({
                id: tournament.id,
                data: { ...formData, manager_id: user.id },
              })
            }
            isSubmitting={isUpdating}
            actionName={"Save"}
          />
        </Modal>
      )}

      {canSeeParticipationButton && (
        <Modal
          open={isModalForParticipationOpen}
          onClose={setIsModalForParticipationOpen}
          title="Confirmation"
        >
          <TournamentParticipationForm
            type={tournament.type}
            userId={user.id}
            onSubmit={(formData) =>
              participate({
                id: tournament.id,
                data: formData,
              })
            }
            isSubmitting={isParticipationSubmitting}
          />
        </Modal>
      )}

      {canSeeBracketUpdateButton && (
        <Modal
          open={isMatchModalOpen}
          onClose={setIsMatchModalOpen}
          title="Update results"
        >
          {selectedMatch && (
            <TournamentResultsForm
              initialData={{
                ...tournamentBracket,
                matches: [selectedMatch],
              }}
              onSubmit={(formData) => {
                updateBracket({
                  id: tournament.id,
                  data: formData,
                });

                setIsMatchModalOpen(false);
              }}
              isSubmitting={isTournamentBracketUpdating}
            />
          )}
        </Modal>
      )}

      {canSeeBracketUpdateButton && (
        <Modal
          open={isModalForResultsOpen}
          onClose={setIsModalForResultsOpen}
          title="Update results"
        >
          <TournamentResultsForm
            initialData={tournamentBracket}
            onSubmit={(formData) =>
              updateBracket({
                id: tournament.id,
                data: formData,
              })
            }
            isSubmitting={isTournamentBracketUpdating}
          />
        </Modal>
      )}
    </main>
  );
}
