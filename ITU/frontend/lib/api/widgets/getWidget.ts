/**
 * Project: ITU
 * Author: Dmitrii Ivanushkin
 * File: GET Widget
 */
import { API_URL } from "@/lib/api/config";
import { Widget } from "../types";

export async function getWidget(id: number): Promise<Widget> {
  const res = await fetch(`${API_URL}/widgets/${id}`);
  if (!res.ok) {
    throw new Error("Failed to fetch widget");
  }
  return res.json();
}
