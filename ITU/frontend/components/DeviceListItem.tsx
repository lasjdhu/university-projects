/*
 *     created by xkrasoo00 on December 11th, 2025
 *
 *     this file describes a device item on device groups screen
 *
 */

import { Device } from "@/lib/api/types";
import { Cog, Trash2 } from "lucide-react-native";
import {
  View,
  Text,
  TouchableOpacity,
  StyleSheet,
  Pressable,
} from "react-native";

export const DeviceListItem = ({
  item,
  onPress,
  onDelete,
}: {
  item: Device;
  onPress: (id: number) => void;
  onDelete: (id: number) => void;
}) => {
  return (
    <View style={styles.container}>
      <Pressable
        style={styles.deviceButton}
        onPress={() => onPress(item.id)}
        android_ripple={{ color: "#ccc" }}
      >
        <Text style={styles.deviceName}>{item.name}</Text>
        <Cog size={20} color="#333" style={styles.icon} />
      </Pressable>

      <TouchableOpacity
        onPress={() => onDelete(item.id)}
        style={styles.deleteButton}
      >
        <Trash2 size={20} color="red" />
      </TouchableOpacity>
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flexDirection: "row",
    alignItems: "center",
    marginVertical: 6,
  },
  deviceButton: {
    flex: 1,
    flexDirection: "row",
    alignItems: "center",
    justifyContent: "space-between",
    backgroundColor: "#E5E5E5",
    borderRadius: 12,
    paddingVertical: 14,
    paddingHorizontal: 16,
  },
  icon: {
    marginLeft: 10,
  },
  deviceName: {
    fontSize: 16,
    fontWeight: "500",
    color: "#333",
  },
  deleteButton: {
    marginLeft: 12,
    padding: 8,
  },
});
