/**
 * IIS Project
 * @brief Tournament info card used in details
 * @author Dmitrii Ivanushkin
 */
import type { TournamentDetailResponse } from "@/lib/api/types";
import {
  BadgeDollarSign,
  Dice2,
  ShieldHalf,
  UserCheck,
  UserPlus2,
} from "lucide-react";

interface TournamentInfoCardProps {
  tournament: TournamentDetailResponse;
}

export default function TournamentInfoCard({
  tournament,
}: TournamentInfoCardProps) {
  return (
    <article className="w-full card flex flex-col p-4">
      <div className="flex items-center justify-between gap-2 mb-2 mx-2">
        <div className="flex gap-2 items-center">
          <ShieldHalf className="w-5 h-5 text-gray-500" />
          <span>Type</span>
        </div>
        <span className="font-bold text-lg">{tournament.type}</span>
      </div>
      <div className="flex items-center justify-between gap-2 mb-2 mx-2">
        <div className="flex gap-2 items-center">
          <Dice2 className="w-5 h-5 text-red-500" />
          <span>Discipline</span>
        </div>
        <span className="font-bold text-lg">{tournament.discipline}</span>
      </div>
      <div className="flex items-center justify-between gap-2 mb-2 mx-2">
        <div className="flex gap-2 items-center">
          <UserCheck className="w-5 h-5 text-green-500" />
          <span>Capacity</span>
        </div>
        <span className="font-bold text-lg">
          {tournament.participants ? `${tournament.participants.length}` : "0"}{" "}
          / {tournament.expected_members}
        </span>
      </div>
      <div className="flex items-center justify-between gap-2 mx-2">
        <div className="flex gap-2 items-center">
          <BadgeDollarSign className="w-5 h-5 text-yellow-500" />
          <span>Prize</span>
        </div>
        <span className="font-bold text-lg">{tournament.prize}$</span>
      </div>
      {tournament.max_limit && tournament.min_limit ? (
        <div className="flex items-center justify-between gap-2 mt-2 mx-2">
          <div className="flex gap-2 items-center">
            <UserPlus2 className="w-5 h-5 text-blue-500" />
            <span>Player constraint</span>
          </div>
          <span className="font-bold text-lg">
            {tournament.min_limit == tournament.max_limit
              ? `${tournament.min_limit}`
              : `${tournament.min_limit}-${tournament.max_limit}`}
          </span>
        </div>
      ) : (
        <></>
      )}
    </article>
  );
}
