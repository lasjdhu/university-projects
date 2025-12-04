/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Albert Tikaiev
 */
import { api } from "../client";
import type { TournamentStatePut } from "../types";

export async function putTournamentState(data: TournamentStatePut) {
  const res = await api.put("/admin/tournaments/state", data);

  return res.data;
}
