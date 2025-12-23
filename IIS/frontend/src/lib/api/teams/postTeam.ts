/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Albert Tikaiev
 */
import { api } from "../client";
import type { TeamResponse } from "../types";
import type { TeamFormData } from "@/lib/schemas/team";

export async function postTeam(data: TeamFormData) {
  const res = await api.post<TeamResponse>("/teams", data);

  return res.data;
}
