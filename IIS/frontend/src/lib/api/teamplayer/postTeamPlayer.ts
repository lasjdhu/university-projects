/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Albert Tikaiev
 */
import { api } from "../client";
import type { PlayerStateResponse } from "../types";

interface InvitePlayerPayload {
  email: string;
  team_id: number;
}

export async function postTeamPlayer(data: InvitePlayerPayload) {
  const res = await api.post<PlayerStateResponse>(
    `/teams/${data.team_id}/invite`,
    { email: data.email },
  );

  return res.data;
}
