/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Dmitrii Ivanushkin
 */
import type { LoginFormData } from "@/lib/schemas/login";
import { api } from "../client";
import type { UserResponse } from "../types";

export async function login(data: LoginFormData) {
  const res = await api.post<UserResponse>("/auth/login", data);

  return res.data;
}
