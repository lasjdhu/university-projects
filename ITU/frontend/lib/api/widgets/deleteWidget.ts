/**
 * Project: ITU
 * Author: Dmitrii Ivanushkin
 * File: DELETE Widget
 */
import { API_URL } from "../config";

export async function deleteWidget(id: number) {
  const res = await fetch(`${API_URL}/widgets/${id}`, { method: "DELETE" });
  if (!res.ok) throw new Error("Failed to delete widget");
  return res.json();
}
