//created by xkrasoo00 on December 11th, 2025
// API for commands CRUD

import { CommandReadSchema } from "@/lib/api/types";
import { API_URL } from "@/lib/api/config";
import { CommandFormValues } from "@/lib/schemas/commandSchema";

export async function fetchCommands(
  deviceId: number,
): Promise<CommandReadSchema[]> {
  const res = await fetch(`${API_URL}/devices/${deviceId}/commands/`);
  if (!res.ok) throw new Error("Failed to fetch commands");
  return res.json();
}

export async function createCommand(deviceId: number, data: CommandFormValues) {
  const res = await fetch(`${API_URL}/devices/${deviceId}/commands/`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(data),
  });
  if (!res.ok) throw new Error("Failed to create command");
  return res.json();
}

export async function updateCommand(
  deviceId: number,
  commandId: number,
  data: CommandFormValues,
) {
  const res = await fetch(
    `${API_URL}/devices/${deviceId}/commands/${commandId}`,
    {
      method: "PUT",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(data),
    },
  );
  if (!res.ok) throw new Error("Failed to update command");
  return res.json();
}

export async function deleteCommand(commandId: number) {
  const res = await fetch(`${API_URL}/commands/${commandId}`, {
    method: "DELETE",
  });
  if (!res.ok) throw new Error("Failed to delete command");
}

export async function runCommand(commandId: number) {
  const res = await fetch(`${API_URL}/commands/${commandId}/run`, {
    method: "POST",
  });
  if (!res.ok) throw new Error("Failed to run command");
  return res.json();
}
