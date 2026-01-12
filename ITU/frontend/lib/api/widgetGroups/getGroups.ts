/**
 * Project: ITU
 * Author: Dmitrii Ivanushkin
 * File: GET WidgetGroup[]
 */
import { API_URL } from "../config";
import { WidgetGroup } from "../types";

export async function getGroups(): Promise<WidgetGroup[]> {
  const res = await fetch(`${API_URL}/widget-groups/`);
  if (!res.ok) throw new Error("Failed to fetch groups");
  return res.json();
}
