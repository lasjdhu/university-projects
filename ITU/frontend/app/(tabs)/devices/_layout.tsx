/**
 * Project: ITU
 * Author: Dmitrii Ivanushkin
 * File: Layout for the device section
 * Defines the stack navigation between the dashboard and details view.
 */
import { Stack } from "expo-router";

export default function DevicesLayout() {
  return (
    <Stack>
      <Stack.Screen name="index" options={{ title: "Devices" }} />
      <Stack.Screen name="[id]" options={{ title: "Device Details" }} />
    </Stack>
  );
}
