/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Albert Tikaiev
 */
import { api } from "../client";
import type {
  PaginationResponse,
  TournamentAdminDetailResponse,
} from "../types";

export async function getAdminTournaments(params: string) {
  const res = await api.get<PaginationResponse<TournamentAdminDetailResponse>>(
    "/admin/tournaments" + params,
  );

  return res.data;
}
