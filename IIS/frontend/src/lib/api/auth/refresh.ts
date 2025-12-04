/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Dmitrii Ivanushkin
 */
import { api } from "../client";
import type { UserResponse } from "../types";

export async function refreshToken() {
  const res = await api.post<UserResponse>("/auth/refresh");

  return res.data;
}
