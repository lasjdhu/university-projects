/**
 * Project: ITU
 * Author: Dmitrii Ivanushkin
 * File: Horizontal scrollable tabs for filtering widgets by Group.
 * Supports inline CRUD operations for groups.
 */
import React from "react";
import {
  View,
  Text,
  TouchableOpacity,
  ScrollView,
  TextInput,
  StyleSheet,
  TextInputSubmitEditingEvent,
  GestureResponderEvent,
} from "react-native";
import { Plus, Check, Edit2, Trash2 } from "lucide-react-native";
import { Control, Controller } from "react-hook-form";
import { WidgetGroup } from "@/lib/api/types";
import { WidgetGroupFormValues } from "@/lib/schemas/widgetGroupSchemas";
import { appColors } from "@/lib/constants/colors";
import { HIT_SLOP } from "@/lib/constants/theme";

interface WidgetGroupTabsProps {
  groups: WidgetGroup[];
  selectedGroup: number | null;
  editingGroupId: number | null;
  groupActionModeId: number | null;
  isCreatingGroup: boolean;
  control: Control<WidgetGroupFormValues, unknown, WidgetGroupFormValues>;
  onSelect: (id: number | null) => void;
  onLongPress: (id: number) => void;
  onEditStart: (id: number) => void;
  onDelete: (id: number) => void;
  onSubmitCreate: (
    e: TextInputSubmitEditingEvent | GestureResponderEvent,
  ) => void;
  onSubmitUpdate: (id: number, name: string) => void;
  onCreateStart: () => void;
}

const WidgetGroupTabs = ({
  groups,
  selectedGroup,
  editingGroupId,
  groupActionModeId,
  isCreatingGroup,
  control,
  onSelect,
  onLongPress,
  onEditStart,
  onDelete,
  onSubmitCreate,
  onSubmitUpdate,
  onCreateStart,
}: WidgetGroupTabsProps) => {
  const displayedGroups = groups || [];

  return (
    <ScrollView
      horizontal
      showsHorizontalScrollIndicator={false}
      style={{ marginBottom: 10, flexGrow: 0 }}
      contentContainerStyle={{ alignItems: "center" }}
    >
      {isCreatingGroup ? (
        <View style={styles.groupEditContainer}>
          <Controller
            control={control}
            name="name"
            render={({ field: { onChange, value } }) => (
              <TextInput
                autoFocus
                value={value}
                onChangeText={onChange}
                style={styles.groupInput}
                placeholder="Name"
                onSubmitEditing={onSubmitCreate}
              />
            )}
          />
          <TouchableOpacity
            onPress={onSubmitCreate}
            hitSlop={HIT_SLOP}
            style={styles.iconButton}
          >
            <Check color={appColors.primary} size={20} />
          </TouchableOpacity>
        </View>
      ) : (
        <TouchableOpacity
          onPress={onCreateStart}
          hitSlop={HIT_SLOP}
          style={styles.iconButton}
        >
          <Plus color={appColors.primary} size={24} />
        </TouchableOpacity>
      )}

      <TouchableOpacity
        onPress={() => onSelect(null)}
        style={[styles.groupTab, !selectedGroup && styles.groupTabSelected]}
      >
        <Text
          style={[styles.groupText, !selectedGroup && styles.groupTextSelected]}
        >
          All
        </Text>
      </TouchableOpacity>

      {displayedGroups.map((group) => {
        const isActionMode = groupActionModeId === group.id;
        const isEditing = editingGroupId === group.id;
        const isSelected = selectedGroup === group.id;

        const safeName =
          typeof group.name === "string"
            ? group.name
            : typeof group.name === "object" &&
                group.name !== null &&
                "name" in group.name
              ? (group.name as any).name
              : String(group.name || "");

        let tempEditName = safeName;

        // Inline Editing Mode
        if (isEditing) {
          return (
            <View key={group.id} style={styles.groupEditContainer}>
              <TextInput
                autoFocus
                defaultValue={safeName}
                onChangeText={(text) => {
                  tempEditName = text;
                }}
                style={styles.groupInput}
                onSubmitEditing={() => onSubmitUpdate(group.id, tempEditName)}
              />
              <TouchableOpacity
                onPress={() => onSubmitUpdate(group.id, tempEditName)}
                hitSlop={HIT_SLOP}
                style={styles.iconButton}
              >
                <Check color={appColors.primary} size={20} />
              </TouchableOpacity>
            </View>
          );
        }

        return (
          <TouchableOpacity
            key={group.id}
            onPress={() => onSelect(group.id)}
            onLongPress={() => onLongPress(group.id)}
            style={[styles.groupTab, isSelected && styles.groupTabSelected]}
          >
            {isActionMode ? (
              <View style={{ flexDirection: "row", alignItems: "center" }}>
                <TouchableOpacity
                  onPress={() => onEditStart(group.id)}
                  hitSlop={HIT_SLOP}
                  style={styles.iconButton}
                >
                  <Edit2 color={appColors.primary} size={20} />
                </TouchableOpacity>

                <View style={{ width: 10 }} />

                <TouchableOpacity
                  onPress={() => onDelete(group.id)}
                  hitSlop={HIT_SLOP}
                  style={styles.iconButton}
                >
                  <Trash2 color={appColors.error} size={20} />
                </TouchableOpacity>
              </View>
            ) : (
              <Text
                style={[
                  styles.groupText,
                  isSelected && styles.groupTextSelected,
                ]}
              >
                {safeName}
              </Text>
            )}
          </TouchableOpacity>
        );
      })}
    </ScrollView>
  );
};

const styles = StyleSheet.create({
  groupTab: {
    marginHorizontal: 10,
    paddingVertical: 8,
    paddingHorizontal: 4,
  },
  groupTabSelected: {
    borderBottomWidth: 2,
    borderBottomColor: appColors.primary,
  },
  groupText: {
    fontSize: 16,
    color: appColors.darkGray,
    fontWeight: "bold",
  },
  groupTextSelected: {
    color: appColors.primary,
  },
  groupEditContainer: {
    flexDirection: "row",
    alignItems: "center",
    borderBottomWidth: 1,
    borderBottomColor: appColors.primary,
    paddingLeft: 6,
    marginRight: 10,
  },
  groupInput: {
    fontSize: 16,
    color: appColors.darkGray,
    minWidth: 80,
    marginRight: 4,
  },
  iconButton: {
    padding: 4,
    justifyContent: "center",
    alignItems: "center",
  },
});

export default WidgetGroupTabs;
