/**
 * Project: ITU
 * Author: Dmitrii Ivanushkin
 * File: Layout for the widgets section.
 * Defines the stack navigation between the dashboard and details view.
 */
import { Stack } from "expo-router";

export default function WidgetsLayout() {
  return (
    <Stack>
      <Stack.Screen name="index" options={{ title: "Widgets" }} />
      <Stack.Screen name="[id]" options={{ title: "Widget Details" }} />
    </Stack>
  );
}
