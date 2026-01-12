/**
 * Project: ITU
 * Author: Dmitrii Ivanushkin
 * File: Widget details.
 * User can edit widget settings (appearance, polling interval, device, topic).
 * Also it provides a historic chart for the chosen device topic and allows to run commands.
 */
import { useRef, useState, useEffect } from "react";
import {
  View,
  Text,
  ScrollView,
  StyleSheet,
  TouchableOpacity,
  TextInput,
  ActivityIndicator,
  KeyboardAvoidingView,
  Platform,
} from "react-native";
import { useLocalSearchParams, Stack, useRouter } from "expo-router";
import { useQuery, useMutation, useQueryClient } from "@tanstack/react-query";
import { Check, Play, Edit2, ChevronRight } from "lucide-react-native";
import {
  BottomSheetModal,
  BottomSheetModalProvider,
} from "@gorhom/bottom-sheet";
import { useForm, Controller } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";

import { getWidget } from "@/lib/api/widgets/getWidget";
import { updateWidget } from "@/lib/api/widgets/updateWidget";
import { getWidgetData } from "@/lib/api/widgets/getWidgetData";
import { getDevices } from "@/lib/api/devices/getDevices";
import { fetchTopics } from "@/lib/api/topics/topics";
import { fetchCommands, runCommand } from "@/lib/api/commands/commands";
import { getGroups } from "@/lib/api/widgetGroups/getGroups";
import { SelectionSheet } from "@/components/SelectionSheet";
import { Toast } from "@/components/Toast";
import { useTimeAgo } from "@/lib/utils/time";

import WidgetChart from "@/components/WidgetChart";
import AppearanceSheet from "@/components/AppearanceSheet";
import {
  appearanceSchema,
  valueDescSchema,
  WidgetFormValues,
} from "@/lib/schemas/widgetSchemas";
import { getIconComponent } from "@/lib/constants/icons";
import { appColors } from "@/lib/constants/colors";
import { PollingSlider } from "@/components/PollingSlider";
import { Collapsible } from "@/components/Collapsible";
import { HIT_SLOP } from "@/lib/constants/theme";

