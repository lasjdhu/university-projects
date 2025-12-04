/**
 * IIS Project
 * @brief Form for changing team info by manager
 * @author Dmitrii Ivanushkin
 */
import { getUserSearchEmail } from "@/lib/api/users/getUserSearchEmail";
import { teamSchema, type TeamFormData } from "@/lib/schemas/team";
import { createQueryParams } from "@/lib/utils/createQueryParams";
import { useDebounce } from "@/lib/utils/useDebounce";
import { zodResolver } from "@hookform/resolvers/zod";
import { useQuery } from "@tanstack/react-query";
import { Plus, X } from "lucide-react";
import { useState } from "react";
import { useForm } from "react-hook-form";
import EmailSearchInput from "../EmailSearchInput";

interface TeamFormProps {
  initialData: Omit<TeamFormData, "manager_id">;
  onSubmit: (data: Omit<TeamFormData, "manager_id">) => void;
  isSubmitting?: boolean;
  playersInvite?: boolean;
  actionName: "Create" | "Save";
}

export default function TeamForm({
  initialData,
  onSubmit,
  isSubmitting,
  playersInvite = false,
  actionName,
}: TeamFormProps) {
  const [emails, setEmail] = useState<string[]>([]);
  const [emailInput, setEmailInput] = useState("");
  const debouncedSearch = useDebounce(emailInput, 400);

  const queryParams = createQueryParams({ search: debouncedSearch });

  const { data: userEmails } = useQuery({
    queryKey: ["users", debouncedSearch],
    queryFn: () => getUserSearchEmail(queryParams),
    initialData: [],
  });

  const {
    register,
    handleSubmit,
    setValue,
    formState: { errors },
  } = useForm<Omit<TeamFormData, "manager_id">>({
    defaultValues: initialData,
    resolver: zodResolver(teamSchema),
  });

  const addInvite = () => {
    const updatedEmails = [...emails, emailInput];
    setEmail(updatedEmails);
    setValue("email_invites", updatedEmails);
    setEmailInput("");
  };

  const deleteInvite = (invite_email: string) => {
    const updatedEmails = emails.filter((value) => value !== invite_email);
    setEmail(updatedEmails);
    setValue("email_invites", updatedEmails);
  };

  const onSubmitWithEmails = (data: Omit<TeamFormData, "manager_id">) => {
    data.email_invites = emails;
    onSubmit(data);
  };

  return (
    <form className="space-y-4" onSubmit={handleSubmit(onSubmitWithEmails)}>
      <div className="flex-1">
        <label className="block mb-2 text-sm font-medium text-gray-900">
          Name
        </label>
        <input type="text" className="input-field" {...register("name")} />
        {errors.name && (
          <p className="text-red-500 text-sm mt-2">{errors.name.message}</p>
        )}
      </div>

      <div className="flex-1">
        <label className="block mb-2 text-sm font-medium text-gray-900">
          Description
        </label>
        <textarea
          className="input-field resize-none min-h-32"
          {...register("description")}
        />
        {errors.description && (
          <p className="text-red-500 text-sm mt-2">
            {errors.description.message}
          </p>
        )}
      </div>

      {playersInvite && (
        <div className="flex-1">
          <label className="block mb-2 text-sm font-medium text-gray-900">
            Invite Players
          </label>

          <div className="relative w-full flex gap-2">
            <div className="flex-1">
              <EmailSearchInput
                value={emailInput}
                onChange={(v) => setEmailInput(v)}
                suggestions={userEmails}
                error={errors.email_invites?.root?.message}
              />
            </div>

            <button
              onClick={(ev) => {
                ev.preventDefault();
                addInvite();
              }}
              className="px-3 flex items-center text-gray-500 hover:text-gray-700 cursor-pointer hover:bg-gray-200 rounded-lg"
            >
              <Plus className="w-5 h-5" />
            </button>
          </div>

          <div className="mt-3 flex flex-wrap gap-2">
            {emails.map((value, i) => (
              <div
                key={i}
                className="flex items-center gap-2 px-3 py-1.5 rounded-full bg-gray-50 border border-gray-300 text-gray-900"
              >
                <span className="text-sm break-all">{value}</span>
                <button
                  onClick={(ev) => {
                    ev.preventDefault();
                    deleteInvite(value);
                  }}
                  className="flex items-center justify-center bg-gray-400 transition rounded-full p-1 cursor-pointer hover:bg-gray-500"
                >
                  <X className="w-4 h-4 text-white" />
                </button>

                {errors.email_invites && errors.email_invites[i] && (
                  <p className="text-red-500 text-xs mt-1">
                    {errors.email_invites[i].message}
                  </p>
                )}
              </div>
            ))}
          </div>
        </div>
      )}

      <button
        type="submit"
        className="w-full btn-primary mt-8"
        disabled={isSubmitting}
      >
        {actionName} Team
      </button>
    </form>
  );
}
