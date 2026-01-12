/**
 * Project: ITU
 * Author: Dmitrii Ivanushkin
 * File: Reusable collapsible component.
 * Used to hide/show settings in the details view.
 */
import { appColors } from "@/lib/constants/colors";
import { ChevronDown, ChevronUp } from "lucide-react-native";
import { PropsWithChildren, useState } from "react";
import { StyleSheet, TouchableOpacity, View, Text } from "react-native";

export function Collapsible({
  children,
  title,
}: PropsWithChildren & { title: string }) {
  const [isOpen, setIsOpen] = useState<boolean>(true);
  return (
    <View style={styles.section}>
      <TouchableOpacity
        style={styles.heading}
        onPress={() => setIsOpen((value) => !value)}
        activeOpacity={0.8}
      >
        <Text style={styles.sectionTitle}>{title}</Text>
        {isOpen ? (
          <ChevronUp size={20} color={appColors.gray} />
        ) : (
          <ChevronDown size={20} color={appColors.gray} />
        )}
      </TouchableOpacity>
      {isOpen && <View style={styles.content}>{children}</View>}
    </View>
  );
}

const styles = StyleSheet.create({
  heading: {
    flexDirection: "row",
    alignItems: "center",
    justifyContent: "space-between",
    gap: 6,
  },
  content: {
    padding: 24,
    paddingBottom: 0,
  },
  sectionTitle: {
    fontSize: 16,
    fontWeight: "bold",
    color: appColors.darkGray,
  },
  section: {
    marginBottom: 24,
  },
});
