/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Albert Tikaiev
 */
import { api } from "../client";

interface UpdateTeamProps {
  id: number;
  data: UpdateTeamPayload;
}

interface UpdateTeamPayload {
  name: string;
  description: string;
}

export async function putTeam({ id, data }: UpdateTeamProps) {
  const res = await api.put(`/teams/${id}`, data);

  return res.data;
}