export default function WidgetDetails() {
  // Get ID from Router
  const { id } = useLocalSearchParams<{ id: string }>();
  const widgetId = parseInt(id, 10);
  const router = useRouter();
  const queryClient = useQueryClient();

  // Init a ref to handle bottomsheet from children
  const bottomSheetRef = useRef<BottomSheetModal>(null);

  // Local states
  const [iconSearch, setIconSearch] = useState("");
  const [editingDesc, setEditingDesc] = useState(false);
  const [runningCommandId, setRunningCommandId] = useState<number | null>(null);
  const [commandSuccess, setCommandSuccess] = useState<string | null>(null);

  // Form handling of value description (unit)
  const {
    control: valueDescControl,
    handleSubmit: handleValueDescSubmit,
    setValue: setValueDescValue,
  } = useForm({
    resolver: zodResolver(valueDescSchema),
    defaultValues: { value_desc: "" },
  });

  // Form handling of icon and color of widget
  const {
    handleSubmit: handleAppearanceSubmit,
    setValue: setAppearanceValue,
    watch: watchAppearance,
  } = useForm({
    resolver: zodResolver(appearanceSchema),
    defaultValues: { icon: "HelpCircle", color: "#888888" },
  });
  const currentAppearance = watchAppearance();

  // Get widget data
  const { data: widget, isLoading: widgetLoading } = useQuery({
    queryKey: ["widget", widgetId],
    queryFn: () => getWidget(widgetId),
  });

  // Get historic values and timestamps, use polling
  const { data: widgetData, isLoading: dataLoading } = useQuery({
    queryKey: ["widget-data", widgetId],
    queryFn: () => getWidgetData(widgetId),
    refetchInterval: widget?.polling_interval || 5000,
    enabled: !!widget?.topic_id,
  });

  // Get all devices to shoe in a  combobox
  const { data: devices, isLoading: devicesLoading } = useQuery({
    queryKey: ["devices"],
    queryFn: () => getDevices(null),
  });

  // Get all device topics for the chosen device, if no topic return []
  const { data: topics, isLoading: topicsLoading } = useQuery({
    queryKey: ["device-topics", widget?.device_id],
    queryFn: () =>
      widget?.device_id ? fetchTopics(widget.device_id) : Promise.resolve([]),
    enabled: !!widget?.device_id,
  });

  // Get all device commands for chosen topic, if no topic return []
  const { data: commands, isLoading: commandsLoading } = useQuery({
    queryKey: ["device-commands", widget?.device_id],
    queryFn: () =>
      widget?.device_id ? fetchCommands(widget.device_id) : Promise.resolve([]),
    enabled: !!widget?.device_id,
  });

  // Get widget groups for combobox options
  const { data: groups, isLoading: groupsLoading } = useQuery({
    queryKey: ["widget-groups"],
    queryFn: getGroups,
  });

  // useTimeAgo dynamically updates "Last updated..." message
  const history = widgetData?.history || [];
  const lastTimestamp = history[history.length - 1]?.timestamp;
  const timeAgo = useTimeAgo(lastTimestamp);

  // Sync BE values with local state
  useEffect(() => {
    if (widget) {
      setValueDescValue("value_desc", widget.value_desc || "");
      setAppearanceValue("icon", widget.icon || "HelpCircle");
      setAppearanceValue("color", widget.color || "#888888");
    }
  }, [widget, setValueDescValue, setAppearanceValue]);

  // Function for sending a PATCH request to BE
  const updateMutation = useMutation({
    mutationFn: (data: WidgetFormValues) => updateWidget(widgetId, data),
    onMutate: async (newData) => {
      await queryClient.cancelQueries({ queryKey: ["widget", widgetId] });
      const previous = queryClient.getQueryData(["widget", widgetId]);
      // Optimistic update to see results immediately
      queryClient.setQueryData(["widget", widgetId], (old: any) => ({
        ...old,
        ...newData,
      }));
      return { previous };
    },
    onError: (_, __, context) => {
      // Fallback for optimistic update, revert changes
      if (context?.previous)
        queryClient.setQueryData(["widget", widgetId], context.previous);
    },
    onSettled: () => {
      // Invalidate previous results to get actual data
      queryClient.invalidateQueries({ queryKey: ["widget", widgetId] });
      queryClient.invalidateQueries({ queryKey: ["widgets"] });
      queryClient.invalidateQueries({ queryKey: ["widget-data", widgetId] });
    },
  });

  // Function to send a POST request to BE
  const runCommandMutation = useMutation({
    mutationFn: (cmdId: number) => runCommand(cmdId),
    onMutate: (cmdId) => setRunningCommandId(cmdId),
    onSuccess: () => {
      // Invalidate chart and widget data
      queryClient.invalidateQueries({ queryKey: ["widget-data", widgetId] });
      queryClient.invalidateQueries({ queryKey: ["widget", widgetId] });
      // Show a toast message for 3s
      setCommandSuccess("Command executed successfully");
      setTimeout(() => setCommandSuccess(null), 3000);
    },
    onSettled: () => setRunningCommandId(null),
  });

  // Handler from any form to envoke mutation
  const handleUpdate = (field: string, value: any) =>
    updateMutation.mutateAsync({ [field]: value });

  // Handler from inline edit to enabled mutation
  const handleDescriptionSubmit = handleValueDescSubmit((data) => {
    updateMutation.mutateAsync({ value_desc: data.value_desc || "" });
    setEditingDesc(false);
  });

  // Show a spinner initially
  const isLoading = widgetLoading || devicesLoading || groupsLoading;
  if (isLoading || !widget || commandsLoading || topicsLoading) {
    return (
      <View style={styles.loader}>
        <ActivityIndicator size="large" color={appColors.primary} />
      </View>
    );
  }

  // Find icon and format diplayed value
  const IconComp = getIconComponent(widget.icon || "HelpCircle");
  const displayValue =
    history[history.length - 1]?.value !== null &&
    history[history.length - 1]?.value !== undefined
      ? history[history.length - 1].value.toFixed(1) +
        " " +
        (widget.value_desc ?? "-")
      : "-";

  // Filter out commands by chosen topic
  const filteredCommands = commands?.filter(
    (cmd) => cmd.topic_id === widget.topic_id,
  );

  return (
    <KeyboardAvoidingView
      behavior={Platform.OS === "ios" ? "padding" : "height"}
      style={{ flex: 1, backgroundColor: "#fff" }}
    >
      <BottomSheetModalProvider>
        <Stack.Screen
          options={{
            title: widget.name,
          }}
        />
        <ScrollView contentContainerStyle={{ padding: 16 }}>
          <View style={styles.headerRow}>
            <TouchableOpacity onPress={() => bottomSheetRef.current?.present()}>
              <View
                style={[
                  styles.iconLarge,
                  { backgroundColor: widget.color || appColors.gray },
                ]}
              >
                <IconComp color="#fff" size={40} />
                <View style={styles.editBadge}>
                  <Edit2 color="#fff" size={12} />
                </View>
              </View>
            </TouchableOpacity>
            <View style={{ marginLeft: 16 }}>
              <Text style={styles.headerValue}>{displayValue}</Text>
              <Text style={styles.headerUpdated}>Last updated {timeAgo}</Text>
            </View>
          </View>

          <WidgetChart
            data={widgetData}
            color={widget.color || appColors.primary}
            loading={dataLoading}
          />

          <Collapsible title="Settings">
            <View style={styles.section}>
              <Text style={[styles.sectionTitle, { marginBottom: 8 }]}>
                Device
              </Text>

              <SelectionSheet
                label=""
                placeholder="Select device"
                items={
                  devices?.map((d) => ({ label: d.name, value: d.id })) ?? []
                }
                value={widget.device_id}
                onSelect={(val) => handleUpdate("device_id", val)}
              />
            </View>

            <View style={styles.section}>
              <View style={{ flexDirection: "row", alignItems: "flex-start" }}>
                <View style={{ flex: 1, marginRight: 16 }}>
                  <Text style={[styles.sectionTitle, { marginBottom: 8 }]}>
                    Topic
                  </Text>
                  <SelectionSheet
                    label=""
                    placeholder="Select topic"
                    items={
                      topics?.map((t) => ({ label: t.name, value: t.id })) ?? []
                    }
                    value={widget.topic_id}
                    onSelect={(val) => handleUpdate("topic_id", val)}
                    disabled={!widget.device_id}
                  />
                </View>

                <View style={{ width: 100 }}>
                  <Text style={[styles.sectionTitle, { marginBottom: 8 }]}>
                    Description
                  </Text>
                  {editingDesc ? (
                    <View style={styles.descEditContainer}>
                      <Controller
                        control={valueDescControl}
                        name="value_desc"
                        render={({ field: { onChange, value } }) => (
                          <TextInput
                            style={styles.descInput}
                            autoFocus
                            value={value}
                            onChangeText={onChange}
                            onSubmitEditing={handleDescriptionSubmit}
                          />
                        )}
                      />
                      <TouchableOpacity
                        onPress={handleDescriptionSubmit}
                        hitSlop={HIT_SLOP}
                        style={styles.iconButton}
                      >
                        <Check size={20} color={appColors.primary} />
                      </TouchableOpacity>
                    </View>
                  ) : (
                    <TouchableOpacity
                      onLongPress={() => setEditingDesc(true)}
                      style={styles.descRow}
                    >
                      <Text style={styles.descText}>
                        {widget.value_desc || "-"}
                      </Text>
                    </TouchableOpacity>
                  )}
                </View>
              </View>
            </View>

            <View style={styles.section}>
              <Text style={[styles.sectionTitle, { marginBottom: 8 }]}>
                Group
              </Text>
              <SelectionSheet
                label=""
                placeholder="Select Group"
                items={
                  groups?.map((g) => ({ label: g.name, value: g.id })) ?? []
                }
                value={widget.group_id}
                onSelect={(val) => handleUpdate("group_id", val)}
              />
            </View>

            <View style={styles.section}>
              <PollingSlider
                initialValue={widget.polling_interval || 5000}
                onUpdate={(val) => handleUpdate("polling_interval", val)}
              />
            </View>
          </Collapsible>

          <View style={styles.section}>
            <View style={styles.sectionHeaderRow}>
              <Text style={styles.sectionTitle}>Commands</Text>
              {widget.device_id && (
                <TouchableOpacity
                  style={styles.manageLink}
                  onPress={() => router.push(`/devices/${widget.device_id}`)}
                >
                  <Text style={styles.manageLinkText}>Manage</Text>
                  <ChevronRight size={16} color={appColors.primary} />
                </TouchableOpacity>
              )}
            </View>
            {filteredCommands?.map((cmd) => (
              <View key={cmd.id} style={styles.commandRow}>
                <Text style={styles.command}>{cmd.name}</Text>
                <TouchableOpacity
                  onPress={() => runCommandMutation.mutateAsync(cmd.id)}
                  disabled={runningCommandId === cmd.id}
                >
                  {runningCommandId === cmd.id ? (
                    <ActivityIndicator color={appColors.primary} size={24} />
                  ) : (
                    <Play size={24} color={appColors.primary} />
                  )}
                </TouchableOpacity>
              </View>
            ))}
            {(!commands || commands.length === 0) && (
              <Text style={{ color: appColors.gray }}>
                No commands available
              </Text>
            )}
          </View>
        </ScrollView>

        <Toast
          visible={!!commandSuccess}
          message={commandSuccess || ""}
          type="success"
        />

        <AppearanceSheet
          bottomSheetRef={bottomSheetRef}
          currentAppearance={currentAppearance}
          iconSearch={iconSearch}
          setIconSearch={setIconSearch}
          setValue={setAppearanceValue}
          onSave={handleAppearanceSubmit((data) => {
            updateMutation.mutateAsync(data);
            bottomSheetRef.current?.dismiss();
          })}
        />
      </BottomSheetModalProvider>
    </KeyboardAvoidingView>
  );
}

