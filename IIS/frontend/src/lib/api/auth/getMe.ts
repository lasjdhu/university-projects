/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Dmitrii Ivanushkin
 */
import { api } from "../client";
import type { UserResponse } from "../types";

export async function getMe() {
  const res = await api.get<UserResponse>("/auth/user/me");

  return res.data;
}
