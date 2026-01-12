/**
 * Project: ITU
 * Authors: Dmitrii Ivanushkin
 * File: Function to retrieve all icons for appearance picket.
 */
import * as Icons from "lucide-react-native";

export const IconMap = Icons as Record<string, any>;

export const getIconComponent = (name: string) => {
  return IconMap[name] || IconMap.HelpCircle;
};
