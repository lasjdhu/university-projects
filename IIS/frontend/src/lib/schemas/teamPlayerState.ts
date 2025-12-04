/**
 * IIS Project
 * @brief Schema for updating the player's state
 * @author Albert Tikaiev
 */
import z from "zod";

export const teamPlayerStateSchema = z.object({
  team_id: z.number().min(1, "Team ID is required"),
  player_id: z.number().min(1, "User ID is required"),
  state: z.enum(["Active", "Inactive", "Invited"]),
});

export type TeamPlayerStateFormData = z.infer<typeof teamPlayerStateSchema>;
