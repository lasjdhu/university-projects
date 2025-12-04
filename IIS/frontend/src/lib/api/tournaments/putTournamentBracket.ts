/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Dmitrii Ivanushkin
 */
import { api } from "../client";
import type { TournamentBracketResponse } from "../types";

interface UpdateTournamentBracketPayload {
  id: number;
  data: TournamentBracketResponse;
}

export async function putTournamentBracket({
  id,
  data,
}: UpdateTournamentBracketPayload) {
  const res = await api.put(`/tournaments/${id}/bracket`, data);

  return res.data;
}
