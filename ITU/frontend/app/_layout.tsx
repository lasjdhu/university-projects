/**
 * Project: ITU
 * Author: Dmitrii Ivanushkin
 * File: Root Application Layout.
 * Wraps the entire application in necessary context providers:
 * - GestureHandler: Needed for bottomsheet
 * - ThemeProvider: For navigation theming (light theme only Default)
 * - QueryClientProvider: For React Query state management and caching
 */
import { queryClient } from "@/lib/api/config";
import { DefaultTheme, ThemeProvider } from "@react-navigation/native";
import { QueryClientProvider } from "@tanstack/react-query";
import { Stack } from "expo-router";
import { StatusBar } from "expo-status-bar";
import "react-native-reanimated";
import { GestureHandlerRootView } from "react-native-gesture-handler";

export default function RootLayout() {
  return (
    <GestureHandlerRootView style={{ flex: 1 }}>
      <ThemeProvider value={DefaultTheme}>
        <QueryClientProvider client={queryClient}>
          <Stack>
            <Stack.Screen name="(tabs)" options={{ headerShown: false }} />
          </Stack>
        </QueryClientProvider>
        <StatusBar style="auto" />
      </ThemeProvider>
    </GestureHandlerRootView>
  );
}
