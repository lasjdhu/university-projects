/**
 * Project: ITU
 * Author: Dmitrii Ivanushkin
 * File: A bottom sheet component for customizing widget appearance.
 * Allows the user to select a color from a predefined palette and search/select an icon.
 */
import React, { useMemo } from "react";
import { View, Text, TouchableOpacity, StyleSheet } from "react-native";
import {
  BottomSheetModal,
  BottomSheetScrollView,
  BottomSheetTextInput,
  BottomSheetBackdrop,
} from "@gorhom/bottom-sheet";
import { Search, Check } from "lucide-react-native";
import { appColors, COLORS } from "@/lib/constants/colors";
import { getIconComponent, IconMap } from "@/lib/constants/icons";

interface AppearanceSheetProps {
  bottomSheetRef: React.RefObject<BottomSheetModal | null>;
  currentAppearance: { icon?: string; color?: string };
  iconSearch: string;
  setIconSearch: (text: string) => void;
  setValue: (field: "icon" | "color", value: string) => void;
  onSave: () => void;
}

const ICON_KEYS = Object.keys(IconMap);

const AppearanceSheet = ({
  bottomSheetRef,
  currentAppearance,
  iconSearch,
  setIconSearch,
  setValue,
  onSave,
}: AppearanceSheetProps) => {
  const allIcons = useMemo(() => {
    const lowerSearch = iconSearch.toLowerCase();
    return ICON_KEYS.filter((name) => name.toLowerCase().includes(lowerSearch));
  }, [iconSearch]);

  const snapPoints = useMemo(() => ["50%"], []);

  return (
    <BottomSheetModal
      ref={bottomSheetRef}
      index={0}
      snapPoints={snapPoints}
      enablePanDownToClose
      backdropComponent={(props) => (
        <BottomSheetBackdrop
          {...props}
          appearsOnIndex={0}
          disappearsOnIndex={-1}
          pressBehavior="close"
        />
      )}
    >
      <View style={styles.sheetContent}>
        <View style={styles.sheetHeader}>
          <Text style={styles.sheetTitle}>Appearance</Text>
          <TouchableOpacity style={styles.saveButton} onPress={onSave}>
            <Text style={styles.saveButtonText}>Save</Text>
          </TouchableOpacity>
        </View>

        <Text style={styles.sheetSubtitle}>Color</Text>
        <View style={styles.colorGrid}>
          {COLORS.map((c) => (
            <TouchableOpacity
              key={c}
              style={[
                styles.colorCell,
                { backgroundColor: c },
                currentAppearance.color === c && styles.colorCellSelected,
              ]}
              onPress={() => setValue("color", c)}
            >
              {currentAppearance.color === c && (
                <Check color="#fff" size={16} />
              )}
            </TouchableOpacity>
          ))}
        </View>

        <Text style={styles.sheetSubtitle}>Icon</Text>
        <View style={styles.searchContainer}>
          <Search size={20} color={appColors.gray} style={{ marginRight: 8 }} />
          <BottomSheetTextInput
            style={styles.searchInput}
            placeholder="Search icons..."
            value={iconSearch}
            onChangeText={setIconSearch}
          />
        </View>

        <BottomSheetScrollView contentContainerStyle={styles.iconGrid}>
          {allIcons.slice(0, 50).map((iconName) => {
            const Ico = getIconComponent(iconName);
            return (
              <TouchableOpacity
                key={iconName}
                style={[
                  styles.iconCell,
                  currentAppearance.icon === iconName &&
                    styles.iconCellSelected,
                ]}
                onPress={() => setValue("icon", iconName)}
              >
                <Ico
                  color={
                    currentAppearance.icon === iconName
                      ? "#fff"
                      : appColors.darkGray
                  }
                  size={24}
                />
              </TouchableOpacity>
            );
          })}
        </BottomSheetScrollView>
      </View>
    </BottomSheetModal>
  );
};

const styles = StyleSheet.create({
  sheetContent: {
    padding: 16,
    flex: 1,
  },
  sheetHeader: {
    flexDirection: "row",
    justifyContent: "space-between",
    alignItems: "center",
    marginBottom: 16,
  },
  sheetTitle: {
    fontSize: 20,
    fontWeight: "bold",
  },
  saveButton: {
    backgroundColor: appColors.primary,
    paddingVertical: 6,
    paddingHorizontal: 12,
    borderRadius: 8,
  },
  saveButtonText: {
    color: "#fff",
    fontWeight: "bold",
  },
  sheetSubtitle: {
    fontSize: 16,
    fontWeight: "bold",
    marginTop: 16,
    marginBottom: 8,
  },
  colorGrid: {
    flexDirection: "row",
    flexWrap: "wrap",
  },
  colorCell: {
    width: 32,
    height: 32,
    borderRadius: 16,
    margin: 6,
    justifyContent: "center",
    alignItems: "center",
  },
  colorCellSelected: {
    borderWidth: 2,
    borderColor: appColors.darkGray,
  },
  searchContainer: {
    flexDirection: "row",
    justifyContent: "space-between",
    alignItems: "center",
    backgroundColor: "#f0f0f0",
    paddingHorizontal: 12,
    borderRadius: 8,
    borderWidth: 1,
    marginBottom: 20,
    borderColor: appColors.gray,
  },
  searchInput: { flex: 1, fontSize: 16 },
  iconGrid: {
    flexDirection: "row",
    flexWrap: "wrap",
    justifyContent: "space-between",
  },
  iconCell: {
    width: "22%",
    aspectRatio: 1,
    justifyContent: "center",
    alignItems: "center",
    backgroundColor: "#f0f0f0",
    borderRadius: 8,
    marginBottom: 10,
  },
  iconCellSelected: {
    backgroundColor: appColors.primary,
  },
});

export default AppearanceSheet;
