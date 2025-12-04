/**
 * IIS Project
 * @brief Hero section with heading and stats
 * @author Dmitrii Ivanushkin
 */
import StatisticsChip from "./statistics/StatisticsChip";

interface HeroProps {
  tournamentsCount: number;
  teamsCount: number;
  playersCount: number;
}

export default function Hero({
  tournamentsCount,
  teamsCount,
  playersCount,
}: HeroProps) {
  return (
    <section className="bg-black flex flex-col justify-center items-center text-center px-4 py-16 md:py-24">
      <h1 className="text-4xl md:text-6xl text-gray-50 font-extrabold leading-tight tracking-tight">
        Student Tournament
        <br />
        Management System
      </h1>

      <p className="text-gray-500 mt-4 text-xl md:text-2xl max-w-2xl">
        Create and manage tournaments, teams, and players
      </p>

      <div className="flex flex-col md:flex-row gap-4 mt-8">
        <StatisticsChip value={tournamentsCount} desc="Tournaments" />
        <StatisticsChip value={teamsCount} desc="Teams" />
        <StatisticsChip value={playersCount} desc="Players" />
      </div>
    </section>
  );
}
