/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Dmitrii Ivanushkin
 */
import { api } from "../client";
import type { TournamentResponse } from "../types";
import type { TournamentFormData } from "@/lib/schemas/tournament";

export async function postTournament(data: TournamentFormData) {
  const res = await api.post<TournamentResponse>("/tournaments", data);

  return res.data;
}
