/**
 * IIS Project
 * @brief Schema for updating the profile
 * @author Dmitrii Ivanushkin
 */
import { z } from "zod";

export const profileSchema = z.object({
  name: z.string().min(1, "First name is required"),
  surname: z.string().min(1, "Surname is required"),
  email: z.string().email("Invalid email address"),
});

export type ProfileFormData = z.infer<typeof profileSchema>;
