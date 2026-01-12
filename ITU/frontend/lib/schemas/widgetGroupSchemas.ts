/**
 * Project: ITU
 * Author: Dmitrii Ivanushkin
 * File: Zod schemas for validating Widget Groups.
 */
import { z } from "zod";

export const groupSchema = z.object({
  name: z.string().min(1, "Name is required"),
});

export type WidgetGroupFormValues = z.infer<typeof groupSchema>;
