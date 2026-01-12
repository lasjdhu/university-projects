/**
 * Project: ITU
 * Author: Dmitrii Ivanushkin
 * File: GET Widget[]
 */
import { API_URL } from "../config";
import { Widget } from "../types";

export async function getWidgets(): Promise<Widget[]> {
  const res = await fetch(`${API_URL}/widgets/`);
  if (!res.ok) throw new Error("Failed to fetch widgets");
  return res.json();
}
