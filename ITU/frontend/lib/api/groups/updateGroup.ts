//created by xkrasoo00 on November 1st, 2025

import { GroupFormValues } from "@/lib/schemas/deviceGroup";
import { API_URL } from "../config";
import { DeviceGroup } from "../types";
import { deleteGroup } from "./deleteGroup";

export async function updateGroup(
  id: number,
  data: GroupFormValues,
): Promise<DeviceGroup> {
  const res = await fetch(`${API_URL}/device-groups/${id}`, {
    method: "PUT",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(data),
  });
  if (!res.ok) {
    throw new Error("Failed to update group");
  }
  return res.json();
}
