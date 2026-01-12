//created by xkrasoo00
// dscribes zod form constraints for device settings (name change)

import { z } from "zod";

export const deviceSettingsSchema = z.object({
  name: z.string().min(1, "Name is required"),
  group_id: z.number().nullable().optional(),
});

export type DeviceSettingsValues = z.infer<typeof deviceSettingsSchema>;
