/**
 * Project: ITU
 * Author: Dmitrii Ivanushkin
 * File: Widgets dashboard with groups management.
 * User can CRUD groups and CRUD widgets. Each widget card is a link to the details of this widget.
 */
import { useState } from "react";
import {
  View,
  Text,
  FlatList,
  TouchableWithoutFeedback,
  TouchableOpacity,
  ActivityIndicator,
  StyleSheet,
  KeyboardAvoidingView,
  Platform,
} from "react-native";
import { Plus, Check, LayoutDashboard } from "lucide-react-native";
import { useRouter } from "expo-router";
import {
  useQuery,
  useMutation,
  useQueryClient,
  useQueries,
} from "@tanstack/react-query";
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";

import { Widget, WidgetGroup } from "@/lib/api/types";
import { getGroups } from "@/lib/api/widgetGroups/getGroups";
import { createGroup } from "@/lib/api/widgetGroups/createGroup";
import { updateGroup } from "@/lib/api/widgetGroups/updateGroup";
import { deleteGroup } from "@/lib/api/widgetGroups/deleteGroup";
import { getWidgets } from "@/lib/api/widgets/getWidgets";
import { createWidget } from "@/lib/api/widgets/createWidget";
import { updateWidget } from "@/lib/api/widgets/updateWidget";
import { deleteWidget } from "@/lib/api/widgets/deleteWidget";
import { getWidgetData } from "@/lib/api/widgets/getWidgetData";
import { Toast } from "@/components/Toast";
import {
  WidgetFullFormValues,
  widgetSchema,
} from "@/lib/schemas/widgetSchemas";

import WidgetGroupTabs from "@/components/WidgetGroupTabs";
import WidgetCard from "@/components/WidgetCard";
import {
  groupSchema,
  WidgetGroupFormValues,
} from "@/lib/schemas/widgetGroupSchemas";
import { appColors } from "@/lib/constants/colors";

