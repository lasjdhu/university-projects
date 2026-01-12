/*    created by xkrasoo00 on november 1st, 2025
 *
 *     This file describes device groups page and its modals
 *
 **/

import { useState, useEffect, useCallback } from "react";
import { useRouter } from "expo-router";
import { appColors } from "@/lib/constants/colors";
import {
  View,
  Text,
  ScrollView,
  TouchableOpacity,
  TouchableWithoutFeedback,
  TextInput,
  FlatList,
  StyleSheet,
  Pressable,
  Keyboard,
  Modal,
  ActivityIndicator,
} from "react-native";
import { Plus, Copy } from "lucide-react-native";
import * as Clipboard from "expo-clipboard";

import { getDevices } from "../../../lib/api/devices/getDevices";
import { createDevice } from "../../../lib/api/devices/createDevice";
import { deleteDevice } from "../../../lib/api/devices/deleteDevice";
import { getGroups } from "../../../lib/api/groups/getGroups";
import { createGroup } from "../../../lib/api/groups/createGroup";
import { deleteGroup } from "../../../lib/api/groups/deleteGroup";
import { updateGroup } from "../../../lib/api/groups/updateGroup";
import { useQuery, useQueryClient } from "@tanstack/react-query";
import { generateToken } from "@/lib/utils/generateRandomToken";

import { useForm, Controller } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import { deviceFormSchema, DeviceFormValues } from "@/lib/schemas/device";
import { GroupListItem } from "@/components/GroupListItem";
import { DeviceListItem } from "@/components/DeviceListItem";
import { Toast } from "@/components/Toast";

