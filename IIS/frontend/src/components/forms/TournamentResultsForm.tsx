/**
 * IIS Project
 * @brief Form for changing tournament results by manager
 * @author Dmitrii Ivanushkin
 */
import { useForm, useFieldArray } from "react-hook-form";
import type { TournamentBracketResponse } from "@/lib/api/types";
import dayjs from "dayjs";

interface TournamentResultsFormProps {
  initialData: TournamentBracketResponse;
  onSubmit: (data: TournamentBracketResponse) => void;
  isSubmitting?: boolean;
}

export default function TournamentResultsForm({
  initialData,
  onSubmit,
  isSubmitting,
}: TournamentResultsFormProps) {
  const { control, handleSubmit, register, setValue, watch } =
    useForm<TournamentBracketResponse>({
      defaultValues: initialData,
    });

  const { fields: matchFields } = useFieldArray({
    control,
    name: "matches" as const,
  });

  const roundsGrouped: Record<string, number[]> = matchFields.reduce(
    (acc, match, idx) => {
      if (!acc[match.tournament_round_text]) {
        acc[match.tournament_round_text] = [];
      }
      acc[match.tournament_round_text].push(idx);
      return acc;
    },
    {} as Record<string, number[]>,
  );

  const formValues = watch();

  const normalizeDateString = (raw?: string | null) => {
    if (!raw) return raw ?? null;
    const parsed = dayjs(raw);
    if (!parsed.isValid()) return raw;
    return parsed.format("YYYY-MM-DDTHH:mm:ss");
  };

  const onSubmitInternal = (data: TournamentBracketResponse) => {
    const normalized: TournamentBracketResponse = {
      ...data,
      matches: (data.matches ?? []).map((m) => ({
        ...m,
        date: normalizeDateString(m.date),
      })),
    };

    onSubmit(normalized);
  };

  return (
    <form className="space-y-6" onSubmit={handleSubmit(onSubmitInternal)}>
      {Object.entries(roundsGrouped).map(([roundName, matchIndices]) => {
        return (
          <div key={roundName}>
            <h3 className="heading-2 mb-4">Round {roundName}</h3>
            <div className="space-y-4">
              {matchIndices.map((matchIndex) => {
                const match = matchFields[matchIndex];

                if (match.participants.some((p) => p.id === null)) {
                  return <p key={matchIndex}>This round is not yet decided</p>;
                }

                return (
                  <div key={match.id} className="card">
                    <h4 className="heading-3 mb-4">{match.name}</h4>

                    <div className="mb-4">
                      <label className="text-gray-700 mb-1 block">Date</label>
                      <input
                        type="datetime-local"
                        step="1"
                        className="input-field"
                        {...register(`matches.${matchIndex}.date`)}
                      />
                    </div>

                    <div className="grid grid-cols-1 sm:grid-cols-2 gap-3">
                      {match.participants.map(
                        (participant, participantIndex) => {
                          const winnerValue =
                            formValues.matches?.[matchIndex]?.participants?.[
                              participantIndex
                            ]?.is_winner;

                          return (
                            <div key={participant.id} className="flex flex-col">
                              <label className="text-gray-700 mb-1">
                                {participant.name}
                              </label>
                              <input
                                type="text"
                                className="input-field mb-2"
                                {...register(
                                  `matches.${matchIndex}.participants.${participantIndex}.result_text`,
                                )}
                              />
                              <label className="inline-flex items-center gap-2">
                                <input
                                  type="checkbox"
                                  className="rounded border-gray-300"
                                  checked={winnerValue}
                                  onChange={() => {
                                    match.participants.forEach((_, idx) => {
                                      setValue(
                                        `matches.${matchIndex}.participants.${idx}.is_winner`,
                                        idx === participantIndex,
                                      );
                                    });
                                  }}
                                />
                                Winner
                              </label>
                            </div>
                          );
                        },
                      )}
                    </div>
                  </div>
                );
              })}
            </div>
          </div>
        );
      })}

      <button
        type="submit"
        className="btn-primary w-full mt-4"
        disabled={isSubmitting}
      >
        Update
      </button>
    </form>
  );
}
