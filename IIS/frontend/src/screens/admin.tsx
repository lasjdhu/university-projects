/**
 * IIS Project
 * @brief Admin page
 * @author Albert Tikaiev
 */
import { useState } from "react";
import Filter from "@/components/Filter";
import { useDebounce } from "@/lib/utils/useDebounce";
import { ShieldUser, Trophy } from "lucide-react";
import { createQueryParams } from "@/lib/utils/createQueryParams";
import { ITEMS_PER_PAGE_MINIMAL } from "@/lib/config/constants";
import { keepPreviousData, useQuery } from "@tanstack/react-query";
import { getAdminUsers } from "@/lib/api/tournaments/getAdminUsers";
import LoadingScreen from "./misc/LoadingScreen";
import ErrorScreen from "./misc/ErrorScreen";
import { getAdminTournaments } from "@/lib/api/tournaments/getAdminTournaments";
import TournamentDetailedCard from "@/components/cards/TournamentDetailedCard";
import Pagination from "@/components/Pagination";
import UserAdminCard from "@/components/cards/UserAdminCard";
import { useUser } from "@/lib/contexts/useUser";
import { Navigate } from "react-router";

export default function Admin() {
  const { user, isLoading } = useUser();

  const [choice, setChoice] = useState<"Tournaments" | "Users">("Tournaments");
  const [stateFilter, setStateFilter] = useState("");
  const [currentUserPage, setCurrentUserPage] = useState(1);
  const [currentTournamentPage, setCurrentTournamentPage] = useState(1);

  const [tournamentsSearch, setTournamentsSearch] = useState("");
  const tournamentsDebouncedSearch = useDebounce(tournamentsSearch, 400);

  const [usersSearch, setUsersSearch] = useState("");
  const usersDebouncedSearch = useDebounce(usersSearch, 400);

  const states = ["Accepted", "Pending", "Rejected"];

  const changeChoice = (newChoice: "Tournaments" | "Users") => {
    if (newChoice === "Tournaments") setTournamentsSearch("");
    if (newChoice === "Users") setUsersSearch("");

    setChoice(newChoice);
  };

  const tournamentsParams = createQueryParams({
    state: stateFilter,
    search: tournamentsDebouncedSearch,
    page: currentTournamentPage.toString(),
    limit: ITEMS_PER_PAGE_MINIMAL.toString(),
  });

  const {
    data: paginatedTournaments,
    isLoading: tournamentsLoading,
    error: tournamentsError,
    refetch: refetchTournaments,
  } = useQuery({
    queryFn: () => getAdminTournaments(tournamentsParams),
    queryKey: ["tournaments", tournamentsParams],
    placeholderData: keepPreviousData,
  });

  const usersParams = createQueryParams({
    search: usersDebouncedSearch,
    page: currentUserPage.toString(),
    limit: ITEMS_PER_PAGE_MINIMAL.toString(),
  });

  const {
    data: paginatedUsers,
    isLoading: usersLoading,
    error: userError,
    refetch: refetchUsers,
  } = useQuery({
    queryFn: () => getAdminUsers(usersParams),
    queryKey: ["users", usersParams],
    placeholderData: keepPreviousData,
  });

  if (
    usersLoading ||
    (!paginatedUsers && !userError) ||
    tournamentsLoading ||
    (!paginatedTournaments && !tournamentsError) ||
    isLoading
  ) {
    return <LoadingScreen />;
  }

  if (userError) {
    return <ErrorScreen message={userError.message} />;
  }

  if (tournamentsError) {
    return <ErrorScreen message={tournamentsError.message} />;
  }

  if (!user || user.role !== "Admin") {
    return <Navigate to="/" />;
  }

  return (
    <main className="flex-1 bg-gray-100 pt-32 px-8">
      <div className="max-w-4xl mx-auto">
        <Filter
          options={[
            {
              value: "Tournaments",
              label: "Tournaments",
              icon: <Trophy size={16} />,
            },
            { value: "Users", label: "Users", icon: <ShieldUser size={16} /> },
          ]}
          chosen={choice}
          setter={changeChoice}
        />

        <input
          type="text"
          value={choice === "Tournaments" ? tournamentsSearch : usersSearch}
          onChange={(e) =>
            choice === "Tournaments"
              ? setTournamentsSearch(e.target.value)
              : setUsersSearch(e.target.value)
          }
          placeholder={`Search ${choice.toLowerCase()}...`}
          className="input-field mb-4"
        />

        {choice === "Tournaments" ? (
          <>
            <Filter
              options={states.map((item) => ({
                value: item,
                label: item,
              }))}
              chosen={stateFilter}
              setter={setStateFilter}
            />
            <div className="w-full flex flex-col">
              <ul className="divide-y divide-gray-200 bg-white rounded-tl-lg rounded-tr-lg shadow">
                {paginatedTournaments.data &&
                  paginatedTournaments.data.map((dtournament) => (
                    <li key={dtournament.id}>
                      <TournamentDetailedCard
                        tournament={dtournament}
                        manager={dtournament.manager}
                        refetch={refetchTournaments}
                      />
                    </li>
                  ))}
              </ul>

              {paginatedTournaments.total_pages > 0 ? (
                <Pagination
                  totalItems={paginatedTournaments.total_records}
                  itemsPerPage={ITEMS_PER_PAGE_MINIMAL}
                  currentPage={paginatedTournaments.page}
                  onPageChange={setCurrentTournamentPage}
                />
              ) : (
                <span className="w-full text-center py-4 text-gray-500">
                  No tournaments
                </span>
              )}
            </div>
          </>
        ) : (
          <div className="w-full flex flex-col">
            <ul className="divide-y divide-gray-200 bg-white rounded-tl-lg rounded-tr-lg shadow">
              {paginatedUsers.data &&
                paginatedUsers.data.map((user) => (
                  <li key={user.id}>
                    <UserAdminCard user={user} refetch={refetchUsers} />
                  </li>
                ))}
            </ul>

            {paginatedUsers.total_pages > 0 ? (
              <Pagination
                totalItems={paginatedUsers.total_records}
                itemsPerPage={ITEMS_PER_PAGE_MINIMAL}
                currentPage={paginatedUsers.page}
                onPageChange={setCurrentUserPage}
              />
            ) : (
              <span className="w-full text-center py-4 text-gray-500">
                No users
              </span>
            )}
          </div>
        )}
      </div>
    </main>
  );
}
