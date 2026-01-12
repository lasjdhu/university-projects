/**
 * Project: ITU
 * Author: Dmitrii Ivanushkin
 * File: Custom Toast Notification Component.
 * Supports different styles for success/info and optional action buttons.
 */
import { appColors } from "@/lib/constants/colors";
import { View, Text, TouchableOpacity, StyleSheet } from "react-native";

interface ToastProps {
  message: string;
  type?: "success" | "info";
  actionLabel?: string;
  onAction?: () => void;
  visible: boolean;
}

export function Toast({
  message,
  type = "info",
  actionLabel,
  onAction,
  visible,
}: ToastProps) {
  if (!visible) return null;

  return (
    <View style={styles.containerWrapper}>
      <View
        style={[
          styles.toast,
          type === "success" ? styles.success : styles.info,
        ]}
      >
        <Text style={styles.toastText}>{message}</Text>
        {actionLabel && (
          <TouchableOpacity onPress={onAction} style={styles.actionButton}>
            <Text style={styles.actionText}>{actionLabel}</Text>
          </TouchableOpacity>
        )}
      </View>
    </View>
  );
}

const styles = StyleSheet.create({
  containerWrapper: {
    position: "absolute",
    bottom: 20,
    left: 20,
    zIndex: 1000,
  },
  toast: {
    borderRadius: 8,
    paddingVertical: 12,
    paddingHorizontal: 24,
    flexDirection: "row",
    alignItems: "center",
    justifyContent: "space-between",
    minWidth: 200,
  },
  info: {
    backgroundColor: appColors.error,
  },
  success: {
    backgroundColor: appColors.success,
  },
  toastText: {
    color: "#fff",
    fontWeight: "bold",
    marginRight: 16,
  },
  actionButton: {
    backgroundColor: "#fff",
    paddingVertical: 4,
    paddingHorizontal: 12,
    borderRadius: 4,
  },
  actionText: {
    color: "#000",
    fontWeight: "bold",
  },
});
