/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Dmitrii Ivanushkin
 */
import { api } from "../client";
import type { UserResponse } from "../types";

export async function getProfile() {
  const res = await api.get<UserResponse>("/user/profile/me");

  return res.data;
}
