/**
 * IIS Project
 * @brief Card with detailed info about tournament used by admin or manager
 * @author Albert Tikaiev, Dmitrii Ivanushkin
 */
import { deleteTournament } from "@/lib/api/tournaments/deleteTournament";
import { putTournamentParticipant } from "@/lib/api/tournaments/putTournamentParticipant";
import { putTournamentState } from "@/lib/api/tournaments/putTournamentState";
import type {
  ParticipantWithConflictsResponse,
  PlayerResponse,
  TournamentState,
  TournamentDetailWithState,
} from "@/lib/api/types";
import { useMutation } from "@tanstack/react-query";
import {
  BadgeDollarSign,
  Dice2,
  UserCheck,
  UserLock,
  UserPlus2,
  Wrench,
} from "lucide-react";
import toast from "react-hot-toast";
import { Link } from "react-router";

interface TournamentDetailedCardProps {
  tournament: TournamentDetailWithState;
  manager?: PlayerResponse;
  participant?: ParticipantWithConflictsResponse[];
  refetch?: () => void;
}

export default function TournamentDetailedCard({
  tournament,
  manager,
  participant,
  refetch,
}: TournamentDetailedCardProps) {
  const { mutate: stateMutate } = useMutation({
    mutationFn: putTournamentState,
    onSuccess: () => {
      refetch?.();
      toast.success("Tournament state was successfully updated");
    },
  });

  const { mutate: requestMutate } = useMutation({
    mutationFn: putTournamentParticipant,
    onSuccess: () => {
      refetch?.();
      toast.success("Tournament invite was proccesed successfully");
    },
  });

  const { mutate: deleteMutate } = useMutation({
    mutationFn: deleteTournament,
    onSuccess: () => {
      refetch?.();
      toast.success("Tournament was deleted successfully");
    },
  });

  const onSubmitState = (state: TournamentState[number]) =>
    stateMutate({ id: tournament.id, state });

  const onSubmitRequest = (id: number, result: "Accept" | "Reject") =>
    requestMutate({ id: tournament.id, data: { id, result } });

  return (
    <article className={`w-full p-4 ${manager ? "" : "card"}`}>
      <div className="flex flex-col md:flex-row gap-4 md:gap-6">
        <div className="flex flex-col gap-1 flex-1">
          <h2 className="text-lg font-semibold">{tournament.name}</h2>
          <p className="text-sm text-gray-400">
            {tournament.type.charAt(0).toUpperCase() + tournament.type.slice(1)}
          </p>
          <div className="flex flex-wrap gap-3 text-sm text-gray-500 mt-1">
            <div className="flex items-center gap-2">
              <Dice2 className="w-5 h-5 text-gray-400" />
              <span>Discipline: {tournament.discipline}</span>
            </div>
            <div className="flex items-center gap-2">
              <BadgeDollarSign className="w-5 h-5 text-gray-400" />
              <span>Prize: {tournament.prize}$</span>
            </div>

            <div className="flex items-center gap-2 ml-2">
              <UserCheck className="w-5 h-5 text-gray-400" />
              <span>Capacity: {tournament.expected_members}</span>
            </div>

            <div className="flex items-center gap-1 text-sm">
              <Wrench className="w-4 h-4 text-gray-400" />
              <span
                className={`px-2 py-0.5 rounded text-xs font-medium ${
                  tournament.state === "Accepted"
                    ? "bg-green-100 text-green-800"
                    : tournament.state === "Rejected"
                      ? "bg-red-100 text-red-800"
                      : "bg-yellow-100 text-yellow-800"
                }`}
              >
                {tournament.state}
              </span>
            </div>
            {tournament.min_limit && tournament.max_limit ? (
              <div className="flex items-center gap-2 ml-2">
                <UserPlus2 className="w-5 h-5 text-gray-400" />
                <span>
                  Player constraint:
                  {tournament.min_limit == tournament.max_limit
                    ? `${tournament.min_limit}`
                    : `${tournament.min_limit}-${tournament.max_limit}`}
                </span>
              </div>
            ) : (
              <></>
            )}
          </div>

          {manager && (
            <div className="flex flex-wrap gap-3 items-center mt-2">
              <span className="flex items-center gap-1 text-sm text-gray-700">
                <UserLock className="w-4 h-4 text-gray-400" />
                Manager:
                <Link
                  to={`/players/${manager.id}`}
                  className="link font-medium truncate"
                >
                  {manager.name} {manager.surname}
                </Link>
              </span>
            </div>
          )}

          {manager && (
            <div className="flex gap-4 mt-4">
              {(["Accepted", "Pending", "Rejected"] as TournamentState).map(
                (s) => (
                  <button
                    key={s}
                    onClick={() => onSubmitState(s)}
                    className={`flex-1 md:flex-none btn-secondary ${
                      s === "Accepted"
                        ? "border-green-500 hover:bg-green-100"
                        : s === "Rejected"
                          ? "border-red-500 hover:bg-red-100"
                          : "border-yellow-400 hover:bg-yellow-100"
                    }`}
                  >
                    {s === "Accepted"
                      ? "Accept"
                      : s === "Rejected"
                        ? "Reject"
                        : "Pend"}
                  </button>
                ),
              )}
            </div>
          )}
        </div>

        <div className="flex flex-col">
          {tournament.state === "Accepted" && (
            <Link
              to={`/tournaments/${tournament.id}`}
              className="btn-secondary"
            >
              View
            </Link>
          )}
          {!manager && tournament.state === "Rejected" ? (
            <button
              className="btn-secondary border-red-400"
              onClick={() => deleteMutate({ id: tournament.id })}
            >
              Delete
            </button>
          ) : (
            <></>
          )}
        </div>
      </div>
      {participant && (
        <>
          <h3 className="heading-3 mb-4 mt-8">Pending requests</h3>
          <ul className="divide-y divide-gray-200 bg-white rounded-lg">
            {participant.map((value) => (
              <li
                className="flex justify-between items-center py-3 rounded-lg hover:bg-gray-50"
                key={value.id}
              >
                <div className="flex flex-col ml-3">
                  <Link
                    to={
                      tournament.type === "Person"
                        ? `/players/${value.user_id}}`
                        : `/teams/${value.team_id}`
                    }
                    className="link font-medium"
                  >
                    {value.name}
                  </Link>
                  {value.conflicts && (
                    <span className="text-xs text-gray-400">
                      Teams with conflict players:{" "}
                      {value.conflicts.map((value, index, conflicts) =>
                        index == conflicts.length - 1 ? value : `${value}, `,
                      )}
                    </span>
                  )}
                </div>
                <div className="flex">
                  <button
                    className="btn-secondary border-green-400 mx-2"
                    onClick={() => onSubmitRequest(value.id, "Accept")}
                  >
                    Accept
                  </button>
                  <button
                    className="btn-secondary border-red-400 mx-2"
                    onClick={() => onSubmitRequest(value.id, "Reject")}
                  >
                    Reject
                  </button>
                </div>
              </li>
            ))}
          </ul>
        </>
      )}
    </article>
  );
}
