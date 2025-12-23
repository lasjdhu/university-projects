/**
 * IIS Project
 * @brief Teams list
 * @author Dmitrii Ivanushkin
 */
import { useState } from "react";
import { keepPreviousData, useMutation, useQuery } from "@tanstack/react-query";
import LoadingScreen from "../misc/LoadingScreen";
import { useDebounce } from "@/lib/utils/useDebounce";
import { useUser } from "@/lib/contexts/useUser";
import Modal from "@/components/Modal";
import TeamForm from "@/components/forms/TeamForm";
import { Plus } from "lucide-react";
import { postTeam } from "@/lib/api/teams/postTeam";
import toast from "react-hot-toast";
import type { TeamFormData } from "@/lib/schemas/team";
import { queryClient } from "@/lib/config/queryClient";
import { ITEMS_PER_PAGE_EXTENDED } from "@/lib/config/constants";
import { createQueryParams } from "@/lib/utils/createQueryParams";
import ErrorScreen from "../misc/ErrorScreen";
import { getTeams } from "@/lib/api/teams/getTeams";
import TeamCard from "@/components/cards/TeamCard";
import Pagination from "@/components/Pagination";

export default function Teams() {
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
    data: paginatedTeams,
    isLoading: teamLoading,
    error,
  } = useQuery({
    queryFn: () => getTeams(params),
    queryKey: ["teams", params],
    placeholderData: keepPreviousData,
  });

  const { mutate: createMutate, isPending: isSubmitting } = useMutation({
    mutationFn: postTeam,
    onSuccess: () => {
      toast.success(
        "Team created successfully! Wait until players accept their invites",
      );
      queryClient.invalidateQueries({ queryKey: ["teams"] });
      setIsModalOpen(false);
    },
  });

  if (teamLoading || (!paginatedTeams && !error) || isLoading) {
    return <LoadingScreen />;
  }

  if (error) {
    return <ErrorScreen message={error.message} />;
  }

  return (
    <main className="flex-1 bg-gray-100 pt-20 2xl:pt-32 px-8">
      <div className="max-w-4xl mx-auto">
        <div className="flex justify-between items-center mb-4">
          <h1 className="heading-1">Teams</h1>
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
          placeholder="Search teams..."
          className="input-field mb-4"
        />

        <div className="w-full flex flex-col">
          <ul className="divide-y divide-gray-200 bg-white rounded-tl-lg rounded-tr-lg shadow">
            {paginatedTeams.data &&
              paginatedTeams.data.map((team) => (
                <li key={team.id}>
                  <TeamCard team={team} variant="ghost" />
                </li>
              ))}
          </ul>

          {paginatedTeams.total_pages > 0 ? (
            <Pagination
              totalItems={paginatedTeams.total_records}
              itemsPerPage={ITEMS_PER_PAGE_EXTENDED}
              currentPage={paginatedTeams.page}
              onPageChange={setCurrentPage}
            />
          ) : (
            <span className="w-full text-center py-4 text-gray-500">
              No teams
            </span>
          )}
        </div>
      </div>
      {user && (
        <Modal open={isModalOpen} onClose={setIsModalOpen} title="New team">
          <TeamForm
            initialData={{
              name: "",
              description: "",
              email_invites: [],
            }}
            onSubmit={(data) => {
              if (!user) return toast.error("User not loaded");

              const payload: TeamFormData = {
                ...data,
                manager_id: user.id,
              };
              createMutate(payload);
            }}
            isSubmitting={isSubmitting}
            playersInvite={true}
            actionName={"Create"}
          />
        </Modal>
      )}
    </main>
  );
}
