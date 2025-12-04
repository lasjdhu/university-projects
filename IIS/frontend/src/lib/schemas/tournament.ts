/**
 * IIS Project
 * @brief Schema for creating/updating the tournament
 * @author Dmitrii Ivanushkin
 */
import { z } from "zod";

export const tournamentSchema = z
  .object({
    name: z.string().min(1, "Name is required"),
    discipline: z.string().min(1, "Discipline is required"),
    expected_members: z
      .number()
      .int("Capacity must be an integer")
      .min(1, "Capacity must be at least 1")
      .refine((val) => (val & (val - 1)) === 0, {
        message: "Capacity must be a power of 2 (2, 4, 8, 16, ...)",
      }),
    type: z.enum(["Person", "Team"]),
    prize: z.number().nullable(),
    min_limit: z.number().nullable(),
    max_limit: z.number().nullable(),
  })
  .superRefine((data, ctx) => {
    if (data.type === "Team") {
      if (data.min_limit === null || data.min_limit === undefined) {
        ctx.addIssue({
          code: z.ZodIssueCode.custom,
          message: "min_limit is required for Team type",
          path: ["min_limit"],
        });
      }
      if (data.max_limit === null || data.max_limit === undefined) {
        ctx.addIssue({
          code: z.ZodIssueCode.custom,
          message: "max_limit is required for Team type",
          path: ["max_limit"],
        });
      }
      if (
        data.min_limit !== null &&
        data.max_limit !== null &&
        data.min_limit > data.max_limit
      ) {
        ctx.addIssue({
          code: z.ZodIssueCode.custom,
          message: "min_limit cannot be greater than max_limit",
          path: ["min_limit"],
        });
      }
    }
  });

export type TournamentFormData = z.infer<typeof tournamentSchema> & {
  manager_id: number;
};
