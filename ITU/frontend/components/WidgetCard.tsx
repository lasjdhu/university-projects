/**
 * Project: ITU
 * Author: Dmitrii Ivanushkin
 * File: Widget Card Component.
 * Represents a single widget in the dashboard grid.
 * Handles multiple display modes:
 * View: Normal clickable card
 * New: Dashed placeholder for creation
 * Edit: Inline name editing
 * Action: Overlay for Edit/Delete actions
 */
import React from "react";
import {
  View,
  Text,
  TouchableOpacity,
  TextInput,
  StyleSheet,
  TextInputSubmitEditingEvent,
  GestureResponderEvent,
} from "react-native";
import { Check, Edit2, Trash2, HelpCircle } from "lucide-react-native";
import { Control, Controller } from "react-hook-form";
import { Widget } from "@/lib/api/types";
import { WidgetFullFormValues } from "@/lib/schemas/widgetSchemas";
import { getIconComponent } from "@/lib/constants/icons";
import { appColors } from "@/lib/constants/colors";
import { HIT_SLOP } from "@/lib/constants/theme";

interface WidgetCardProps {
  item: Widget;
  mode: "view" | "new" | "edit" | "action";
  control: Control<WidgetFullFormValues, unknown, WidgetFullFormValues>;
  onPress: () => void;
  onLongPress: () => void;
  onEditStart: () => void;
  onDelete: () => void;
  onSubmitCreate: (
    e: TextInputSubmitEditingEvent | GestureResponderEvent,
  ) => void;
  onSubmitUpdate: (id: number, name: string) => void;
}

const WidgetCard = ({
  item,
  mode,
  control,
  onPress,
  onLongPress,
  onEditStart,
  onDelete,
  onSubmitCreate,
  onSubmitUpdate,
}: WidgetCardProps) => {
  const IconComp = getIconComponent(item.icon || "HelpCircle");

  const displayValue =
    item.last_value !== null && item.last_value !== undefined
      ? item.last_value.toFixed(1) + " " + (item.value_desc ?? "-")
      : "-";

  // Mode: Creating a new widget
  if (mode === "new") {
    return (
      <View style={[styles.widgetContainer, styles.widgetNew]}>
        <View style={styles.widgetHeader}>
          <View style={[styles.iconContainer, { backgroundColor: "#ccc" }]}>
            <HelpCircle color="#fff" size={24} />
          </View>
        </View>
        <View style={styles.widgetBody}>
          <Controller
            control={control}
            name="name"
            render={({ field: { onChange, value } }) => (
              <TextInput
                autoFocus
                value={value}
                onChangeText={onChange}
                placeholder="Widget name"
                style={styles.widgetNameInput}
                onSubmitEditing={onSubmitCreate}
              />
            )}
          />
        </View>
      </View>
    );
  }

  // Mode: Editing the widget name inline
  if (mode === "edit") {
    return (
      <View style={[styles.widgetContainer, styles.editingContainer]}>
        <View style={styles.widgetHeader}>
          <View style={[styles.iconContainer, { backgroundColor: item.color }]}>
            <IconComp color="#fff" size={24} />
          </View>
          <Text style={styles.widgetValue}>{displayValue}</Text>
        </View>
        <View style={styles.editRow}>
          <TextInput
            autoFocus
            defaultValue={item.name}
            onChangeText={(text) => (item.name = text)}
            style={[styles.widgetNameInput, { flex: 1 }]}
          />
          <TouchableOpacity
            onPress={() => onSubmitUpdate(item.id, item.name)}
            hitSlop={HIT_SLOP}
            style={styles.iconButton}
          >
            <Check color={appColors.primary} size={20} />
          </TouchableOpacity>
        </View>
      </View>
    );
  }

  // Mode: Selection overlay
  if (mode === "action") {
    return (
      <View style={[styles.widgetContainer, styles.actionModeContainer]}>
        <TouchableOpacity style={styles.actionButtonBlue} onPress={onEditStart}>
          <Edit2 color="#fff" size={20} />
        </TouchableOpacity>
        <TouchableOpacity style={styles.actionButtonRed} onPress={onDelete}>
          <Trash2 color="#fff" size={20} />
        </TouchableOpacity>
      </View>
    );
  }

  // Mode: Standard View
  return (
    <TouchableOpacity
      style={styles.widgetContainer}
      onPress={onPress}
      onLongPress={onLongPress}
    >
      <View style={styles.widgetHeader}>
        <View
          style={[
            styles.iconContainer,
            { backgroundColor: item.color || appColors.gray },
          ]}
        >
          <IconComp color="#fff" size={24} />
        </View>
        <Text style={styles.widgetValue}>{displayValue}</Text>
      </View>
      <Text style={styles.widgetName} numberOfLines={2}>
        {item.name}
      </Text>
    </TouchableOpacity>
  );
};

const styles = StyleSheet.create({
  widgetContainer: {
    backgroundColor: "#fff",
    borderRadius: 12,
    padding: 16,
    height: 140,
    justifyContent: "space-between",
    borderWidth: 1,
    borderStyle: "solid",
    borderColor: "#DDD",
  },
  widgetNew: {
    borderWidth: 1,
    borderStyle: "dashed",
    borderColor: appColors.primary,
  },
  editingContainer: {
    borderColor: appColors.primary,
    borderWidth: 1,
  },
  widgetHeader: {
    flexDirection: "row",
    justifyContent: "space-between",
    alignItems: "center",
  },
  iconContainer: {
    width: 48,
    height: 48,
    borderRadius: 24,
    justifyContent: "center",
    alignItems: "center",
  },
  widgetValue: {
    fontSize: 20,
    fontWeight: "bold",
  },
  widgetName: {
    fontSize: 18,
    fontWeight: "bold",
    color: appColors.darkGray,
  },
  widgetBody: {
    flex: 1,
    justifyContent: "flex-end",
  },
  widgetNameInput: {
    fontSize: 16,
    fontWeight: "bold",
    borderBottomWidth: 1,
    borderBottomColor: appColors.gray,
    paddingVertical: 2,
  },
  actionModeContainer: {
    flexDirection: "row",
    justifyContent: "space-around",
    alignItems: "center",
  },
  actionButtonBlue: {
    width: 48,
    height: 48,
    borderRadius: 24,
    backgroundColor: appColors.primary,
    justifyContent: "center",
    alignItems: "center",
  },
  actionButtonRed: {
    width: 48,
    height: 48,
    borderRadius: 24,
    backgroundColor: appColors.error,
    justifyContent: "center",
    alignItems: "center",
  },
  editRow: {
    flexDirection: "row",
    alignItems: "center",
    marginTop: 10,
  },
  iconButton: {
    padding: 4,
    justifyContent: "center",
    alignItems: "center",
    marginLeft: 4,
  },
});

export default WidgetCard;
