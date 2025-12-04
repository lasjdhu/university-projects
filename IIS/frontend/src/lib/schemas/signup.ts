/**
 * IIS Project
 * @brief Schema for signup
 * @author Dmitrii Ivanushkin
 */
import z from "zod";

export const signupSchema = z
  .object({
    name: z.string().min(1, "First name is required"),
    surname: z.string().min(1, "Surname is required"),
    email: z.email("Invalid email address"),
    password: z.string().min(8, "Password must be at least 6 characters"),
    confirm_password: z.string().min(8, "Confirm password is required"),
  })
  .refine((data) => data.password === data.confirm_password, {
    message: "Passwords do not match",
    path: ["confirm_password"],
  });

export type SignupFormData = z.infer<typeof signupSchema>;
