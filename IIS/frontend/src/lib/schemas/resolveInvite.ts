/**
 * IIS Project
 * @brief Schema for reacting to invite
 * @author Albert Tikaiev
 */
import { z } from "zod";

export const resolveInviteSchema = z.object({
  team_id: z.number().min(1, "Team ID is required"),
  result: z.enum(["Accept", "Reject"]),
});

export type ResolveInviteFormData = z.infer<typeof resolveInviteSchema>;
