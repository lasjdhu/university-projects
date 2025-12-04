/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Albert Tikaiev
 */
import type { UserEditFormData } from "@/lib/schemas/userEdit";
import { api } from "../client";

interface PutUserAdminData {
  id: number;
  data: UserEditFormData;
}

export async function putUserAdmin({ id, data }: PutUserAdminData) {
  const res = await api.put(`/admin/users/${id}`, data);

  return res.data;
}
