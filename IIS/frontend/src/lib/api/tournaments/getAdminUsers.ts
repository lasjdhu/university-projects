/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Albert Tikaiev
 */
import { api } from "../client";
import type { PaginationResponse, UserResponse } from "../types";

export async function getAdminUsers(params: string) {
  const res = await api.get<PaginationResponse<UserResponse>>(
    "/admin/users" + params,
  );

  return res.data;
}
