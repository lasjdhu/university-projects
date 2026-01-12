/**
 * Project: ITU
 * Author: Dmitrii Ivanushkin
 * File: PUT WidgetGroup
 */
import { WidgetGroupFormValues } from "@/lib/schemas/widgetGroupSchemas";
import { API_URL } from "../config";
import { WidgetGroup } from "../types";

export async function updateGroup(
  id: number,
  data: WidgetGroupFormValues,
): Promise<WidgetGroup> {
  const res = await fetch(`${API_URL}/widget-groups/${id}`, {
    method: "PUT",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(data),
  });
  if (!res.ok) throw new Error("Failed to update group");
  return res.json();
}
