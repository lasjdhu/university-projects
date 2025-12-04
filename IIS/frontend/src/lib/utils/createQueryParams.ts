/**
 * IIS Project
 * @brief Creates query parameters for requests
 * @author Dmitrii Ivanushkin
 */
import type { QueryParams } from "../api/types";

export function createQueryParams(params: QueryParams): string {
  const query = new URLSearchParams(params);

  return `?${query.toString()}`;
}
