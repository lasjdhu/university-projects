//created by xkrasoo00 on December 11th, 2025
// describes zod form constraints for command

import { z } from "zod";

export const commandSchema = z.object({
  name: z.string().min(1, "Alias is required"),
  topic_id: z.number().min(1, "Topic is required"),
  device_id: z.number().min(1, "Device is required"),
  value: z.number().nullable(),
});

export type CommandFormValues = z.infer<typeof commandSchema>;
