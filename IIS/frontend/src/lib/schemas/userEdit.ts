/**
 * IIS Project
 * @brief Schema for admin screen
 * @author Albert Tikaiev
 */
import z from "zod";

export const userEditSchema = z.object({
  email: z.string().min(1, "Email is required").email("Invalid email address"),
  password: z.string(),
});

export type UserEditFormData = z.infer<typeof userEditSchema>;
