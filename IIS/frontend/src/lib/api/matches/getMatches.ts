/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Albert Tikaiev
 */
import { api } from "../client";
import type { MatchDetailed, PaginationResponse } from "../types";

export async function getMatches(params: string) {
  const res = await api.get<PaginationResponse<MatchDetailed>>(
    "/matches" + params,
  );

  return res.data;
}
