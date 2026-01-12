/**
 * Project: ITU
 * Author: Dmitrii Ivanushkin
 * File: App Entry Point.
 * Redirects the user immediately to the main dashboard (widgets tab).
 */
import { Redirect } from "expo-router";

export default function Index() {
  return <Redirect href="/(tabs)/widgets" />;
}
