/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Dmitrii Ivanushkin
 */
import { api } from "../client";
import type { TournamentBracketResponse } from "../types";

export async function getTournamentBracket(id: string) {
  const res = await api.get<TournamentBracketResponse>(
    `/tournaments/${id}/bracket`,
  );

  return res.data;
}
