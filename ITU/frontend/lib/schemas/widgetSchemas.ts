/**
 * Project: ITU
 * Author: Dmitrii Ivanushkin
 * File: Zod schemas for validating Widget forms.
 */
import { z } from "zod";

export const widgetSchema = z.object({
  name: z.string().min(1, "Name is required"),
  group_id: z.number().nullable().optional(),
});

export const valueDescSchema = z.object({
  value_desc: z.string().optional(),
});

export const appearanceSchema = z.object({
  icon: z.string().optional(),
  color: z.string().optional(),
});

export const fullWidgetFormSchema = widgetSchema
  .extend(valueDescSchema.shape)
  .extend(appearanceSchema.shape);

// Partial schema for updates (PATCH requests)
export const widgetUpdateSchema = fullWidgetFormSchema.partial();

export type WidgetFullFormValues = z.infer<typeof fullWidgetFormSchema>;
export type WidgetFormValues = z.infer<typeof widgetUpdateSchema>;
