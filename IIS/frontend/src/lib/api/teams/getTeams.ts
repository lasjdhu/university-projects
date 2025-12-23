/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Dmitrii Ivanushkin
 */
import { api } from "../client";
import type { PaginationResponse, TeamResponse } from "../types";

export async function getTeams(params: string) {
  const res = await api.get<PaginationResponse<TeamResponse>>(
    "/teams" + params,
  );

  return res.data;
}
