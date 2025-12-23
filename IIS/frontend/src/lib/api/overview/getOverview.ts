/**
 * IIS Project
 * @brief Endpoint call definition
 * @author Dmitrii Ivanushkin
 */
import { api } from "../client";
import type { OverviewResponse } from "../types";

export async function getOverview() {
  const res = await api.get<OverviewResponse>("/overview");

  return res.data;
}
