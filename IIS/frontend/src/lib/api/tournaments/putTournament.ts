/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Dmitrii Ivanushkin
 */
import { api } from "../client";
import type { TournamentResponse } from "../types";
import type { TournamentFormData } from "@/lib/schemas/tournament";

interface UpdateTournamentPayload {
  id: number;
  data: TournamentFormData;
}

export async function putTournament({ id, data }: UpdateTournamentPayload) {
  const res = await api.put<TournamentResponse>(`/tournaments/${id}`, data);
  return res.data;
}
