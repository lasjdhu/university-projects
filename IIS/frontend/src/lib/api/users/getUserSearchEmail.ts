/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Albert Tikaiev
 */
import { api } from "../client";
import type { BaseUserResponse } from "../types";

export async function getUserSearchEmail(params: string) {
  const res = await api.get<BaseUserResponse[]>("/user" + params);

  return res.data;
}
