/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Dmitrii Ivanushkin
 */
import { api } from "../client";
import type { LogoutResponse } from "../types";

export async function logout() {
  const res = await api.post<LogoutResponse>("/auth/logout");

  return res.data;
}
