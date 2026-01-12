/**
 * Project: ITU
 * Authors: Dmitrii Ivanushkin and Oleh Krasovskyi
 * File: Type definitions.
 */
export interface WidgetGroup {
  id: number;
  name: string;
}

export interface Widget {
  id: number;
  name: string;
  icon?: string;
  color?: string;
  device_id?: number | null;
  topic_id?: number | null;
  group_id?: number | null;
  value_desc?: string;
  polling_interval?: number;
  history?: { value: number; timestamp: string }[];
  last_value?: number | null;
}

export interface WidgetData {
  widget_id: number;
  topic: string;
  last_value: number | null;
  history: { value: number; timestamp: string }[];
}

export interface DeviceGroupBase {
  name: string;
}

export interface DeviceGroupReadSchema extends DeviceGroupBase {
  id: number;
}

export type Device = {
  id: number;
  name: string;
  access_token: string;
  group: DeviceGroup;
};

export type DeviceGroup = {
  id: number;
  name: string;
};

export interface DeviceTopicBase {
  name: string;
  value?: number;
  value_desc?: string;
  device_id: number;
}

export interface DeviceTopicCreateSchema extends DeviceTopicBase {}

export interface DeviceTopicUpdateSchema {
  name?: string;
  value?: number;
  value_desc?: string;
}

export interface DeviceTopicReadSchema extends DeviceTopicBase {
  id: number;
}

export interface CommandBase {
  name: string;
  topic_id: number;
  device_id: number;
  value?: number | null;
}

export interface CommandReadSchema extends CommandBase {
  id: number;
}
