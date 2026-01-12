/**
 * Project: ITU
 * Author: Dmitrii Ivanushkin
 * File: POST WidgetGroup
 */
import { WidgetGroupFormValues } from "@/lib/schemas/widgetGroupSchemas";
import { API_URL } from "../config";
import { WidgetGroup } from "../types";

export async function createGroup(
  data: WidgetGroupFormValues,
): Promise<WidgetGroup> {
  const res = await fetch(`${API_URL}/widget-groups/`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(data),
  });
  if (!res.ok) throw new Error("Failed to create group");
  return res.json();
}
