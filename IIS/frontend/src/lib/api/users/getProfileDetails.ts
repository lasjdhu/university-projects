/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Albert Tikaiev
 */
import { api } from "../client";
import type { ProfileResponse } from "../types";

export async function getProfileDetails(params: string) {
  const res = await api.get<ProfileResponse>("/user/profile/details" + params);

  return res.data;
}
