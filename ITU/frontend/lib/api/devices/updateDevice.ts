//created by xkrasoo00 on November 1st, 2025

import { Device } from "../types";
import { API_URL } from "../config";
import { DeviceFormValues } from "@/lib/schemas/device";

export async function updateDevice(
  id: number,
  data: DeviceFormValues,
): Promise<Device> {
  const res = await fetch(`${API_URL}/devices/${id}`, {
    method: "PUT",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(data),
  });
  if (!res.ok) throw new Error("Failed to update device");
  return res.json();
}

export async function fetchDevice(id: number): Promise<Device> {
  const res = await fetch(`${API_URL}/devices/${id}`, {
    method: "GET",
    headers: { "Content-Type": "application/json" },
  });
  if (!res.ok) throw new Error("Failed to get device");
  return res.json();
}
