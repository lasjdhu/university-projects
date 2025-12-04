/**
 * IIS Project
 * @brief Discipline statistics
 * @author Albert Tikaiev, Dmitrii Ivanushkin
 */
import type { DisciplineValues } from "@/lib/api/types";
import { PieChart, Pie, Tooltip } from "recharts";
import randomColor from "randomcolor";

interface DisciplineStatsProps {
  data: DisciplineValues[];
}

export default function DisciplineStats({ data }: DisciplineStatsProps) {
  const disciplineData = data.map((d) => ({
    value: d.tournaments,
    name: d.name,
    fill: randomColor({ luminosity: "bright" }),
  }));

  return (
    <article className="w-full card flex flex-col">
      {data.length > 0 ? (
        <div className="flex flex-col sm:flex-row items-center gap-8">
          <PieChart width={200} height={200}>
            <Pie data={disciplineData} dataKey="value" cx="50%" cy="50%" />
            <Tooltip />
          </PieChart>

          <div className="flex flex-col w-full text-center sm:text-left">
            <h2 className="heading-2 mb-3">Favorite Disciplines</h2>

            <ul className="flex flex-col gap-3 text-lg">
              {disciplineData.map((d) => (
                <li key={d.name} className="flex items-center gap-3">
                  <div
                    className="w-4 h-4 rounded"
                    style={{ backgroundColor: d.fill }}
                  />
                  <span>
                    {d.name} ({d.value})
                  </span>
                </li>
              ))}
            </ul>
          </div>
        </div>
      ) : (
        <div className="flex flex-col items-center py-6">
          <h2 className="heading-2 mb-2">Favorite Disciplines</h2>
          <span className="text-gray-700">
            Player did not participate in any tournament
          </span>
        </div>
      )}
    </article>
  );
}
