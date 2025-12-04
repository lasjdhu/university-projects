/**
 * IIS Project
 * @brief Chip for hero section
 * @author Dmitrii Ivanushkin
 */
interface StatsShipProps {
  value: number;
  desc: string;
}

export default function StatisticsChip({ value, desc }: StatsShipProps) {
  return (
    <div className="flex items-center px-4 py-2.5 bg-white rounded-lg gap-1 md:gap-2 w-48 justify-center">
      <span className="text-primary-500 text-xl md:text-2xl font-bold">
        {value}
      </span>
      <p className="font-bold">{desc}</p>
    </div>
  );
}
