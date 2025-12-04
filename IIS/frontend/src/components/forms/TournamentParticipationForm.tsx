/**
 * IIS Project
 * @brief Form for registering for a tournament
 * @author Dmitrii Ivanushkin
 */
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import type { TournamentType } from "@/lib/api/types";
import {
  tournamentParticipantSchema,
  type TournamentParticipantFormData,
} from "@/lib/schemas/tournamentParticipant";
import { createQueryParams } from "@/lib/utils/createQueryParams";
import { useQuery } from "@tanstack/react-query";
import { getProfileDetails } from "@/lib/api/users/getProfileDetails";

interface TournamentParticipationFormProps {
  type: TournamentType[number];
  userId: number;
  onSubmit: (data: TournamentParticipantFormData) => void;
  isSubmitting?: boolean;
}

export default function TournamentParticipationForm({
  type,
  userId,
  onSubmit,
  isSubmitting,
}: TournamentParticipationFormProps) {
  const {
    handleSubmit,
    setValue,
    formState: { errors },
  } = useForm<TournamentParticipantFormData>({
    defaultValues: {
      player_id: type === "Person" ? userId : null,
      team_id: null,
    },
    resolver: zodResolver(tournamentParticipantSchema),
  });

  const queryParams = createQueryParams({
    detail: "teams",
  });

  const { data: details } = useQuery({
    queryFn: () => getProfileDetails(queryParams),
    queryKey: ["profile-details", queryParams],
  });

  const teams = details?.managing ?? [];

  return (
    <form className="space-y-4" onSubmit={handleSubmit(onSubmit)}>
      {type === "Person" ? (
        <div className="flex-1">
          <p className="block mb-2 text-sm font-medium text-gray-900">
            Please confirm by clicking on a button
          </p>
        </div>
      ) : (
        <div className="flex-1">
          <label className="block mb-2 text-sm font-medium text-gray-900">
            Please select a team
          </label>

          {teams.length === 0 ? (
            <p className="text-gray-600 text-sm">
              You are not managing any teams.
            </p>
          ) : (
            <div className="flex flex-col card gap-4">
              {teams.map((team) => (
                <label
                  key={team.id}
                  className="flex items-center gap-3 cursor-pointer"
                >
                  <input
                    type="radio"
                    name="team_id"
                    value={team.id}
                    className="h-4 w-4 text-primary-500"
                    onChange={() => setValue("team_id", team.id)}
                  />
                  <span className="text-sm text-gray-900">{team.name}</span>
                </label>
              ))}
            </div>
          )}

          {errors.team_id && (
            <p className="text-red-500 text-sm mt-2">
              {errors.team_id.message}
            </p>
          )}
        </div>
      )}

      <button
        type="submit"
        className="w-full btn-primary mt-8"
        disabled={isSubmitting}
      >
        Participate
      </button>
    </form>
  );
}
