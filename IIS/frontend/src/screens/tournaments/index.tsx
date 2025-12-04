/**
 * IIS Project
 * @brief Tournaments list
 * @author Dmitrii Ivanushkin
 */
import { useState } from "react";
import { keepPreviousData, useMutation, useQuery } from "@tanstack/react-query";
import LoadingScreen from "../misc/LoadingScreen";
import { type TournamentFormData } from "@/lib/schemas/tournament";
import { postTournament } from "@/lib/api/tournaments/postTournament";
import toast from "react-hot-toast";
import Modal from "@/components/Modal";
import { useUser } from "@/lib/contexts/useUser";
import TournamentForm from "@/components/forms/TournamentForm";
import { Plus } from "lucide-react";
import { useDebounce } from "@/lib/utils/useDebounce";
import { queryClient } from "@/lib/config/queryClient";
import { ITEMS_PER_PAGE_EXTENDED } from "@/lib/config/constants";
import { createQueryParams } from "@/lib/utils/createQueryParams";
import { getTournaments } from "@/lib/api/tournaments/getTournaments";
import ErrorScreen from "../misc/ErrorScreen";
import TournamentCard from "@/components/cards/TournamentCard";
import Pagination from "@/components/Pagination";

export default function Tournaments() {
  const { user, isLoading } = useUser();
  const [isModalOpen, setIsModalOpen] = useState(false);
  const [search, setSearch] = useState("");
  const debouncedSearch = useDebounce(search, 400);
  const [currentPage, setCurrentPage] = useState(1);

  const params = createQueryParams({
    search: debouncedSearch,
    page: currentPage.toString(),
    limit: ITEMS_PER_PAGE_EXTENDED.toString(),
  });

  const {
    data: paginatedTournaments,
    isLoading: tournamentsLoading,
    error,
  } = useQuery({
    queryFn: () => getTournaments(params),
    queryKey: ["tournaments", params],
    placeholderData: keepPreviousData,
  });

  const { mutate: createMutate, isPending: isSubmitting } = useMutation({
    mutationFn: postTournament,
    onSuccess: () => {
      toast.success(
        "Tournament submitted successfully! Admin will review your submission",
      );
      queryClient.invalidateQueries({ queryKey: ["tournaments"] });
      setIsModalOpen(false);
    },
  });

  if (tournamentsLoading || (!paginatedTournaments && !error) || isLoading) {
    return <LoadingScreen />;
  }

  if (error) {
    return <ErrorScreen message={error.message} />;
  }

  return (
    <main className="flex-1 bg-gray-100 px-8 pt-20">
      <div className="max-w-4xl mx-auto">
        <div className="flex justify-between items-center mb-4">
          <h1 className="heading-1">Tournaments</h1>
          {user && (
            <button
              className="btn-primary flex items-center gap-1"
              onClick={() => {
                setIsModalOpen(true);
              }}
            >
              <Plus className="w-4 h-4" /> Create
            </button>
          )}
        </div>

        <input
          type="text"
          value={search}
          onChange={(e) => setSearch(e.target.value)}
          placeholder="Search tournaments..."
          className="input-field mb-4"
        />

        <div className="w-full flex flex-col">
          <ul className="divide-y divide-gray-200 bg-white rounded-tl-lg rounded-tr-lg shadow">
            {paginatedTournaments.data &&
              paginatedTournaments.data.map((tournament) => (
                <li key={tournament.id}>
                  <TournamentCard
                    tournament={tournament}
                    variant="ghost"
                    layout="horizontal"
                  />
                </li>
              ))}
          </ul>

          {paginatedTournaments.total_pages > 0 ? (
            <Pagination
              totalItems={paginatedTournaments.total_records}
              itemsPerPage={ITEMS_PER_PAGE_EXTENDED}
              currentPage={paginatedTournaments.page}
              onPageChange={setCurrentPage}
            />
          ) : (
            <span className="w-full text-center py-4 text-gray-500">
              No tournaments
            </span>
          )}
        </div>
      </div>

      {user && (
        <Modal
          open={isModalOpen}
          onClose={setIsModalOpen}
          title="New tournament"
        >
          <TournamentForm
            initialData={{
              name: "",
              discipline: "",
              expected_members: 2,
              type: "Person",
              prize: 0,
              min_limit: null,
              max_limit: null,
            }}
            onSubmit={(data) => {
              if (!user) return toast.error("User not loaded");
              const payload: TournamentFormData = {
                ...data,
                prize: data.prize ?? 0,
                max_limit: data.max_limit ?? null,
                min_limit: data.min_limit ?? null,
                manager_id: user.id,
              };
              createMutate(payload);
            }}
            isSubmitting={isSubmitting}
            actionName={"Create"}
          />
        </Modal>
      )}
    </main>
  );
}
