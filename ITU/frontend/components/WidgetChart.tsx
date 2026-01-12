/**
 * Project: ITU
 * Author: Dmitrii Ivanushkin
 * File: Chart component for visualization of historical widget data.
 */
import React from "react";
import { View, ActivityIndicator, Dimensions, StyleSheet } from "react-native";
import { LineChart } from "react-native-gifted-charts";
import { parseTimestamp } from "@/lib/utils/time";
import { WidgetData } from "@/lib/api/types";
import { appColors } from "@/lib/constants/colors";

const screenWidth = Dimensions.get("window").width;

interface WidgetChartProps {
  data: WidgetData | undefined;
  color: string;
  loading: boolean;
}

const WidgetChart = ({ data, color, loading }: WidgetChartProps) => {
  // Map API history data to chart format
  const chartData =
    data?.history.map((h) => {
      const date = parseTimestamp(h.timestamp);

      return {
        value: h.value,
        label: date.toLocaleTimeString(["en-GB"], {
          hour: "2-digit",
          minute: "2-digit",
        }),
      };
    }) || [];

  // Provide fallback data to prevent chart crash on empty array
  const safeChartData =
    chartData.length > 0 ? chartData : [{ value: 0, label: "" }];

  return (
    <View style={styles.chartContainer}>
      {loading && !data ? (
        <ActivityIndicator color={appColors.primary} />
      ) : (
        <LineChart
          data={safeChartData}
          height={200}
          width={screenWidth - 64}
          spacing={60}
          initialSpacing={20}
          color={color || appColors.primary}
          thickness={3}
          curved
          hideDataPoints={false}
          dataPointsColor={color || appColors.primary}
          startFillColor={color || appColors.primary}
          endFillColor="#fff"
          startOpacity={0.2}
          endOpacity={0.0}
          areaChart
          yAxisTextStyle={{ color: appColors.gray }}
          rulesColor="#eee"
          hideRules={false}
          yAxisColor="transparent"
          xAxisColor="transparent"
          xAxisLabelTextStyle={{ fontSize: 10, color: appColors.gray }}
          scrollToEnd
        />
      )}
    </View>
  );
};

const styles = StyleSheet.create({
  chartContainer: {
    marginBottom: 32,
    alignItems: "center",
    justifyContent: "center",
    borderRadius: 16,
    overflow: "hidden",
  },
});

export default WidgetChart;
