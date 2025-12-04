/**
 * IIS Project
 * @brief Schema for inviting to team
 * @author Albert Tikaiev
 */
import z from "zod";

export const invitePlayerSchema = z.object({
  email: z.string().min(1, "Email is required").email("Invalid email address"),
});

export type InvitePlayerFormData = z.infer<typeof invitePlayerSchema>;
