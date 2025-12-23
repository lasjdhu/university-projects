/**
 * IIS Project
 * @brief Form for changing the tournament info by manager
 * @author Albert Tikaiev
 */
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import {
  tournamentSchema,
  type TournamentFormData,
} from "@/lib/schemas/tournament";
import { useState } from "react";

interface TournamentFormProps {
  initialData: Omit<TournamentFormData, "manager_id">;
  onSubmit: (data: Omit<TournamentFormData, "manager_id">) => void;
  isSubmitting?: boolean;
  actionName: "Create" | "Save";
  capacityEditDisabled?: boolean;
  typeDisabled?: boolean;
}

export default function TournamentForm({
  initialData,
  onSubmit,
  isSubmitting,
  actionName,
  capacityEditDisabled,
  typeDisabled,
}: TournamentFormProps) {
  const [typeOption, setTypeOption] = useState<"Team" | "Person">(
    initialData.type || "Person",
  );
  const {
    register,
    handleSubmit,
    formState: { errors },
  } = useForm<Omit<TournamentFormData, "manager_id">>({
    defaultValues: initialData,
    resolver: zodResolver(tournamentSchema),
  });

  return (
    <form className="space-y-4" onSubmit={handleSubmit(onSubmit)}>
      <div className="flex-1">
        <label
          htmlFor="name"
          className="block mb-2 text-sm font-medium text-gray-900"
        >
          Name
        </label>
        <input
          type="text"
          id="name"
          className="input-field"
          {...register("name")}
        />
        {errors.name && (
          <p className="text-red-500 text-sm mt-2">{errors.name.message}</p>
        )}
      </div>

      <div className="flex-1">
        <label
          htmlFor="discipline"
          className="block mb-2 text-sm font-medium text-gray-900"
        >
          Discipline
        </label>
        <input
          type="text"
          id="discipline"
          className="input-field"
          {...register("discipline")}
        />
        {errors.discipline && (
          <p className="text-red-500 text-sm mt-2">
            {errors.discipline.message}
          </p>
        )}
      </div>

      <div className="flex-1">
        <label
          htmlFor="expected_members"
          className="block mb-2 text-sm font-medium text-gray-900"
        >
          Capacity
        </label>
        <input
          type="number"
          id="expected_members"
          className="input-field"
          disabled={capacityEditDisabled}
          {...register("expected_members", { valueAsNumber: true })}
        />
        {errors.expected_members && (
          <p className="text-red-500 text-sm mt-2">
            {errors.expected_members.message}
          </p>
        )}
      </div>

      <div className="flex-1">
        <label
          htmlFor="prize"
          className="block mb-2 text-sm font-medium text-gray-900"
        >
          Prize
        </label>
        <input
          type="number"
          id="prize"
          className="input-field"
          disabled={typeDisabled}
          {...register("prize", { valueAsNumber: true })}
        />
        {errors.prize && (
          <p className="text-red-500 text-sm mt-2">{errors.prize.message}</p>
        )}
      </div>

      <div className="flex-1">
        <label
          htmlFor="type"
          className="block mb-2 text-sm font-medium text-gray-900"
        >
          Type
        </label>
        <select
          id="type"
          className="input-field"
          disabled={typeDisabled}
          {...register("type")}
          onChange={(value) =>
            value.target.value === "Person" || value.target.value === "Team"
              ? setTypeOption(value.target.value)
              : setTypeOption("Person")
          }
        >
          <option value="Person">Person</option>
          <option value="Team">Team</option>
        </select>
        {errors.type && (
          <p className="text-red-500 text-sm mt-2">{errors.type.message}</p>
        )}
      </div>

      {typeOption === "Team" ? (
        <>
          <div className="flex-1">
            <label
              htmlFor="min_limit"
              className="block mb-2 text-sm font-medium text-gray-900"
            >
              Minimal constraint
            </label>
            <input
              type="number"
              id="min_limit"
              className="input-field"
              disabled={typeDisabled}
              {...register("min_limit", { valueAsNumber: true })}
            />
            {errors.min_limit && (
              <p className="text-red-500 text-sm mt-2">
                {errors.min_limit.message}
              </p>
            )}
          </div>

          <div className="flex-1">
            <label
              htmlFor="max_limit"
              className="block mb-2 text-sm font-medium text-gray-900"
            >
              Maximum constraint
            </label>
            <input
              type="number"
              id="max_limit"
              className="input-field"
              disabled={typeDisabled}
              {...register("max_limit", { valueAsNumber: true })}
            />
            {errors.max_limit && (
              <p className="text-red-500 text-sm mt-2">
                {errors.max_limit.message}
              </p>
            )}
          </div>
        </>
      ) : (
        <></>
      )}

      <button
        type="submit"
        className="w-full btn-primary mt-8"
        disabled={isSubmitting}
      >
        {actionName} Tournament
      </button>
    </form>
  );
}
