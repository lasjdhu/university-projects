/**
 * Project: ITU
 * Author: Dmitrii Ivanushkin
 * File: GET WidgetData
 */
import { API_URL } from "@/lib/api/config";
import { WidgetData } from "../types";

export async function getWidgetData(id: number): Promise<WidgetData> {
  const res = await fetch(`${API_URL}/widgets/${id}/data`);
  if (!res.ok) {
    throw new Error("Failed to fetch widget data");
  }
  return res.json();
}
