/**
 * Project: ITU
 * Author: Dmitrii Ivanushkin
 * File: PATCH Widget
 */
import { Widget } from "../types";
import { API_URL } from "../config";
import { WidgetFormValues } from "@/lib/schemas/widgetSchemas";

export async function updateWidget(
  id: number,
  data: WidgetFormValues,
): Promise<Widget> {
  const res = await fetch(`${API_URL}/widgets/${id}`, {
    method: "PATCH",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(data),
  });
  if (!res.ok) throw new Error("Failed to update widget");
  return res.json();
}
