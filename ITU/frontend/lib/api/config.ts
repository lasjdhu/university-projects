/**
 * Project: ITU
 * Author: Dmitrii Ivanushkin
 * File: API URL and queryClient definition.
 */
import { QueryClient } from "@tanstack/react-query";
import Constants from "expo-constants";

// If local BE is needed remove EXPO_PUBLIC_API_URL var from .env.local
export const API_URL =
  process.env.EXPO_PUBLIC_API_URL ??
  `http://${Constants.expoConfig?.hostUri?.split(":").shift()?.concat(":8080")}`;

export const queryClient = new QueryClient();
