/**
 * IIS Project
 * @brief Settings of react-query
 * @author Dmitrii Ivanushkin
 */
import { QueryClient } from "@tanstack/react-query";

export const queryClient = new QueryClient({
  defaultOptions: {
    queries: {
      retry: 2,
    },
  },
});
