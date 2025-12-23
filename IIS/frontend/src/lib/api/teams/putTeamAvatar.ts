/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Albert Tikaiev
 */
import { api } from "../client";

interface PutAvatarProps {
  file: File;
  id: string;
}

export async function putTeamAvatar({ file, id }: PutAvatarProps) {
  const res = await api.put(`/teams/${id}/avatar`, file, {
    headers: { "Content-Type": file.type },
  });

  return res.data;
}
