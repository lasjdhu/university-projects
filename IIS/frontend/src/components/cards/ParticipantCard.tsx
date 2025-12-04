/**
 * IIS Project
 * @brief Basic player or team info card
 * @author Dmitrii Ivanushkin
 */
import type { ParticipantResponse } from "@/lib/api/types";
import { Link } from "react-router";

interface ParticipantCardProps {
  participant: ParticipantResponse;
}

export default function ParticipantCard({ participant }: ParticipantCardProps) {
  return (
    <article className="w-full flex items-center justify-between p-4 h-22">
      <div className="flex items-center gap-4">
        <h2 className="ml-4 heading-2">{participant.name}</h2>
      </div>

      {participant.user_id ? (
        <Link to={`/players/${participant.user_id}`} className="btn-secondary">
          View
        </Link>
      ) : (
        <Link to={`/teams/${participant.team_id}`} className="btn-secondary">
          View
        </Link>
      )}
    </article>
  );
}
