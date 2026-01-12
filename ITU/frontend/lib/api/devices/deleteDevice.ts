//created by xkrasoo00 on November 1st, 2025

import { API_URL } from "../config";

export async function deleteDevice(id: number) {
  const res = await fetch(`${API_URL}/devices/${id}`, { method: "DELETE" });
  if (!res.ok) throw new Error("Failed to delete device");
  return res.json();
}
