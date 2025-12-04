/**
 * IIS Project
 * @brief Profile: basic info, created tournaments, invites and managed teams
 * @author Dmitrii Ivanushkin, Albert Tikaiev
 */
import { useState } from "react";
import { useQuery, useMutation } from "@tanstack/react-query";
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import toast from "react-hot-toast";
import { Cog, Shield, TicketPlus, Trophy } from "lucide-react";

import LoadingScreen from "./misc/LoadingScreen";
import { profileSchema, type ProfileFormData } from "@/lib/schemas/profile";
import Modal from "@/components/Modal";
import { getProfileDetails } from "@/lib/api/users/getProfileDetails";
import { createQueryParams } from "@/lib/utils/createQueryParams";
import TeamManager from "@/components/TeamManager";
import Filter from "@/components/Filter";
import { useDebounce } from "@/lib/utils/useDebounce";
import TeamInvite from "@/components/cards/TeamInviteCard";
import TournamentDetailedCard from "@/components/cards/TournamentDetailedCard";
import { getProfile } from "@/lib/api/users/getProfile";
import { putProfile } from "@/lib/api/users/putProfile";
import { useUser } from "@/lib/contexts/useUser";
import { Navigate } from "react-router";
import Spinner from "@/components/Spinner";

export default function Profile() {
  const { user, isLoading } = useUser();

  const [isModalOpen, setIsModalOpen] = useState(false);
  const [menuOption, setMenuOption] = useState("settings");

  const [teamsSearch, setTeamsSearch] = useState("");
  const teamsDebouncedSearch = useDebounce(teamsSearch, 400);

  const [tournamentsSearch, setTournamentsSearch] = useState("");
  const tournamentsDebouncedSearch = useDebounce(tournamentsSearch, 400);

  const menuItems = [
    { key: "settings", label: "Settings", icon: <Cog size={16} /> },
    { key: "invites", label: "Invites", icon: <TicketPlus size={16} /> },
    { key: "teams", label: "My teams", icon: <Shield size={16} /> },
    { key: "tournaments", label: "My tournaments", icon: <Trophy size={16} /> },
  ];

  const searchTerm =
    menuOption === "teams"
      ? teamsDebouncedSearch
      : menuOption === "tournaments"
        ? tournamentsDebouncedSearch
        : "";

  const queryParams = createQueryParams({
    detail: menuOption,
    search: searchTerm,
  });

  const {
    data: userProfile,
    isLoading: userProfileLoading,
    refetch,
  } = useQuery({
    queryFn: getProfile,
    queryKey: ["profile", menuOption],
  });

  const {
    data: details,
    isLoading: detailsLoading,
    refetch: refecthDetails,
  } = useQuery({
    queryFn: () => getProfileDetails(queryParams),
    queryKey: ["profile-details", queryParams, menuOption],
  });

  const { mutate: updateMutate } = useMutation({
    mutationFn: putProfile,
    onSuccess: () => {
      toast.success("Profile updated successfully!");
      refetch();
      setIsModalOpen(false);
    },
  });

  const {
    register,
    handleSubmit,
    formState: { errors, isSubmitting },
    reset,
  } = useForm<ProfileFormData>({ resolver: zodResolver(profileSchema) });

  const onSubmit = (data: ProfileFormData) => updateMutate(data);

  if (isLoading) {
    return <LoadingScreen />;
  }

  if (!user) {
    return <Navigate to="/" />;
  }

  return (
    <main className="flex-1 bg-gray-100 pt-32 px-8">
      <div className="max-w-4xl mx-auto">
        <h1 className="heading-1 mb-4">Profile</h1>
        <Filter
          options={menuItems.map((item) => ({
            value: item.key,
            label: item.label,
            icon: item.icon,
          }))}
          chosen={menuOption}
          setter={setMenuOption}
        />

        {(userProfileLoading || detailsLoading || isLoading) && (
          <div className="flex justify-center py-8">
            <Spinner />
          </div>
        )}

        {menuOption === "settings" && userProfile && (
          <div className="card flex flex-col gap-4">
            <div>
              <p className="text-sm font-medium text-gray-500">First name</p>
              <p className="text-lg font-semibold">{userProfile.name}</p>
            </div>

            <div>
              <p className="text-sm font-medium text-gray-500">Surname</p>
              <p className="text-lg font-semibold">{userProfile.surname}</p>
            </div>

            <div>
              <p className="text-sm font-medium text-gray-500">Email</p>
              <p className="text-lg font-semibold">{userProfile.email}</p>
            </div>

            <button
              className="btn-primary mt-2 self-start"
              onClick={() => {
                reset({
                  name: userProfile.name,
                  surname: userProfile.surname,
                  email: userProfile.email,
                });
                setIsModalOpen(true);
              }}
            >
              Edit profile
            </button>
          </div>
        )}

        {menuOption === "invites" && details && (
          <>
            {details.team_invites?.length ? (
              <ul className="divide-y divide-gray-200 bg-white rounded-lg shadow">
                {details.team_invites.map((invite) => (
                  <li key={invite.team_id}>
                    <TeamInvite
                      invite={invite}
                      callback={() => refecthDetails()}
                    />
                  </li>
                ))}
              </ul>
            ) : (
              <span className="text-center py-4 text-gray-500">
                No team invites
              </span>
            )}
          </>
        )}

        {menuOption === "teams" && details && (
          <>
            <input
              type="text"
              value={teamsSearch}
              onChange={(e) => setTeamsSearch(e.target.value)}
              placeholder="Search teams..."
              className="input-field mb-4"
            />

            {details.managing?.length ? (
              <ul className="flex flex-col gap-4">
                {details.managing.map((team) => (
                  <li key={team.id}>
                    <TeamManager initialTeam={team} />
                  </li>
                ))}
              </ul>
            ) : (
              <span className="text-center py-4 text-gray-500">
                No managed teams
              </span>
            )}
          </>
        )}

        {menuOption === "tournaments" && details && (
          <>
            <input
              type="text"
              value={tournamentsSearch}
              onChange={(e) => setTournamentsSearch(e.target.value)}
              placeholder="Search tournaments..."
              className="input-field mb-4"
            />

            {details.created_tournaments?.length ? (
              <ul className="flex flex-col gap-4">
                {details.created_tournaments.map((tournament) => (
                  <li key={tournament.id}>
                    <TournamentDetailedCard
                      tournament={tournament}
                      participant={tournament.participant_requests}
                      refetch={refecthDetails}
                    />
                  </li>
                ))}
              </ul>
            ) : (
              <span className="text-center py-4 text-gray-500">
                No created tournaments
              </span>
            )}
          </>
        )}
      </div>

      <Modal open={isModalOpen} onClose={setIsModalOpen} title="Edit profile">
        <form className="space-y-4" onSubmit={handleSubmit(onSubmit)}>
          <div className="flex gap-2">
            <div className="flex-1">
              <label
                htmlFor="name"
                className="block mb-2 text-sm font-medium text-gray-900"
              >
                First name
              </label>
              <input
                type="text"
                id="name"
                className="input-field"
                {...register("name")}
              />
              {errors.name && (
                <p className="text-red-500 text-sm mt-2">
                  {errors.name.message}
                </p>
              )}
            </div>

            <div className="flex-1">
              <label
                htmlFor="surname"
                className="block mb-2 text-sm font-medium text-gray-900"
              >
                Surname
              </label>
              <input
                type="text"
                id="surname"
                className="input-field"
                {...register("surname")}
              />
              {errors.surname && (
                <p className="text-red-500 text-sm mt-2">
                  {errors.surname.message}
                </p>
              )}
            </div>
          </div>

          <div>
            <label
              htmlFor="email"
              className="block mb-2 text-sm font-medium text-gray-900"
            >
              Email
            </label>
            <input
              type="email"
              id="email"
              className="input-field"
              {...register("email")}
            />
            {errors.email && (
              <p className="text-red-500 text-sm mt-2">
                {errors.email.message}
              </p>
            )}
          </div>

          <button
            type="submit"
            className="w-full btn-primary mt-8"
            disabled={isSubmitting}
          >
            Save changes
          </button>
        </form>
      </Modal>
    </main>
  );
}
