//created by xkrasoo00 on November 1st, 2025

import { API_URL } from "../config";

export async function deleteGroup(id: number) {
  const res = await fetch(`${API_URL}/device-groups/${id}`, {
    method: "DELETE",
  });
  if (!res.ok) throw new Error("Failed to delete group");
  return res.json();
}
