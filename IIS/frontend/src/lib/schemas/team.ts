/**
 * IIS Project
 * @brief Schema for creating/updating the team
 * @author Albert Tikaiev
 */
import { z } from "zod";

export const teamSchema = z.object({
  name: z.string().min(3, "Name is too short").max(30, "Name is too long"),
  description: z.string().max(300, "Description is too long"),
  email_invites: z.array(z.string().email()),
});

export type TeamFormData = z.infer<typeof teamSchema> & {
  manager_id: number;
};
