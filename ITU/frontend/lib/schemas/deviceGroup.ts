//created by xkrasoo00 on November 1st, 2025
// describes zod form constraints for device group

import z from "zod";

export const groupFormSchema = z.object({
  name: z.string().min(1, "Group name cannot be empty"),
});

export type GroupFormValues = z.infer<typeof groupFormSchema>;
