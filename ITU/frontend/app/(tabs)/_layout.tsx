/**
 * Project: ITU
 * Author: Dmitrii Ivanushkin
 * File: Main Tab layout configuration.
 * Provides bottom navigation to switch between Widgets Dashboard and Devices Management.
 */
import { appColors } from "@/lib/constants/colors";
import { Tabs } from "expo-router";
import { LayoutDashboard, Microchip } from "lucide-react-native";

export default function TabLayout() {
  return (
    <Tabs
      screenOptions={{
        tabBarActiveTintColor: appColors.primary,
        headerShown: false,
      }}
    >
      <Tabs.Screen
        name="widgets"
        options={{
          title: "Widgets",
          tabBarIcon: ({ color }) => (
            <LayoutDashboard size={28} color={color} />
          ),
        }}
      />
      <Tabs.Screen
        name="devices"
        options={{
          title: "Devices",
          tabBarIcon: ({ color }) => <Microchip size={28} color={color} />,
        }}
      />
    </Tabs>
  );
}
