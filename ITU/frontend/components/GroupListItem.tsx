/*
 *     created by xkrasoo00 on December 11th, 2025
 *
 *     this file describes ain item in a scrolling group list
 *
 */

import { Check, Trash2 } from "lucide-react-native";
import { appColors } from "@/lib/constants/colors";
import {
  TouchableOpacity,
  View,
  Text,
  StyleSheet,
  TextInput,
} from "react-native";
import { useState } from "react";
import { DeviceGroup } from "@/lib/api/types";

export const GroupListItem = ({
  group,
  isActive,
  isEditing,
  onSelect,
  onRename,
  onLongPress,
  onDelete,
  onUpdateSubmit,
}: {
  group: DeviceGroup;
  isActive: boolean;
  isEditing: boolean;
  onSelect: (id: number) => void;
  onRename: (id: number) => void;
  onLongPress: (id: number) => void;
  onDelete: (id: number) => void;
  onUpdateSubmit: (id: number, name: string) => void;
}) => {
  const [editingName, setEditingName] = useState(group.name);

  return (
    <View
      style={{
        flexDirection: "row",
        alignItems: "center",
        marginRight: 4,
        marginLeft: 4,
      }}
    >
      <TouchableOpacity
        style={[styles.tag, isActive && styles.activeTag]}
        onPress={() => onSelect(group.id)}
        onLongPress={() => onLongPress(group.id)}
        activeOpacity={0.7}
      >
        {isEditing ? (
          <>
            <TextInput
              value={editingName}
              onChangeText={setEditingName}
              style={styles.input}
              autoFocus
              onSubmitEditing={() => onUpdateSubmit(group.id, editingName)}
            />
            <TouchableOpacity
              onPress={() => onUpdateSubmit(group.id, editingName)}
            >
              <Check size={18} color={appColors.primary} />
            </TouchableOpacity>
          </>
        ) : (
          <Text style={styles.tagText}>{group.name}</Text>
        )}
      </TouchableOpacity>

      {isEditing && (
        <TouchableOpacity
          onPress={() => onDelete(group.id)}
          style={{ paddingHorizontal: 4 }}
        >
          <Trash2 size={18} color="red" />
        </TouchableOpacity>
      )}
    </View>
  );
};

const styles = StyleSheet.create({
  tag: {
    flexDirection: "row",
    alignItems: "center",
    backgroundColor: "#E5E5E5",
    paddingHorizontal: 14,
    paddingVertical: 8,
    borderRadius: 16,
    gap: 8,
    minHeight: 36,
  },
  activeTag: {
    backgroundColor: appColors.secondary,
  },
  tagText: {
    fontSize: 14,
    color: "#333",
    fontWeight: "500",
  },
  input: {
    minWidth: 80,
    fontSize: 14,
    paddingHorizontal: 8,
    paddingVertical: 4,
    borderWidth: 1,
    borderColor: appColors.gray,
    borderRadius: 12,
    height: 32,
  },
});
