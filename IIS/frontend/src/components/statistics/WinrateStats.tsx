/**
 * IIS Project
 * @brief Winrate statistics
 * @author Albert Tikaiev, Dmitrii Ivanushkin
 */
import type { WinrateValues } from "@/lib/api/types";
import { ChevronsDown, ChevronsUp } from "lucide-react";
import { PieChart, Pie, Tooltip } from "recharts";

interface WinrateStatsProps {
  data: WinrateValues;
}

export default function WinrateStats({ data }: WinrateStatsProps) {
  return (
    <article className="w-full card flex flex-col sm:flex-row items-center gap-8">
      <div className="relative w-[200px] h-[200px]">
        <div className="absolute flex items-center justify-center w-full h-full">
          <span className="text-3xl font-bold text-gray-900">
            {data.percentage}%
          </span>
        </div>

        <PieChart width={200} height={200}>
          <Pie
            data={[
              { value: data.wins, fill: "#22c55e", name: "Wins" },
              { value: data.loses, fill: "#ef4444", name: "Losses" },
            ]}
            dataKey="value"
            cx="50%"
            cy="50%"
            innerRadius="75%"
            outerRadius="100%"
          />
          <Tooltip />
        </PieChart>
      </div>

      <div className="flex flex-col items-center sm:items-start">
        <h2 className="heading-2 mb-4 text-center sm:text-left">Winrate</h2>

        <div className="flex flex-col gap-3 text-lg">
          <div className="flex items-center gap-2">
            <ChevronsUp className="text-green-500 w-8 h-8" />
            <span>Wins ({data.wins})</span>
          </div>

          <div className="flex items-center gap-2">
            <ChevronsDown className="text-red-500 w-8 h-8" />
            <span>Losses ({data.loses})</span>
          </div>
        </div>
      </div>
    </article>
  );
}
