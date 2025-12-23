/**
 * IIS Project
 * @brief Activity statistics
 * @author Albert Tikaiev
 */
import type { ActivityValues } from "@/lib/api/types";
import { LineChart, CartesianGrid, XAxis, Line, YAxis, Legend } from "recharts";

interface ActivityStatsProps {
  data: ActivityValues[];
  personal: boolean;
}

export default function ActivityStats({ data, personal }: ActivityStatsProps) {
  return (
    <article className="w-full card flex flex-col">
      <h2 className="heading-2 text-center mb-4">Activity</h2>

      <div className="w-full flex justify-center overflow-x-auto">
        <LineChart height={300} width={380} data={data}>
          <XAxis dataKey="month" />
          <YAxis />
          <CartesianGrid strokeDasharray="3 3" />
          <Legend />
          <Line
            dataKey="teams"
            name="Team matches"
            stroke="#00a9e0"
            strokeWidth={3}
            type="monotone"
          />
          {personal && (
            <Line
              dataKey="personal"
              name="Personal matches"
              stroke="#10b981"
              strokeWidth={3}
              type="monotone"
            />
          )}
        </LineChart>
      </div>
    </article>
  );
}
