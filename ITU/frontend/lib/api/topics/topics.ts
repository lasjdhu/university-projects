//created by xkrasoo00 on December 11th, 2025
// describes topics API

import {
  DeviceTopicReadSchema,
  DeviceTopicCreateSchema,
  DeviceTopicUpdateSchema,
} from "@/lib/api/types";
import { API_URL } from "@/lib/api/config";

export async function fetchTopics(
  deviceId: number,
): Promise<DeviceTopicReadSchema[]> {
  const res = await fetch(`${API_URL}/device-topics/${deviceId}`);
  if (!res.ok) throw new Error("Failed to fetch topics");
  return res.json();
}

export async function createTopic(
  deviceId: number,
  data: DeviceTopicCreateSchema,
) {
  const res = await fetch(`${API_URL}/device-topics/${deviceId}`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(data),
  });
  if (!res.ok) throw new Error("Failed to create topic");
  return res.json();
}

export async function updateTopic(
  deviceId: number,
  topicId: number,
  data: DeviceTopicUpdateSchema,
) {
  const res = await fetch(`${API_URL}/device-topics/${deviceId}/${topicId}`, {
    method: "PUT",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(data),
  });
  if (!res.ok) throw new Error("Failed to update topic");
  return res.json();
}

export async function deleteTopic(deviceId: number, topicId: number) {
  const res = await fetch(`${API_URL}/device-topics/${deviceId}/${topicId}`, {
    method: "DELETE",
  });
  if (!res.ok) throw new Error("Failed to delete topic");
}
