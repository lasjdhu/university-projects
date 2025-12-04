/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Dmitrii Ivanushkin
 */
import type { SignupFormData } from "@/lib/schemas/signup";
import { api } from "../client";
import type { UserResponse } from "../types";

export async function signup(data: Omit<SignupFormData, "confirm_password">) {
  const res = await api.post<UserResponse>("/auth/register", data);

  return res.data;
}
