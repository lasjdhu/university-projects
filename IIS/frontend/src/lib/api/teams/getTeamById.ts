/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Dmitrii Ivanushkin
 */
import { api } from "../client";
import type { TeamDetailResponse } from "../types";

export async function getTeamById(id: string) {
  const res = await api.get<TeamDetailResponse>("/teams/" + id);

  return res.data;
}
