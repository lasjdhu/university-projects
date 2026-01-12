/**
 * Project: ITU
 * Author: Dmitrii Ivanushkin
 * File: POST Widget
 */
import { Widget } from "../types";
import { API_URL } from "../config";
import { WidgetFormValues } from "@/lib/schemas/widgetSchemas";

export async function createWidget(data: WidgetFormValues): Promise<Widget> {
  const res = await fetch(`${API_URL}/widgets/`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(data),
  });
  if (!res.ok) throw new Error("Failed to create widget");
  return res.json();
}
