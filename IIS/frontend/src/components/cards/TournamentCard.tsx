/**
 * IIS Project
 * @brief Tournament info card used in lists
 * @author Dmitrii Ivanushkin
 */
import type { TournamentResponse } from "@/lib/api/types";
import { Dice2, UserCheck } from "lucide-react";
import { Link } from "react-router";

interface TournamentCardProps {
  tournament: TournamentResponse;
  layout?: "square" | "horizontal";
  variant?: "standard" | "ghost";
}

export default function TournamentCard({
  tournament,
  layout = "square",
  variant = "standard",
}: TournamentCardProps) {
  return (
    <article
      className={`w-full flex p-4 ${
        layout === "square" ? "flex-col" : "items-center justify-between"
      } ${variant === "ghost" ? "" : "card"}`}
    >
      <div className={`flex flex-col ${layout === "square" ? "" : "ml-4"}`}>
        <h2
          className={`heading-2 mb-1 ${layout === "square" ? "text-center" : ""}`}
        >
          {tournament.name}
        </h2>
        {layout === "square" && (
          <p className="text-center text-gray-400 mb-4">
            {tournament.type.charAt(0).toUpperCase() + tournament.type.slice(1)}
          </p>
        )}

        {layout === "square" && (
          <>
            <div className="flex items-center gap-2 mb-2 mx-2">
              <Dice2 className="w-5 h-5 text-gray-400" />
              <span>{tournament.discipline}</span>
            </div>

            <div className="flex items-center gap-2 mb-4 mx-2">
              <UserCheck className="w-5 h-5 text-gray-400" />
              <span>Capacity: {tournament.expected_members}</span>
            </div>
          </>
        )}
      </div>

      <Link to={`/tournaments/${tournament.id}`} className="btn-secondary">
        View
      </Link>
    </article>
  );
}
