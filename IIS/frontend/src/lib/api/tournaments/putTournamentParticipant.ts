/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Albert Tikaiev
 */
import { api } from "../client";

interface PutTournamentParticipantProps {
  id: number;
  data: PutTournamentParticipantPayload;
}

interface PutTournamentParticipantPayload {
  id: number;
  result: "Accept" | "Reject";
}

export async function putTournamentParticipant({
  id,
  data,
}: PutTournamentParticipantProps) {
  const res = await api.put(
    `/tournaments/${id}/participants
`,
    data,
  );

  return res.data;
}
