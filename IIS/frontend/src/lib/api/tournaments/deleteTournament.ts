/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Albert Tikaiev
 */
import { api } from "../client";

interface TournamentDeleteProps {
  id: number;
}

export async function deleteTournament({ id }: TournamentDeleteProps) {
  const res = await api.delete(`/tournaments/${id}`);

  return res.data;
}
