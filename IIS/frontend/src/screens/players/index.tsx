/**
 * IIS Project
 * @brief Players list
 * @author Dmitrii Ivanushkin
 */
import { useState } from "react";
import Pagination from "@/components/Pagination";
import PlayerCard from "@/components/cards/PlayerCard";
import { keepPreviousData, useQuery } from "@tanstack/react-query";
import { getPlayers } from "@/lib/api/players/getPlayers";
import { createQueryParams } from "@/lib/utils/createQueryParams";
import LoadingScreen from "../misc/LoadingScreen";
import ErrorScreen from "../misc/ErrorScreen";
import { useDebounce } from "@/lib/utils/useDebounce";
import { ITEMS_PER_PAGE_EXTENDED } from "@/lib/config/constants";

export default function Players() {
  const [currentPage, setCurrentPage] = useState(1);
  const [search, setSearch] = useState("");
  const debouncedSearch = useDebounce(search, 400);

  const queryParams = createQueryParams({
    search: debouncedSearch,
    page: currentPage.toString(),
    limit: ITEMS_PER_PAGE_EXTENDED.toString(),
  });

  const {
    data: paginatedPlayers,
    isLoading,
    error,
  } = useQuery({
    queryFn: () => getPlayers(queryParams),
    queryKey: ["players", currentPage, debouncedSearch],
    placeholderData: keepPreviousData,
  });

  if (isLoading || (!paginatedPlayers && !error)) {
    return <LoadingScreen />;
  }

  if (error) {
    return <ErrorScreen message={error.message} />;
  }

  return (
    <main className="flex-1 bg-gray-100 px-8 pt-24">
      <div className="max-w-4xl mx-auto">
        <h1 className="heading-1 mb-4">Players</h1>
        <input
          type="text"
          value={search}
          onChange={(e) => setSearch(e.target.value)}
          placeholder="Search players..."
          className="input-field mb-4"
        />

        <ul className="divide-y divide-gray-200 bg-white rounded-tl-lg rounded-tr-lg shadow">
          {paginatedPlayers.data &&
            paginatedPlayers.data.map((player) => (
              <li key={player.id}>
                <PlayerCard player={player} variant="ghost" />
              </li>
            ))}
        </ul>

        <Pagination
          totalItems={paginatedPlayers.total_records}
          itemsPerPage={ITEMS_PER_PAGE_EXTENDED}
          currentPage={paginatedPlayers.page}
          onPageChange={setCurrentPage}
        />
      </div>
    </main>
  );
}
