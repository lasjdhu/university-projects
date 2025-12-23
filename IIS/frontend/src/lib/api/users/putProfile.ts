/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Dmitrii Ivanushkin
 */
import { api } from "../client";
import type { ProfileFormData } from "@/lib/schemas/profile";

export async function putProfile(data: ProfileFormData) {
  const res = await api.put("/user/profile/me", data);

  return res.data;
}
