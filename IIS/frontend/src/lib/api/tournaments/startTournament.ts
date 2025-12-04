/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Dmitrii Ivanushkin
 */
import { api } from "../client";
import type { TournamentResponse } from "../types";

export async function startTournament(id: number) {
  const res = await api.post<TournamentResponse>(`/tournaments/${id}/start`);

  return res.data;
}
