/*
 *     created by xkrasoo00 on November 1st, 2025
 *
 *     this file describes device settings page, its topics, commands
 *     command editing modal is also described here
 *
 */

import { useLocalSearchParams } from "expo-router";
import { appColors } from "@/lib/constants/colors";
import {
  View,
  Text,
  Modal,
  ScrollView,
  TouchableOpacity,
  TextInput,
  StyleSheet,
  ActivityIndicator,
  Pressable,
  Keyboard,
} from "react-native";
import { useEffect, useState } from "react";
import { useQuery, useQueryClient } from "@tanstack/react-query";
import { Controller, useForm } from "react-hook-form";
import * as Clipboard from "expo-clipboard";
import { Pencil, Trash2, Plus, Check, Copy } from "lucide-react-native";

import { fetchDevice, updateDevice } from "@/lib/api/devices/updateDevice";
import {
  fetchTopics,
  createTopic,
  updateTopic,
  deleteTopic,
} from "@/lib/api/topics/topics";
import {
  fetchCommands,
  createCommand,
  updateCommand,
  deleteCommand,
} from "@/lib/api/commands/commands";

import { zodResolver } from "@hookform/resolvers/zod";
import {
  deviceSettingsSchema,
  DeviceSettingsValues,
} from "@/lib/schemas/deviceSettingsSchema";
import { Device, CommandReadSchema } from "@/lib/api/types";
import { commandSchema, CommandFormValues } from "@/lib/schemas/commandSchema";
import { Toast } from "@/components/Toast";

