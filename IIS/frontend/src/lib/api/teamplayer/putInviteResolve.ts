/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Albert Tikaiev
 */
import type { ResolveInviteFormData } from "@/lib/schemas/resolveInvite";
import { api } from "../client";

export async function putInviteResolve(data: ResolveInviteFormData) {
  const res = await api.put(`/teams/${data.team_id}/invite`, {
    result: data.result,
  });

  return res.data;
}
