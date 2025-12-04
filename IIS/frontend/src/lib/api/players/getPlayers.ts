/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Dmitrii Ivanushkin
 */
import { api } from "../client";
import type { PaginationResponse, PlayerResponse } from "../types";

export async function getPlayers(params: string) {
  const res = await api.get<PaginationResponse<PlayerResponse>>(
    "/players" + params,
  );

  return res.data;
}
