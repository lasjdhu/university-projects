//created by xkrasoo00 on November 1st, 2025

import { API_URL } from "../config";
import { Device } from "../types";

export async function getDevices(groupId: number | null): Promise<Device[]> {
  const url =
    groupId !== null
      ? `${API_URL}/devices/?group_id=${groupId}`
      : `${API_URL}/devices/`;
  const res = await fetch(url);
  if (!res.ok) throw new Error("Failed to fetch devices");
  return res.json();
}
