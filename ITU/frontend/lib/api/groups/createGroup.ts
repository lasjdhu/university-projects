//created by xkrasoo00 on November 1st, 2025

import { DeviceGroup } from "../types";
import { API_URL } from "../config";
import { GroupFormValues } from "@/lib/schemas/deviceGroup";

export async function createGroup(data: GroupFormValues): Promise<DeviceGroup> {
  const res = await fetch(`${API_URL}/device-groups/`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(data),
  });
  if (!res.ok) throw new Error("Failed to create group");
  return res.json();
}
