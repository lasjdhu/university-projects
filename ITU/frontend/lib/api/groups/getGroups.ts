//created by xkrasoo00 on November 1st, 2025

import { API_URL } from "../config";
import { DeviceGroup } from "../types";

export async function getGroups(): Promise<DeviceGroup[]> {
  const url = `${API_URL}/device-groups/`;
  const res = await fetch(url);
  if (!res.ok) throw new Error("Failed to fetch groups");
  return res.json();
}
