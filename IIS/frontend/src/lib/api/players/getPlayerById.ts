/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Dmitrii Ivanushkin
 */
import { api } from "../client";
import type { PlayerDetailResponse } from "../types";

export async function getPlayerById(id: string) {
  const res = await api.get<PlayerDetailResponse>("/players/" + id);

  return res.data;
}
