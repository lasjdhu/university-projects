/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Dmitrii Ivanushkin
 */
import { api } from "../client";
import type { PaginationResponse, TournamentResponse } from "../types";

export async function getTournaments(params: string) {
  const res = await api.get<PaginationResponse<TournamentResponse>>(
    "/tournaments" + params,
  );

  return res.data;
}
