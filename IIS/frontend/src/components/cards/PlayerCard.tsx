/**
 * IIS Project
 * @brief Basic player info card
 * @author Dmitrii Ivanushkin
 */
import type { PlayerResponse } from "@/lib/api/types";
import { UserStar } from "lucide-react";
import { Link } from "react-router";

interface PlayerCardProps {
  player: PlayerResponse;
  variant?: "standard" | "ghost";
  isManager?: boolean;
}

export default function PlayerCard({
  player,
  variant = "standard",
  isManager,
}: PlayerCardProps) {
  return (
    <article
      className={`w-full flex items-center justify-between p-4 h-22 ${variant === "ghost" ? "" : "card"}`}
    >
      <div className="flex items-center gap-4">
        <h2 className="ml-4 heading-2">
          {player.name} {player.surname}
        </h2>
        {isManager && (
          <span className="flex items-center gap-1 px-2 py-0.5 rounded text-xs font-medium bg-green-100 text-green-800">
            <UserStar className="w-4 h-4 text-green-800" />
            Manager
          </span>
        )}
      </div>

      <Link to={`/players/${player.id}`} className="btn-secondary">
        View
      </Link>
    </article>
  );
}
