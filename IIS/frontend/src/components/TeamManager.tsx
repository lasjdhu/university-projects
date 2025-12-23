/**
 * IIS Project
 * @brief Invite flow for manager
 * @author Albert Tikaiev
 */
import type { TeamAndPlayers } from "@/lib/api/types";
import { Link } from "react-router";
import Avatar from "./Avatar";
import { UserStar } from "lucide-react";
import { useState } from "react";
import { useMutation, useQuery } from "@tanstack/react-query";
import { putTeamPlayerState } from "@/lib/api/teamplayer/putTeamPlayerState";
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import { postTeamPlayer } from "@/lib/api/teamplayer/postTeamPlayer";
import { createQueryParams } from "@/lib/utils/createQueryParams";
import { useDebounce } from "@/lib/utils/useDebounce";
import { getUserSearchEmail } from "@/lib/api/users/getUserSearchEmail";
import {
  invitePlayerSchema,
  type InvitePlayerFormData,
} from "@/lib/schemas/invitePlayer";
import EmailSearchInput from "./EmailSearchInput";

interface TeamCardProps {
  initialTeam: TeamAndPlayers;
  withoutHeader?: boolean;
}

const StateTextColorsMap = {
  Active: "text-green-500",
  Inactive: "text-gray-500",
  Invited: "text-yellow-500",
};

const StateButtonColorsMap = {
  Active: "bg-red-600 hover:bg-red-700",
  Inactive: "bg-green-600 hover:bg-green-700",
  Invited: "bg-gray-600 hover:bg-gray-700",
};

const StateButtonActionMap = {
  Active: "Deactivate",
  Inactive: "Activate",
  Invited: "Cancel",
};

export default function TeamManager({
  initialTeam,
  withoutHeader,
}: TeamCardProps) {
  const [team, setTeam] = useState(initialTeam);
  const [search, setSearch] = useState("");
  const debouncedSearch = useDebounce(search, 400);

  const queryParams = createQueryParams({ search: debouncedSearch });

  const {
    handleSubmit,
    setValue,
    formState: { errors, isSubmitting },
  } = useForm<InvitePlayerFormData>({
    resolver: zodResolver(invitePlayerSchema),
  });

  const { data: userEmails } = useQuery({
    queryKey: ["users", debouncedSearch],
    queryFn: () => getUserSearchEmail(queryParams),
    initialData: [],
  });

  const { mutate: playerStateMutate } = useMutation({
    mutationFn: putTeamPlayerState,
    onSuccess: (data) => setTeam(data),
  });

  const { mutate: playerAddMutate } = useMutation({
    mutationFn: postTeamPlayer,
    onSuccess: (data) =>
      setTeam((prev) => ({
        ...prev,
        players: prev.players ? [...prev.players, data] : [data],
      })),
  });

  const onSubmit = (data: InvitePlayerFormData) =>
    playerAddMutate({ email: data.email, team_id: team.id });

  return (
    <article className="divide-y divide-gray-200 bg-white rounded-lg shadow mb-4">
      {!withoutHeader && (
        <div className="flex items-center justify-between py-8 px-12">
          <div className="flex items-center gap-4 ml-4">
            <Avatar
              image={team.image}
              name={team.name}
              width={50}
              height={50}
            />
            <h2 className="heading-2">{team.name}</h2>
          </div>

          <Link to={`/teams/${team.id}`} className="btn-secondary">
            View
          </Link>
        </div>
      )}

      <div className="px-12 py-4 border-b border-gray-200">
        <form
          onSubmit={handleSubmit(onSubmit)}
          className="flex flex-col sm:flex-row items-center gap-3"
        >
          <div className="flex-1 w-full">
            <EmailSearchInput
              value={search}
              onChange={(v) => {
                setSearch(v);
                setValue("email", v);
              }}
              suggestions={userEmails}
              error={errors.email?.message}
            />
          </div>

          <button
            type="submit"
            className="btn-primary whitespace-nowrap"
            disabled={isSubmitting}
          >
            Invite
          </button>
        </form>
        {team.confilct_tournaments && team.confilct_tournaments.length > 0 ? (
          <span className="text-red-400">
            By deactivating some player your invite will be deleted from
            following tournaments:{" "}
            {team.confilct_tournaments.map((value, index, conflicts) =>
              index == conflicts.length - 1 ? value : `${value}, `,
            )}
          </span>
        ) : (
          <></>
        )}
      </div>

      <ul className="divide-y divide-gray-200 bg-white rounded-b-lg">
        {team.players &&
          team.players.map((player) => {
            const color = StateTextColorsMap[player.state];
            return (
              <li
                key={player.player_id}
                className="flex justify-between items-center px-12 py-3 hover:bg-gray-50"
              >
                <div className="flex items-center gap-3 truncate">
                  <span className={`${color} font-medium`}>{player.state}</span>
                  <UserStar className={`${color} w-6 h-6`} />
                  <Link
                    to={`/players/${player.id}`}
                    className="link font-medium truncate"
                  >
                    {player.name} {player.surname}
                  </Link>
                </div>

                <button
                  className={`btn-primary ${StateButtonColorsMap[player.state]}`}
                  onClick={() =>
                    playerStateMutate({
                      team_id: initialTeam.id,
                      player_id: player.player_id,
                      state: player.state,
                    })
                  }
                >
                  {StateButtonActionMap[player.state]}
                </button>
              </li>
            );
          })}
      </ul>
    </article>
  );
}
