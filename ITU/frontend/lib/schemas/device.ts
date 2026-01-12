//created by xkrasoo00 on November 1st, 2025
// describes zod form constraints for device

import z from "zod";

export const deviceFormSchema = z.object({
  name: z.string().min(1, "Device name is required"),
  access_token: z.string().min(1, "Token is required"),
  group_id: z.number().nullable().optional(),
});

export type DeviceFormValues = z.infer<typeof deviceFormSchema>;
