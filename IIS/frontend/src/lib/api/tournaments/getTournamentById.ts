/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Dmitrii Ivanushkin
 */
import { api } from "../client";
import type { TournamentDetailResponse } from "../types";

export async function getTournamentById(id: string) {
  const res = await api.get<TournamentDetailResponse>("/tournaments/" + id);

  return res.data;
}
