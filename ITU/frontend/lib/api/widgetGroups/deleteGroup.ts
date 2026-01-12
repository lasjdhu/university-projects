/**
 * Project: ITU
 * Author: Dmitrii Ivanushkin
 * File: DELETE WidgetGroup
 */
import { API_URL } from "../config";

export async function deleteGroup(id: number) {
  const res = await fetch(`${API_URL}/widget-groups/${id}`, {
    method: "DELETE",
  });
  if (!res.ok) throw new Error("Failed to delete group");
  return res.json();
}
