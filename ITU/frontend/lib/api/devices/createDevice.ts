//created by xkrasoo00 on November 1st, 2025

import { Device } from "../types";
import { API_URL } from "../config";
import { DeviceFormValues } from "@/lib/schemas/device";

export async function createDevice(data: DeviceFormValues): Promise<Device> {
  const res = await fetch(`${API_URL}/devices/`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(data),
  });
  if (!res.ok) throw new Error("Failed to create device");
  return res.json();
}