const styles = StyleSheet.create({
  loader: {
    flex: 1,
    justifyContent: "center",
    alignItems: "center",
  },
  headerRow: {
    flexDirection: "row",
    alignItems: "center",
    marginBottom: 24,
  },
  iconLarge: {
    width: 80,
    height: 80,
    borderRadius: 40,
    justifyContent: "center",
    alignItems: "center",
    position: "relative",
  },
  editBadge: {
    position: "absolute",
    bottom: 0,
    right: 0,
    backgroundColor: appColors.primary,
    borderRadius: 10,
    width: 20,
    height: 20,
    justifyContent: "center",
    alignItems: "center",
  },
  headerValue: {
    fontSize: 32,
    fontWeight: "bold",
  },
  headerUpdated: {
    fontSize: 14,
    color: appColors.gray,
    marginTop: 4,
  },
  section: {
    marginBottom: 8,
  },
  sectionHeaderRow: {
    flexDirection: "row",
    justifyContent: "space-between",
    alignItems: "center",
    marginBottom: 8,
  },
  sectionTitle: {
    fontSize: 16,
    fontWeight: "bold",
    color: appColors.darkGray,
  },
  manageLink: {
    flexDirection: "row",
    alignItems: "center",
  },
  manageLinkText: {
    color: appColors.primary,
    fontWeight: "600",
    fontSize: 14,
    marginRight: 2,
  },
  descRow: {
    flexDirection: "row",
    alignItems: "center",
    justifyContent: "flex-start",
    paddingVertical: 8,
    borderBottomWidth: 1,
    borderBottomColor: appColors.gray,
    borderStyle: "dotted",
  },
  descText: {
    fontSize: 16,
    fontWeight: "bold",
  },
  descEditContainer: {
    flexDirection: "row",
    alignItems: "center",
    borderBottomWidth: 1,
    borderBottomColor: appColors.primary,
    paddingVertical: 4,
  },
  descInput: {
    fontSize: 16,
    flex: 1,
    marginRight: 8,
  },
  commandRow: {
    flexDirection: "row",
    justifyContent: "space-between",
    alignItems: "center",
    paddingVertical: 12,
  },
  command: {
    fontSize: 16,
    fontWeight: "bold",
  },
  iconButton: {
    padding: 4,
    justifyContent: "center",
    alignItems: "center",
  },
});
