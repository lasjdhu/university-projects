/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Albert Tikaiev
 */
import type { TeamPlayerStateFormData } from "@/lib/schemas/teamPlayerState";
import { api } from "../client";
import type { TeamAndPlayers } from "../types";

export async function putTeamPlayerState(data: TeamPlayerStateFormData) {
  const res = await api.put<TeamAndPlayers>(
    `/teams/${data.team_id}/players/${data.player_id}/state`,
    { state: data.state },
  );

  return res.data;
}