export default function DeviceSettingsScreen() {
  const { id } = useLocalSearchParams();
  const deviceId = Number(id);
  const queryClient = useQueryClient();

  //for UNDO
  type PendingDelete =
    | { type: "command"; item: CommandReadSchema; timeoutId: number }
    | { type: "topic"; item: any; timeoutId: number };

  const [pendingDelete, setPendingDelete] = useState<PendingDelete | null>(
    null,
  );
  const [toastVisible, setToastVisible] = useState(false);
  const [isCommandModalVisible, setIsCommandModalVisible] = useState(false);
  const [editingCommand, setEditingCommand] = useState<null | { id: number }>(
    null,
  );
  const [editingTopicId, setEditingTopicId] = useState<number | null>(null);
  const [editingTopicName, setEditingTopicName] = useState("");
  const [newTopicName, setNewTopicName] = useState("");
  const [selectedTopicId, setSelectedTopicId] = useState<number | null>(null);
  const [topicToDelete, setTopicToDelete] = useState<number | null>(null);

  // Queries
  const { data: device, isLoading: loadingDevice } = useQuery<Device>({
    queryKey: ["device", deviceId],
    queryFn: () => fetchDevice(deviceId),
  });
  const { data: topics } = useQuery({
    queryKey: ["device-topics", deviceId],
    queryFn: () => fetchTopics(deviceId),
  });
  const { data: commands } = useQuery({
    queryKey: ["device-commands", deviceId],
    queryFn: () => fetchCommands(deviceId),
  });

  // Device form
  const {
    control,
    handleSubmit,
    setValue,
    formState: { errors },
  } = useForm<DeviceSettingsValues>({
    resolver: zodResolver(deviceSettingsSchema),
    defaultValues: { name: "", group_id: null },
  });

  // Command form
  const {
    control: commandControl,
    handleSubmit: handleCommandSubmit,
    reset: resetCommandForm,
    formState: { errors: commandErrors },
  } = useForm<CommandFormValues>({
    resolver: zodResolver(commandSchema),
    defaultValues: {
      name: "",
      topic_id: selectedTopicId ?? 0,
      device_id: deviceId,
      value: null,
    },
  });

  useEffect(() => {
    if (device) setValue("name", device.name);
  }, [device, setValue]);

  // Initialize default selected topic
  useEffect(() => {
    if (topics && topics.length > 0 && selectedTopicId === null) {
      setSelectedTopicId(topics[0].id);
    }
  }, [topics, selectedTopicId]);

  //save device
  const handleSaveDevice = async (data: DeviceSettingsValues) => {
    try {
      await updateDevice(deviceId, {
        ...data,
        access_token: device ? device.access_token : "",
      });
      queryClient.invalidateQueries({ queryKey: ["device", deviceId] });
      queryClient.invalidateQueries({ queryKey: ["devices"] });
    } catch (e) {
      console.error("Error updating device:", e);
    }
  };

  //modal for create command
  const openNewCommandModal = () => {
    setEditingCommand(null);
    resetCommandForm({
      name: "",
      topic_id: selectedTopicId ?? 0,
      device_id: deviceId,
      value: null,
    });
    setIsCommandModalVisible(true);
  };

  //edit command
  const openEditCommandModal = (cmd: CommandReadSchema) => {
    setEditingCommand({ id: cmd.id });
    resetCommandForm({
      name: cmd.name,
      topic_id: cmd.topic_id,
      device_id: cmd.device_id,
      value: cmd.value ?? null,
    });
    setIsCommandModalVisible(true);
  };

  //UNDO infrastructure
  const startUndoableDelete = (
    type: "command" | "topic",
    item: any,
    optimisticUpdate: () => void,
    commitDelete: () => Promise<void>,
  ) => {
    cancelEditTopic();
    optimisticUpdate();
    setToastVisible(true);

    const timeoutId = setTimeout(async () => {
      try {
        await commitDelete();
      } catch (e) {
        console.error(`${type} delete failed:`, e);
      } finally {
        setPendingDelete(null);
        setToastVisible(false);
      }
    }, 5000);

    setPendingDelete({ type, item, timeoutId });
  };

  //if UNDO pressed
  const handleUndoDelete = () => {
    if (!pendingDelete) return;
    clearTimeout(pendingDelete.timeoutId);

    if (pendingDelete.type === "command") {
      queryClient.invalidateQueries({
        queryKey: ["device-commands", deviceId],
      });
    }

    if (pendingDelete.type === "topic") {
      queryClient.invalidateQueries({
        queryKey: ["device-topics", deviceId],
      });
      queryClient.invalidateQueries({
        queryKey: ["device-commands", deviceId],
      });
    }

    setPendingDelete(null);
    setEditingTopicId(null);
    setEditingTopicName("");
    setToastVisible(false);
  };

  const saveCommand = async (data: CommandFormValues) => {
    try {
      if (selectedTopicId === null) {
        console.error("No topic selected!");
        return;
      }
      const commandData: CommandFormValues = {
        name: data.name,
        topic_id: selectedTopicId,
        value: data.value ?? null,
        device_id: deviceId,
      };
      //created or alreaday existing?
      if (editingCommand) {
        await updateCommand(deviceId, editingCommand.id, commandData);
      } else {
        await createCommand(deviceId, commandData);
      }
      queryClient.invalidateQueries({
        queryKey: ["device-commands", deviceId],
      });
      setIsCommandModalVisible(false);
    } catch (e) {
      console.error("Error saving command:", e);
    }
  };

  const removeCommand = (command: CommandReadSchema) => {
    startUndoableDelete(
      "command",
      command,
      () => {
        queryClient.setQueryData(
          ["device-commands", deviceId],
          (old: CommandReadSchema[] | undefined) =>
            old?.filter((c) => c.id !== command.id) ?? [],
        );
      },
      async () => {
        await deleteCommand(command.id);
        queryClient.invalidateQueries({
          queryKey: ["device-commands", deviceId],
        });
      },
    );
  };

  const removeTopic = (topic: any) => {
    if(topic.id === selectedTopicId) {
      setSelectedTopicId(null);
    }
    startUndoableDelete(
      "topic",
      topic,
      () => {
        queryClient.setQueryData(
          ["device-topics", deviceId],
          (old: any[] | undefined) =>
            old?.filter((t) => t.id !== topic.id) ?? [],
        );
        queryClient.setQueryData(
          ["device-commands", deviceId],
          (old: CommandReadSchema[] | undefined) =>
            old?.filter((c) => c.topic_id !== topic.id) ?? [],
        );
      },
      async () => {
        await deleteTopic(deviceId, topic.id);
        queryClient.invalidateQueries({
          queryKey: ["device-topics", deviceId],
        });
        queryClient.invalidateQueries({
          queryKey: ["device-commands", deviceId],
        });
      },
    );
  };

  const saveEditedTopic = async (topicId: number, name: string) => {
    if (!name.trim()) return;
    try {
      await updateTopic(deviceId, topicId, { name: name.trim() });
      setEditingTopicId(null);
      setEditingTopicName("");
      queryClient.invalidateQueries({ queryKey: ["device-topics", deviceId] });
    } catch (e) {
      console.error("Error updating topic:", e);
    }
  };

  const addNewTopic = async () => {
    if (!newTopicName.trim()) {
      setEditingTopicId(null);
      return;
    }
    try {
      await createTopic(deviceId, {
        name: newTopicName.trim(),
        device_id: deviceId,
      });
      setNewTopicName("");
      setEditingTopicId(null);
      queryClient.invalidateQueries({ queryKey: ["device-topics", deviceId] });
    } catch (e) {
      console.error("Error creating topic:", e);
    }
  };

  const cancelEditTopic = () => {
    if (editingTopicId === -1) {
      setNewTopicName("");
    }
    setEditingTopicId(null);
    setEditingTopicName("");
    setTopicToDelete(null);
    Keyboard.dismiss();
  };

  if (loadingDevice) {
    return (
      <View style={[styles.center, { flex: 1 }]}>
        <ActivityIndicator size="large" color={appColors.primary} />
        <Text>Loading</Text>
      </View>
    );
  }

  return (
    <Pressable style={{ flex: 1 }} onPress={cancelEditTopic}>
      <ScrollView style={styles.container} keyboardShouldPersistTaps="handled">
        <Text style={styles.title}>{device?.name}</Text>

        <Text style={styles.label}>Device name</Text>
        <Controller
          control={control}
          name="name"
          render={({ field: { value, onChange } }) => (
            <TextInput
              value={value}
              onChangeText={onChange}
              style={[styles.input, errors.name && styles.inputError]}
              placeholder="Enter name"
            />
          )}
        />
        {errors.name && (
          <Text style={styles.errorText}>{errors.name.message}</Text>
        )}

        <Text style={[styles.label, { marginTop: 12 }]}>Access token</Text>
        <View style={{ flexDirection: "row", alignItems: "center", gap: 8 }}>
          <TextInput
            value={device?.access_token || ""}
            editable={false}
            style={[styles.input, { backgroundColor: "#f1f1f1", flex: 1 }]}
          />
          <TouchableOpacity
            onPress={() => {
              if (device?.access_token) {
                Clipboard.setStringAsync(device.access_token);
              }
            }}
          >
            <Copy size={20} color={appColors.primary} />
          </TouchableOpacity>
        </View>

        <TouchableOpacity
          style={styles.saveButton}
          onPress={handleSubmit(handleSaveDevice)}
        >
          <Text style={styles.saveButtonText}>Save</Text>
        </TouchableOpacity>

        <Text style={styles.sectionTitle}>Device topics</Text>
        <ScrollView
          horizontal
          showsHorizontalScrollIndicator={false}
          contentContainerStyle={styles.topicContainer}
        >
          {topics?.map((t) => {
            const isEditing = editingTopicId === t.id;
            const isSelected = selectedTopicId === t.id;

            return (
              <View
                key={t.id}
                style={{ flexDirection: "row", alignItems: "center", gap: 6 }}
              >
                <Pressable
                  onPress={(e) => {
                    e.stopPropagation();
                    setSelectedTopicId(t.id);
                  }}
                  style={[
                    styles.topicTag,
                    isSelected && { backgroundColor: appColors.secondary },
                  ]}
                  onLongPress={() => {
                    setEditingTopicId(t.id);
                    setEditingTopicName(t.name);
                    setNewTopicName("");
                    setTopicToDelete(t.id); // show delete icon
                  }}
                >
                  {isEditing ? (
                    <>
                      <TextInput
                        autoFocus
                        value={editingTopicName}
                        onChangeText={setEditingTopicName}
                        style={[
                          styles.input,
                          { paddingHorizontal: 8, minWidth: 60, fontSize: 13 },
                        ]}
                        onSubmitEditing={() =>
                          saveEditedTopic(t.id, editingTopicName)
                        }
                      />
                      <TouchableOpacity
                        onPress={() => saveEditedTopic(t.id, editingTopicName)}
                      >
                        <Check size={16} color={appColors.primary} />
                      </TouchableOpacity>
                    </>
                  ) : (
                    <Text style={{ fontSize: 13 }}>{t.name}</Text>
                  )}
                </Pressable>

                {topicToDelete === t.id && isEditing && (
                  <TouchableOpacity
                    onPress={() => removeTopic(t)}
                    style={{ paddingHorizontal: 4 }}
                  >
                    <Trash2 size={16} color={appColors.error} />
                  </TouchableOpacity>
                )}
              </View>
            );
          })}

          {editingTopicId === null && !newTopicName && (
            <TouchableOpacity
              onPress={(e) => {
                e.stopPropagation();
                setEditingTopicId(-1);
              }}
              style={{ padding: 4 }}
            >
              <Plus size={18} color={appColors.primary} />
            </TouchableOpacity>
          )}

          {editingTopicId === -1 && (
            <Pressable
              style={styles.topicTag}
              onPress={(e) => e.stopPropagation()}
            >
              <TextInput
                autoFocus
                value={newTopicName}
                placeholder="New topic"
                onChangeText={setNewTopicName}
                style={[
                  styles.input,
                  { paddingHorizontal: 8, minWidth: 60, fontSize: 13 },
                ]}
                onSubmitEditing={addNewTopic}
              />
              <TouchableOpacity onPress={addNewTopic}>
                <Check size={16} color={appColors.primary} />
              </TouchableOpacity>
            </Pressable>
          )}
        </ScrollView>

        <Text style={styles.sectionTitle}>
          Fast commands for{" "}
          {topics?.find((t) => t.id === selectedTopicId)?.name ||
            "your topics..."}
        </Text>
        {(commands ?? [])
          .filter(
            (cmd: CommandReadSchema) =>
              selectedTopicId !== null &&
              cmd.topic_id === selectedTopicId &&
              cmd.id != null,
          )
          .map((cmd: CommandReadSchema) => (
            <View
              key={cmd.id}
              style={{
                flexDirection: "row",
                alignItems: "center",
                marginBottom: 10,
              }}
            >
              <TouchableOpacity
                style={{
                  flexDirection: "row",
                  alignItems: "center",
                  justifyContent: "space-between",
                  backgroundColor: "#E5E5E5",
                  padding: 16,
                  borderRadius: 12,
                  flex: 1,
                }}
                onPress={() => openEditCommandModal(cmd)}
                activeOpacity={0.7}
              >
                <Text style={styles.commandText}>{cmd.name}</Text>
                <Pencil size={20} color="#333" />
              </TouchableOpacity>

              <TouchableOpacity
                style={{ marginLeft: 12 }}
                onPress={() => removeCommand(cmd)}
              >
                <Trash2 size={20} color={appColors.error} />
              </TouchableOpacity>
            </View>
          ))}

        {selectedTopicId !== null && (
          <TouchableOpacity
            style={styles.addCommandButton}
            onPress={openNewCommandModal}
          >
            <Plus size={22} color={appColors.primary} />
            <Text style={styles.addCommandText}>Add command</Text>
          </TouchableOpacity>
        )}

        <Modal
          transparent
          visible={isCommandModalVisible}
          animationType="slide"
        >
          <Pressable style={styles.modalOverlay} onPress={Keyboard.dismiss}>
            <View style={styles.modalBody}>
              <View style={styles.modalHandle} />
              <Text style={styles.modalTitle}>
                {editingCommand ? "Edit command" : "Add command"}
              </Text>

              <Text style={styles.label}>Command alias</Text>
              <Controller
                control={commandControl}
                name="name"
                render={({ field: { value, onChange } }) => (
                  <TextInput
                    value={value}
                    onChangeText={onChange}
                    style={[
                      styles.input,
                      commandErrors.name && styles.inputError,
                    ]}
                    placeholder="Alias"
                  />
                )}
              />
              {commandErrors.name && (
                <Text style={styles.inputError}>
                  {commandErrors.name.message}
                </Text>
              )}

              <Text style={[styles.label, { marginTop: 12 }]}>Value</Text>
              <Controller
                control={commandControl}
                name="value"
                render={({ field: { value, onChange } }) => (
                  <TextInput
                    value={value?.toString() || ""}
                    onChangeText={(v) => onChange(v ? Number(v) : null)}
                    placeholder="Value"
                    style={styles.input}
                    keyboardType="numeric"
                  />
                )}
              />

              <TouchableOpacity
                style={styles.doneButton}
                onPress={handleCommandSubmit(saveCommand)}
              >
                <Text style={styles.doneText}>Done</Text>
              </TouchableOpacity>

              <TouchableOpacity
                style={styles.cancelButton}
                onPress={() => setIsCommandModalVisible(false)}
              >
                <Text>Cancel</Text>
              </TouchableOpacity>
            </View>
          </Pressable>
        </Modal>
      </ScrollView>
      <Toast
        visible={toastVisible}
        message={
          pendingDelete?.type === "topic" ? "Topic deleted" : "Command deleted"
        }
        actionLabel="Undo"
        onAction={handleUndoDelete}
      />
    </Pressable>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    padding: 20,
    backgroundColor: "#fff",
  },
  center: {
    justifyContent: "center",
    alignItems: "center",
  },
  title: {
    fontSize: 22,
    fontWeight: "700",
    marginBottom: 20,
  },
  label: {
    fontSize: 14,
    fontWeight: "600",
    marginBottom: 4,
  },
  input: {
    borderWidth: 1,
    borderRadius: 8,
    padding: 6,
    fontSize: 14,
    borderColor: appColors.gray,
  },
  inputError: {
    borderColor: appColors.error,
  },
  saveButton: {
    backgroundColor: appColors.primary,
    paddingVertical: 12,
    borderRadius: 10,
    marginTop: 16,
    alignItems: "center",
  },
  saveButtonText: { color: "#fff", fontWeight: "600" },
  sectionTitle: {
    fontSize: 16,
    fontWeight: "700",
    marginTop: 25,
    marginBottom: 8,
  },
  topicContainer: {
    flexDirection: "row",
    alignItems: "center",
    gap: 6,
  },
  topicTag: {
    flexDirection: "row",
    alignItems: "center",
    backgroundColor: "#E5E5E5",
    paddingHorizontal: 10,
    paddingVertical: 5,
    borderRadius: 12,
    marginRight: 6,
    gap: 6,
  },
  commandText: {
    fontSize: 16,
  },
  addCommandButton: {
    flexDirection: "row",
    alignItems: "center",
    gap: 8,
    marginTop: 12,
  },
  addCommandText: {
    fontSize: 15,
    color: appColors.primary,
  },
  modalOverlay: {
    flex: 1,
    justifyContent: "flex-end",
    backgroundColor: "rgba(0,0,0,0.3)",
  },
  modalBody: {
    backgroundColor: "#fff",
    padding: 20,
    borderTopLeftRadius: 20,
    borderTopRightRadius: 20,
  },
  modalHandle: {
    width: 40,
    height: 4,
    backgroundColor: appColors.gray,
    borderRadius: 2,
    alignSelf: "center",
    marginBottom: 12,
  },
  modalTitle: {
    fontSize: 18,
    fontWeight: "700",
    marginBottom: 16,
  },
  doneButton: {
    backgroundColor: appColors.primary,
    paddingVertical: 12,
    borderRadius: 10,
    alignItems: "center",
    marginTop: 24,
  },
  doneText: {
    color: "#fff",
    fontSize: 16,
    fontWeight: "600",
  },
  cancelButton: {
    marginTop: 16,
    alignSelf: "center",
  },
  errorText: {
    color: appColors.error,
    fontSize: 12,
    marginTop: 4,
  },
});