export default function WidgetsScreen() {
  // Navigation and Query Client
  const router = useRouter();
  const queryClient = useQueryClient();

  // Local state for UI modes and selection
  const [selectedGroup, setSelectedGroup] = useState<number | null>(null);
  const [editingGroupId, setEditingGroupId] = useState<number | null>(null);
  const [editingWidgetId, setEditingWidgetId] = useState<number | null>(null);

  // State for Action Modes (Edit/Delete buttons visible)
  const [groupActionModeId, setGroupActionModeId] = useState<number | null>(
    null,
  );
  const [widgetActionModeId, setWidgetActionModeId] = useState<number | null>(
    null,
  );

  // State for creation modes
  const [isCreatingGroup, setIsCreatingGroup] = useState(false);
  const [isCreatingWidget, setIsCreatingWidget] = useState(false);

  // State to track deleted widget for Undo functionality
  const [deletedWidget, setDeletedWidget] = useState<{
    id: number;
    data: Widget;
  } | null>(null);
  // Undo logic. While the time isn't expired user can revert destructive changes
  const [undoTimer, setUndoTimer] = useState<ReturnType<
    typeof setTimeout
  > | null>(null);

  // Form handling of creating/updating a group
  const {
    control: groupControl,
    handleSubmit: handleGroupSubmit,
    reset: resetGroupForm,
  } = useForm({
    resolver: zodResolver(groupSchema),
    defaultValues: { name: "" },
  });

  // Form handling of creating/updating a widget
  const {
    control: widgetControl,
    handleSubmit: handleWidgetSubmit,
    reset: resetWidgetForm,
  } = useForm({
    resolver: zodResolver(widgetSchema),
    defaultValues: { name: "" },
  });

  // Get all widget groups
  const { data: widgetGroups, isLoading: widgetGroupsLoading } = useQuery({
    queryKey: ["widget-groups"],
    queryFn: getGroups,
  });

  // Get all widgets
  const { data: allWidgets, isLoading: widgetsLoading } = useQuery({
    queryKey: ["widgets"],
    queryFn: getWidgets,
    refetchInterval: 5000,
  });

  // Widgets are filtered on FE to achieve smooth group change without needing to refetch
  const widgets = selectedGroup
    ? allWidgets?.filter((w) => w.group_id === selectedGroup)
    : allWidgets;

  // Get historic values, use polling
  const widgetDataQueries = useQueries({
    queries: (allWidgets ?? []).map((widget) => ({
      queryKey: ["widget-data", widget.id],
      queryFn: () => getWidgetData(widget.id),
      refetchInterval: widget.polling_interval || 5000,
      enabled: !!widget.topic_id,
    })),
  });

  // Get groupped widgets with actual last value
  const displayedWidgetsList = widgets?.map((w) => {
    const dataQuery = widgetDataQueries.find((q) => q.data?.widget_id === w.id);
    return dataQuery?.data
      ? { ...w, last_value: dataQuery.data.last_value }
      : w;
  });

  // Function to send a POST request to BE for group
  const createGroupMutation = useMutation({
    mutationFn: (data: WidgetGroupFormValues) => createGroup(data),
    onMutate: async (name) => {
      await queryClient.cancelQueries({ queryKey: ["widget-groups"] });
      const previous = queryClient.getQueryData(["widget-groups"]);
      // Optimistic update to see results immediately
      queryClient.setQueryData(["widget-groups"], (old: WidgetGroup[]) => [
        ...(old || []),
        { id: Date.now(), name, temp: true },
      ]);
      return { previous };
    },
    onError: (_, __, context) =>
      // Fallback for optimistic update, revert changes
      context?.previous &&
      queryClient.setQueryData(["widget-groups"], context.previous),
    onSettled: () =>
      // Invalidate previous results to get actual data
      queryClient.invalidateQueries({ queryKey: ["widget-groups"] }),
    onSuccess: (newGroup) => {
      // Sync with local state and reset form
      setSelectedGroup(newGroup.id);
      setIsCreatingGroup(false);
      resetGroupForm();
    },
  });

  // Function to send a PUT request to BE for group
  const updateGroupMutation = useMutation({
    mutationFn: ({ id, name }: { id: number; name: string }) =>
      updateGroup(id, { name }),
    onMutate: async ({ id, name }) => {
      await queryClient.cancelQueries({ queryKey: ["widget-groups"] });
      const previous = queryClient.getQueryData(["widget-groups"]);
      // Optimistic update to see results immediately
      queryClient.setQueryData(["widget-groups"], (old: WidgetGroup[]) =>
        old?.map((g) => (g.id === id ? { ...g, name } : g)),
      );
      return { previous };
    },
    onError: (_, __, context) =>
      // Fallback for optimistic update, revert changes
      context?.previous &&
      queryClient.setQueryData(["widget-groups"], context.previous),
    onSettled: () =>
      // Invalidate previous results to get actual data
      queryClient.invalidateQueries({ queryKey: ["widget-groups"] }),
  });

  // Function to send a DELETE request to BE for group
  const deleteGroupMutation = useMutation({
    mutationFn: (id: number) => deleteGroup(id),
    onMutate: async (id) => {
      await queryClient.cancelQueries({ queryKey: ["widget-groups"] });
      const previous = queryClient.getQueryData(["widget-groups"]);
      queryClient.setQueryData(["widget-groups"], (old: WidgetGroup[]) =>
        // Optimistic update to see results immediately
        old?.filter((g) => g.id !== id),
      );
      return { previous };
    },
    onError: (_, __, context) =>
      // Fallback for optimistic update, revert changes
      context?.previous &&
      queryClient.setQueryData(["widget-groups"], context.previous),
    onSettled: () =>
      // Invalidate previous results to get actual data
      queryClient.invalidateQueries({ queryKey: ["widget-groups"] }),
    onSuccess: (_, id) => {
      // Update local state
      if (selectedGroup === id) setSelectedGroup(null);
    },
  });

  // Function to send a POST request to BE for widget
  const createWidgetMutation = useMutation({
    mutationFn: (data: WidgetFullFormValues) => createWidget(data),
    onMutate: async (data) => {
      await queryClient.cancelQueries({ queryKey: ["widgets"] });
      const previous = queryClient.getQueryData(["widgets"]);
      // Create temp widget for optimistic update
      const tempWidget: Widget = {
        id: Date.now(),
        name: data.name,
        icon: data.icon,
        color: data.color,
        group_id: data.group_id,
        device_id: null,
        topic_id: null,
        last_value: null,
        history: [],
      };
      queryClient.setQueryData(["widgets"], (old: Widget[]) => [
        ...(old || []),
        tempWidget,
      ]);
      return { previous };
    },
    onError: (_, __, context) =>
      // Fallback for optimistic update
      context?.previous &&
      queryClient.setQueryData(["widgets"], context.previous),
    onSettled: () => {
      queryClient.invalidateQueries({ queryKey: ["widgets"] });
    },
    onSuccess: () => {
      resetWidgetForm();
      setIsCreatingWidget(false);
    },
  });

  // Function to send a PATCH request to BE for widget
  const updateWidgetMutation = useMutation({
    mutationFn: ({ id, data }: { id: number; data: WidgetFullFormValues }) =>
      updateWidget(id, data),
    onMutate: async ({ id, data }) => {
      await queryClient.cancelQueries({ queryKey: ["widgets"] });
      const previous = queryClient.getQueryData(["widgets"]);
      // Optimistic update to see results immediately
      queryClient.setQueryData(["widgets"], (old: Widget[]) =>
        old?.map((w) => (w.id === id ? { ...w, ...data } : w)),
      );
      return { previous };
    },
    onError: (_, __, context) =>
      context?.previous &&
      queryClient.setQueryData(["widgets"], context.previous),
    onSettled: () => queryClient.invalidateQueries({ queryKey: ["widgets"] }),
  });

  // Function to send a DELETE request to BE for widget
  const deleteWidgetMutation = useMutation({
    mutationFn: (id: number) => deleteWidget(id),
    onMutate: async (id) => {
      await queryClient.cancelQueries({ queryKey: ["widgets"] });
      const previous = queryClient.getQueryData(["widgets"]);
      // Optimistic update
      queryClient.setQueryData(["widgets"], (old: Widget[]) =>
        old?.filter((w) => w.id !== id),
      );
      return { previous };
    },
    onError: (_, __, context) =>
      context?.previous &&
      queryClient.setQueryData(["widgets"], context.previous),
    onSettled: () => queryClient.invalidateQueries({ queryKey: ["widgets"] }),
  });

  // Custom handler to support Undo operation. It delays the API call
  // and temporarily stores the widget to be deleted.
  const handleDeleteWithUndo = (widget: Widget) => {
    if (undoTimer) clearTimeout(undoTimer);
    // If a deletion was already pending, execute it immediately
    if (deletedWidget) deleteWidgetMutation.mutateAsync(deletedWidget.id);

    setDeletedWidget({ id: widget.id, data: widget });
    // Set timeout for actual deletion
    const timer = setTimeout(() => {
      deleteWidgetMutation.mutateAsync(widget.id);
      setDeletedWidget(null);
      setUndoTimer(null);
    }, 3000);
    setUndoTimer(timer);
  };

  // Restores the widget scheduled for deletion
  const handleUndo = () => {
    if (undoTimer) clearTimeout(undoTimer);
    setDeletedWidget(null);
    setUndoTimer(null);
  };

  // Reset all local states to exit edit/create modes
  const cancelEditing = () => {
    setEditingGroupId(null);
    setEditingWidgetId(null);
    setGroupActionModeId(null);
    setWidgetActionModeId(null);
    setIsCreatingGroup(false);
    setIsCreatingWidget(false);
    resetGroupForm();
    resetWidgetForm();
  };

  // Show a spinner initially
  if (widgetGroupsLoading || widgetsLoading) {
    return (
      <View style={styles.loader}>
        <ActivityIndicator size="large" color={appColors.primary} />
      </View>
    );
  }

  // Filter out widget that is currently in "deleted" state awaiting undo
  const displayedWidgets = (displayedWidgetsList ?? []).filter(
    (w) => w.id !== deletedWidget?.id,
  );

  // Check if current view is empty
  const isEmpty =
    displayedWidgets.length === 0 && !isCreatingWidget && !widgetsLoading;

  return (
    <KeyboardAvoidingView
      behavior={Platform.OS === "ios" ? "padding" : "height"}
      style={{ flex: 1 }}
    >
      {/* Clicking outside of cards/tabs should cancel editing mode */}
      <TouchableWithoutFeedback onPress={cancelEditing}>
        <View style={{ flex: 1, backgroundColor: "#f9f9f9" }}>
          <View style={styles.container}>
            <View style={{ flexDirection: "row", alignItems: "center" }}>
              <WidgetGroupTabs
                groups={widgetGroups || []}
                selectedGroup={selectedGroup}
                editingGroupId={editingGroupId}
                groupActionModeId={groupActionModeId}
                isCreatingGroup={isCreatingGroup}
                control={groupControl}
                onSelect={setSelectedGroup}
                onLongPress={(id) => {
                  setGroupActionModeId(id);
                  setEditingGroupId(null);
                }}
                onEditStart={(id) => {
                  setEditingGroupId(id);
                  setGroupActionModeId(null);
                }}
                onDelete={(id) => {
                  deleteGroupMutation.mutateAsync(id);
                  setGroupActionModeId(null);
                }}
                onSubmitCreate={handleGroupSubmit((data) =>
                  createGroupMutation.mutateAsync({ name: data.name }),
                )}
                onSubmitUpdate={(id, name) => {
                  updateGroupMutation.mutateAsync({ id, name });
                  cancelEditing();
                }}
                onCreateStart={() => setIsCreatingGroup(true)}
              />
            </View>

            {isEmpty ? (
              <View style={styles.emptyState}>
                <View style={styles.emptyIconContainer}>
                  <LayoutDashboard color={appColors.gray} size={80} />
                </View>
                <Text style={styles.emptyTitle}>Nothing here yet</Text>
                <Text style={styles.emptySubtitle}>
                  Click on + icon to add new widget
                </Text>
              </View>
            ) : (
              <FlatList
                data={
                  // If creating, add a temp item to render the "new" card
                  isCreatingWidget && !createWidgetMutation.isPending
                    ? [
                        ...displayedWidgets,
                        { id: -1, temp: true } as Widget & { temp: boolean },
                      ]
                    : displayedWidgets
                }
                keyExtractor={(item) => item.id.toString()}
                numColumns={2}
                columnWrapperStyle={{ justifyContent: "space-between" }}
                renderItem={({ item }) => {
                  // Determine the state of the card (view, new, edit, action)
                  let mode: "new" | "view" | "edit" | "action" = "view";
                  if ((item as Widget & { temp: boolean }).temp) mode = "new";
                  else if (editingWidgetId === item.id) mode = "edit";
                  else if (widgetActionModeId === item.id) mode = "action";

                  return (
                    <View style={styles.widgetWrapper}>
                      <WidgetCard
                        item={item}
                        mode={mode}
                        control={widgetControl}
                        onPress={() => router.push(`/widgets/${item.id}`)}
                        onLongPress={() => setWidgetActionModeId(item.id)}
                        onEditStart={() => {
                          setEditingWidgetId(item.id);
                          setWidgetActionModeId(null);
                        }}
                        onDelete={() => {
                          handleDeleteWithUndo(item);
                          setWidgetActionModeId(null);
                        }}
                        onSubmitCreate={handleWidgetSubmit((data) => {
                          createWidgetMutation.mutateAsync({
                            name: data.name,
                            icon: "HelpCircle",
                            color: appColors.darkGray,
                            group_id: selectedGroup,
                          });
                        })}
                        onSubmitUpdate={(id, name) => {
                          updateWidgetMutation.mutateAsync({
                            id,
                            data: { name },
                          });
                          cancelEditing();
                        }}
                      />
                    </View>
                  );
                }}
              />
            )}
          </View>

          <Toast
            visible={!!deletedWidget}
            message="Widget deleted"
            type="info"
            actionLabel="Undo"
            onAction={handleUndo}
          />

          {!isCreatingWidget ? (
            <TouchableOpacity
              style={styles.fab}
              onPress={() => setIsCreatingWidget(true)}
            >
              <Plus color="#fff" size={32} />
            </TouchableOpacity>
          ) : (
            <TouchableOpacity
              style={[styles.fab, styles.fabGreen]}
              onPress={handleWidgetSubmit((data) => {
                createWidgetMutation.mutate({
                  name: data.name,
                  icon: "HelpCircle",
                  color: appColors.darkGray,
                  group_id: selectedGroup,
                });
              })}
            >
              <Check color="#fff" size={32} />
            </TouchableOpacity>
          )}
        </View>
      </TouchableWithoutFeedback>
    </KeyboardAvoidingView>
  );
}

const styles = StyleSheet.create({
  loader: {
    flex: 1,
    justifyContent: "center",
    alignItems: "center",
  },
  container: {
    padding: 16,
    flex: 1,
  },
  emptyState: {
    flex: 1,
    justifyContent: "center",
    alignItems: "center",
  },
  emptyIconContainer: {
    marginBottom: 10,
  },
  emptyTitle: {
    fontSize: 24,
    fontWeight: "bold",
    color: appColors.darkGray,
    marginBottom: 8,
  },
  emptySubtitle: {
    fontSize: 16,
    color: appColors.gray,
  },
  widgetWrapper: {
    width: "48%",
    marginBottom: 16,
  },
  fab: {
    position: "absolute",
    bottom: 24,
    right: 24,
    width: 64,
    height: 64,
    borderRadius: 32,
    backgroundColor: appColors.primary,
    justifyContent: "center",
    alignItems: "center",
  },
  fabGreen: {
    backgroundColor: appColors.success,
  },
});
