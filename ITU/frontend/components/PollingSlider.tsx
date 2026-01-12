/**
 * Project: ITU
 * Author: Dmitrii Ivanushkin
 * File: Custom Slider for selecting data polling interval.
 */
import React, { useState, useEffect } from "react";
import { View, Text, StyleSheet } from "react-native";
import Slider from "@react-native-community/slider";
import { appColors } from "@/lib/constants/colors";

type PollingSliderProps = {
  initialValue: number;
  onUpdate: (val: number) => void;
};

export const PollingSlider = ({
  initialValue,
  onUpdate,
}: PollingSliderProps) => {
  const [value, setValue] = useState(initialValue);

  // Sync internal state if prop changes from parent
  useEffect(() => {
    setValue(initialValue);
  }, [initialValue]);

  return (
    <View>
      <Text style={styles.sectionTitle}>
        Update interval: every {value / 1000}s
      </Text>
      <Slider
        style={{ width: "100%", height: 40 }}
        minimumValue={1000}
        maximumValue={60000}
        step={1000}
        value={value}
        onValueChange={setValue}
        onSlidingComplete={onUpdate}
        minimumTrackTintColor={appColors.primary}
        maximumTrackTintColor="#000000"
      />
    </View>
  );
};

const styles = StyleSheet.create({
  sectionTitle: {
    fontSize: 16,
    fontWeight: "bold",
    color: appColors.darkGray,
  },
});
