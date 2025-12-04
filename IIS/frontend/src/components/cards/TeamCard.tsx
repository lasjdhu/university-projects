/**
 * IIS Project
 * @brief Basic team info card
 * @author Dmitrii Ivanushkin
 */
import type { TeamResponse } from "@/lib/api/types";
import { Link } from "react-router";
import Avatar from "../Avatar";

interface TeamCardProps {
  team: TeamResponse;
  variant?: "standard" | "ghost";
}

export default function TeamCard({
  team,
  variant = "standard",
}: TeamCardProps) {
  return (
    <article
      className={`w-full flex items-center justify-between p-4 h-22 ${variant === "ghost" ? "" : "card"}`}
    >
      <div className="flex items-center gap-4 ml-4">
        <Avatar image={team.image} name={team.name} width={50} height={50} />
        <h2 className="heading-2">{team.name}</h2>
      </div>

      <Link to={`/teams/${team.id}`} className="btn-secondary">
        View
      </Link>
    </article>
  );
}
