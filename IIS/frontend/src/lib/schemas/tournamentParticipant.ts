/**
 * IIS Project
 * @brief Schema for registering for tournament
 * @author Dmitrii Ivanushkin
 */
import { z } from "zod";

export const tournamentParticipantSchema = z.object({
  player_id: z.number().nullable(),
  team_id: z.number().nullable(),
});

export type TournamentParticipantFormData = z.infer<
  typeof tournamentParticipantSchema
>;