//UNDO infrastructure
export default function DevicesScreen() {
  const router = useRouter();
  const queryClient = useQueryClient();
  type PendingDelete =
    | {
        type: "device";
        item: any;
        timeoutId: number;
      }
    | {
        type: "group";
        item: any;
        timeoutId: number;
      };

  const [pendingDelete, setPendingDelete] = useState<PendingDelete | null>(
    null,
  );
  const [toastVisible, setToastVisible] = useState(false);

  const [activeGroupId, setActiveGroupId] = useState<number | null>(null);
  const [editingGroupId, setEditingGroupId] = useState<number | null>(null);
  const [isModalVisible, setIsModalVisible] = useState(false);
  const [generatedToken, setGeneratedToken] = useState(generateToken());

  //device create form
  const {
    control: deviceControl,
    handleSubmit: handleDeviceSubmit,
    setValue: setDeviceValue,
    reset: resetDeviceForm,
    formState: { errors: deviceErrors },
    getValues,
  } = useForm<DeviceFormValues>({
    resolver: zodResolver(deviceFormSchema),
    defaultValues: {
      name: "",
      access_token: generatedToken,
      group_id: null,
    },
  });

  //get groups
  const { data: deviceGroups, isLoading: deviceGroupsLoading } = useQuery({
    queryKey: ["device-groups"],
    queryFn: getGroups,
  });

  //get devices in the group
  const { data: devices, isLoading: devicesLoading } = useQuery({
    queryKey: ["devices", activeGroupId],
    queryFn: () => getDevices(activeGroupId),
    enabled: activeGroupId !== null,
  });

  useEffect(() => {
    setDeviceValue("access_token", generatedToken);
  }, [generatedToken, setDeviceValue]);

  useEffect(() => {
    setDeviceValue("group_id", activeGroupId);
  }, [activeGroupId, setDeviceValue]);

  useEffect(() => {
    if (deviceGroups && deviceGroups.length > 0 && activeGroupId === null) {
      setActiveGroupId(deviceGroups[0].id);
    }
  }, [deviceGroups, activeGroupId]);

  //UNDO infrastructure
  const startUndoableDelete = (
    type: "device" | "group",
    item: any,
    optimisticUpdate: () => void,
    commitDelete: () => Promise<void>,
  ) => {
    // Optimistic UI update
    optimisticUpdate();

    setToastVisible(true);

    const timeoutId = setTimeout(async () => {
      try {
        await commitDelete();
      } catch (e) {
        console.error("Delete failed:", e);
      } finally {
        setPendingDelete(null);
        setToastVisible(false);
      }
    }, 5000);

    setPendingDelete({ type, item, timeoutId });
  };

  const handleUndoDelete = () => {
    if (!pendingDelete) return;

    clearTimeout(pendingDelete.timeoutId);

    if (pendingDelete.type === "device") {
      queryClient.setQueryData(
        ["devices", activeGroupId],
        (old: any[] | undefined) =>
          old ? [pendingDelete.item, ...old] : [pendingDelete.item],
      );
    }

    if (pendingDelete.type === "group") {
      queryClient.setQueryData(["device-groups"], (old: any[] | undefined) =>
        old ? [pendingDelete.item, ...old] : [pendingDelete.item],
      );
    }

    setEditingGroupId(null);
    setPendingDelete(null);
    setToastVisible(false);
  };

  //group U (CRUD)
  const handleUpdateGroupSubmit = useCallback(
    async (groupId: number, newName: string) => {
      try {
        await updateGroup(groupId, { name: newName });
        setEditingGroupId(null);
      } catch (e) {
        console.error("Error updating group:", e);
      } finally {
        Keyboard.dismiss();
        queryClient.invalidateQueries({ queryKey: ["device-groups"] });
      }
    },
    [queryClient],
  );

  //group D (CRUD)
  const handleDeleteGroup = useCallback(
    (groupId: number) => {
      const group = deviceGroups?.find((g) => g.id === groupId);
      if (!group) return;

      startUndoableDelete(
        "group",
        group,
        () => {
          queryClient.setQueryData(
            ["device-groups"],
            (old: any[] | undefined) =>
              old?.filter((g) => g.id !== groupId) ?? [],
          );

          if (activeGroupId === groupId) {
            setActiveGroupId(null);
          }
        },
        async () => {
          await deleteGroup(groupId);
          queryClient.invalidateQueries({ queryKey: ["device-groups"] });
          queryClient.invalidateQueries({ queryKey: ["devices"] });
        },
      );
    },
    [deviceGroups, activeGroupId, queryClient],
  );

  //group C (CRUD)
  const handleAddGroup = async () => {
    try {
      const newGroup = await createGroup({ name: " " });
      setActiveGroupId(newGroup.id);
      setEditingGroupId(newGroup.id);
    } catch (e) {
      console.error("Error creating group:", e);
    } finally {
      await queryClient.invalidateQueries({ queryKey: ["device-groups"] });
    }
  };

  const handleCancelGroupEdit = async (groupId: number, name: string) => {
    try {
      if (name.trim() === "") await deleteGroup(groupId);
      if (activeGroupId === groupId) setActiveGroupId(null);
    } catch (e) {
      console.error("Error deleting group:", e);
    } finally {
      await queryClient.invalidateQueries({ queryKey: ["device-groups"] });
      setEditingGroupId(null);
    }
  };

  //device D (CRUD)
  const handleDeleteDevice = useCallback(
    (deviceId: number) => {
      const device = devices?.find((d) => d.id === deviceId);
      if (!device) return;

      startUndoableDelete(
        "device",
        device,
        () => {
          queryClient.setQueryData(
            ["devices", activeGroupId],
            (old: any[] | undefined) =>
              old?.filter((d) => d.id !== deviceId) ?? [],
          );
        },
        async () => {
          await deleteDevice(deviceId);
          queryClient.invalidateQueries({ queryKey: ["devices"] });
        },
      );
    },
    [queryClient, devices, activeGroupId],
  );

  //device C (CRUD) with zod constraints
  const onDeviceFormSubmit = async (data: DeviceFormValues) => {
    try {
      await createDevice({
        name: data.name,
        access_token: data.access_token,
        group_id: activeGroupId,
      });
      setIsModalVisible(false);
      resetDeviceForm();
      queryClient.invalidateQueries({ queryKey: ["devices"] });
    } catch (e) {
      console.error("Error creating device:", e);
    }
  };

  //raises modal
  const handleAddDevice = () => {
    const newToken = generateToken();
    setGeneratedToken(newToken);
    resetDeviceForm({
      name: "",
      access_token: newToken,
      group_id: activeGroupId,
    });
    setIsModalVisible(true);
  };

  const handleBackgroundPress = () => {
    if (deviceGroups && editingGroupId !== null) {
      const naGroup = deviceGroups.find((group) => group.id === editingGroupId);
      if (!naGroup) {
        return;
      }
      handleCancelGroupEdit(naGroup.id, naGroup.name);
    }
    setEditingGroupId(null);
    Keyboard.dismiss();
  };

  //loading...
  if (devicesLoading || deviceGroupsLoading) {
    return (
      <View style={[styles.container, styles.center]}>
        <ActivityIndicator size="large" color={appColors.primary} />
      </View>
    );
  }

  return (
    <Pressable style={styles.container} onPress={handleBackgroundPress}>
      <ScrollView
        horizontal
        showsHorizontalScrollIndicator={false}
        style={styles.groupScroll}
      >
        {deviceGroups?.map((g) => (
          <GroupListItem
            key={g.id}
            group={g}
            isActive={activeGroupId === g.id}
            isEditing={editingGroupId === g.id}
            onSelect={setActiveGroupId}
            onLongPress={(id) => setEditingGroupId(id)}
            onRename={setEditingGroupId}
            onDelete={handleDeleteGroup}
            onUpdateSubmit={handleUpdateGroupSubmit}
          />
        ))}
        <TouchableOpacity
          style={styles.groupAddButton}
          onPress={handleAddGroup}
        >
          <Plus size={22} color={appColors.primary} />
        </TouchableOpacity>
      </ScrollView>

      {activeGroupId && deviceGroups && (
        <Text style={styles.groupTitle}>
          Devices in group{" "}
          {deviceGroups.find((g) => g.id === activeGroupId)?.name || ""}
        </Text>
      )}
      {!(activeGroupId && deviceGroups) && (
        <Text style={styles.groupTitle}>
          Select a group at the top to start!
        </Text>
      )}
      <FlatList
        data={devices}
        keyExtractor={(item) => item.id.toString()}
        renderItem={({ item }) => (
          <DeviceListItem
            item={item}
            onPress={(id) => router.push(`/devices/${id}`)}
            onDelete={handleDeleteDevice}
          />
        )}
        contentContainerStyle={styles.deviceList}
      />

      <Toast
        visible={toastVisible}
        message={
          pendingDelete?.type === "group" ? "Group deleted" : "Device deleted"
        }
        actionLabel="Undo"
        onAction={handleUndoDelete}
      />

      { activeGroupId && deviceGroups && deviceGroups[0] && (
      <TouchableOpacity style={styles.fab} onPress={handleAddDevice}>
        <Plus size={28} color="#fff" />
      </TouchableOpacity>
      )}

      <Modal
        visible={isModalVisible}
        animationType="slide"
        transparent
        onRequestClose={() => setIsModalVisible(false)}
      >
        <TouchableWithoutFeedback onPress={() => setIsModalVisible(false)}>
          <View style={styles.modalOverlay}>
            <TouchableWithoutFeedback>
              <View style={styles.modalContainer}>
                <View style={styles.modalHandle} />
                <Text style={styles.modalLabel}>Device name</Text>

                <Controller
                  control={deviceControl}
                  name="name"
                  render={({ field: { onChange, onBlur, value } }) => (
                    <TextInput
                      value={value}
                      onBlur={onBlur}
                      onChangeText={onChange}
                      placeholder="Enter device name"
                      style={[
                        styles.input,
                        deviceErrors.name
                          ? styles.inputError
                          : styles.inputDefault,
                      ]}
                    />
                  )}
                />
                {deviceErrors.name && (
                  <Text style={styles.errorText}>
                    {deviceErrors.name.message}
                  </Text>
                )}

                <Text style={[styles.modalLabel, { marginTop: 16 }]}>
                  Autogenerated token
                </Text>
                <View style={styles.tokenRow}>
                  <Controller
                    control={deviceControl}
                    name="access_token"
                    render={({ field: { value } }) => (
                      <TextInput
                        value={value}
                        editable={false}
                        style={[
                          styles.input,
                          styles.inputDefault,
                          { flex: 1, color: "#999" },
                        ]}
                      />
                    )}
                  />
                  <TouchableOpacity
                    style={styles.copyButton}
                    onPress={() => {
                      getValues("access_token") &&
                        Clipboard.setStringAsync(getValues("access_token"));
                    }}
                  >
                    <Copy size={20} color="#333" />
                  </TouchableOpacity>
                </View>

                <TouchableOpacity
                  style={styles.doneButton}
                  onPress={handleDeviceSubmit(onDeviceFormSubmit)}
                >
                  <Text style={styles.doneText}>Done</Text>
                </TouchableOpacity>
              </View>
            </TouchableWithoutFeedback>
          </View>
        </TouchableWithoutFeedback>
      </Modal>
    </Pressable>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: "#fff",
    paddingTop: 0,
  },
  center: {
    justifyContent: "center",
    alignItems: "center",
  },
  groupScroll: {
    flexGrow: 0,
    borderBottomWidth: 1,
    borderColor: appColors.gray,
    height: 48,
    marginBottom: 10,
  },
  groupButton: {
    flexDirection: "row",
    alignItems: "center",
    paddingHorizontal: 12,
    marginHorizontal: 4,
    borderRightWidth: 1,
    borderColor: appColors.gray,
    height: "100%",
  },
  activeGroupButton: {
    borderBottomWidth: 2,
    borderColor: appColors.primary,
  },
  boldGroupText: {
    fontWeight: "700",
  },
  activeGroupText: {
    color: appColors.primary,
  },
  iconRow: {
    flexDirection: "row",
    alignItems: "center",
  },
  groupAddButton: {
    justifyContent: "center",
    alignItems: "center",
    paddingHorizontal: 10,
    borderLeftWidth: 1,
    borderColor: appColors.gray,
  },
  deviceList: {
    paddingHorizontal: 20,
    paddingBottom: 100,
  },
  deviceCard: {
    flexDirection: "row",
    alignItems: "center",
    justifyContent: "space-between",
    backgroundColor: "#E5E5E5",
    borderRadius: 12,
    padding: 16,
    marginVertical: 6,
  },
  deviceName: {
    fontSize: 16,
    fontWeight: "500",
  },
  deviceIcons: {
    flexDirection: "row",
    alignItems: "center",
  },
  fab: {
    position: "absolute",
    bottom: 16,
    right: 12,
    width: 56,
    height: 56,
    backgroundColor: appColors.primary,
    borderRadius: 28,
    justifyContent: "center",
    alignItems: "center",
  },
  modalOverlay: {
    flex: 1,
    justifyContent: "flex-end",
    backgroundColor: "rgba(0,0,0,0.3)",
  },
  modalContainer: {
    backgroundColor: "#fff",
    borderTopLeftRadius: 20,
    borderTopRightRadius: 20,
    padding: 20,
  },
  modalHandle: {
    width: 40,
    height: 4,
    backgroundColor: appColors.gray,
    borderRadius: 2,
    alignSelf: "center",
    marginBottom: 12,
  },
  modalLabel: {
    fontSize: 14,
    fontWeight: "600",
    color: "#333",
  },
  input: {
    borderWidth: 1,
    borderRadius: 8,
    padding: 10,
    marginTop: 6,
    fontSize: 14,
  },
  inputDefault: {
    borderColor: appColors.primary,
  },
  inputError: {
    borderColor: appColors.primary,
  },
  errorText: {
    color: appColors.error,
    fontSize: 12,
    marginTop: 4,
  },
  tokenRow: {
    flexDirection: "row",
    alignItems: "center",
  },
  copyButton: {
    marginLeft: 8,
    backgroundColor: "#f1f1f1",
    padding: 10,
    borderRadius: 8,
  },
  doneButton: {
    backgroundColor: appColors.primary,
    borderRadius: 10,
    paddingVertical: 12,
    alignItems: "center",
    marginTop: 24,
  },
  groupTitle: {
    fontSize: 16,
    fontWeight: "700",
    marginTop: 6,
    marginBottom: 8,
    marginLeft: 12,
  },
  doneText: {
    color: "#fff",
    fontWeight: "600",
    fontSize: 16,
  },
});
