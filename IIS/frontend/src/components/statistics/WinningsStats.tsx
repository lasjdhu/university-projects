/**
 * IIS Project
 * @brief Card with winnings info
 * @author Albert Tikaiev
 */
import { DollarSign } from "lucide-react";

interface WinningsStatsProps {
  value: number;
}

export default function WinningsStats({ value }: WinningsStatsProps) {
  return (
    <article className="w-full card flex flex-col sm:flex-row items-center gap-8 justify-between">
      <div className="flex items-center justify-center">
        <DollarSign className="text-green-400" />
        <h2 className="text-xl font-bold">Winnings</h2>
      </div>
      <span className="text-xl font-bold">{value}$</span>
    </article>
  );
}
