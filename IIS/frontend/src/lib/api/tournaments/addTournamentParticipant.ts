/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Dmitrii Ivanushkin
 */
import { api } from "../client";
import type { TournamentResponse } from "../types";
import type { TournamentParticipantFormData } from "@/lib/schemas/tournamentParticipant";

interface TournamentParticipantPayload {
  id: number;
  data: TournamentParticipantFormData;
}

export async function addTournamentParticipant({
  id,
  data,
}: TournamentParticipantPayload) {
  const res = await api.post<TournamentResponse>(
    `/tournaments/${id}/participants`,
    data,
  );

  return res.data;
}
