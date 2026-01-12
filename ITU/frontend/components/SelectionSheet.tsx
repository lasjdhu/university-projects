/**
 * Project: ITU
 * Author: Dmitrii Ivanushkin
 * File: Generic bottomsheet for selecting a single item from a list.
 */
import { useRef, useMemo, useCallback } from "react";
import { View, Text, TouchableOpacity, StyleSheet } from "react-native";
import {
  BottomSheetModal,
  BottomSheetScrollView,
  BottomSheetBackdrop,
} from "@gorhom/bottom-sheet";
import { Check, ChevronDown } from "lucide-react-native";
import { appColors } from "@/lib/constants/colors";

interface SelectionSheetProps {
  label: string;
  placeholder: string;
  items: { label: string; value: number }[];
  value?: number | null;
  onSelect: (value: number) => void;
  disabled?: boolean;
}

export function SelectionSheet({
  label,
  placeholder,
  items,
  value,
  onSelect,
  disabled,
}: SelectionSheetProps) {
  const bottomSheetModalRef = useRef<BottomSheetModal>(null);
  const snapPoints = useMemo(() => ["50%"], []);

  const handlePresentModalPress = useCallback(() => {
    if (disabled) return;
    bottomSheetModalRef.current?.present();
  }, [disabled]);

  const selectedItem = items.find((i) => i.value === value);

  return (
    <View style={styles.container}>
      {!!label && <Text style={styles.label}>{label}</Text>}

      <TouchableOpacity
        style={[styles.button, disabled && styles.disabled]}
        onPress={handlePresentModalPress}
        disabled={disabled}
      >
        <Text style={styles.buttonText}>
          {selectedItem?.label ?? placeholder ?? "Select..."}
        </Text>
        <ChevronDown size={20} color={disabled ? appColors.gray : "#666"} />
      </TouchableOpacity>

      <BottomSheetModal
        ref={bottomSheetModalRef}
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
          <Text style={styles.sheetTitle}>{label || placeholder}</Text>
          <BottomSheetScrollView>
            {items.map((item) => (
              <TouchableOpacity
                key={item.value}
                style={styles.item}
                onPress={() => {
                  onSelect(item.value);
                  bottomSheetModalRef.current?.dismiss();
                }}
              >
                <Text
                  style={[
                    styles.itemText,
                    item.value === value && styles.selectedItemText,
                  ]}
                >
                  {item.label}
                </Text>
                {item.value === value && (
                  <Check size={20} color={appColors.primary} />
                )}
              </TouchableOpacity>
            ))}
          </BottomSheetScrollView>
        </View>
      </BottomSheetModal>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    marginBottom: 16,
  },
  label: {
    fontSize: 14,
    fontWeight: "bold",
    marginBottom: 4,
    color: appColors.darkGray,
  },
  button: {
    flexDirection: "row",
    justifyContent: "space-between",
    alignItems: "center",
    backgroundColor: "#f0f0f0",
    padding: 12,
    borderRadius: 8,
    borderWidth: 1,
    borderColor: appColors.gray,
  },
  disabled: {
    opacity: 0.6,
  },
  buttonText: {
    fontSize: 16,
  },
  sheetContent: {
    flex: 1,
    paddingHorizontal: 16,
  },
  sheetTitle: {
    fontSize: 18,
    fontWeight: "bold",
    marginBottom: 16,
    textAlign: "center",
  },
  item: {
    paddingVertical: 16,
    borderBottomWidth: 1,
    borderBottomColor: "#eee",
    flexDirection: "row",
    justifyContent: "space-between",
  },
  itemText: {
    fontSize: 16,
    color: appColors.darkGray,
  },
  selectedItemText: {
    color: appColors.primary,
    fontWeight: "bold",
  },
});
