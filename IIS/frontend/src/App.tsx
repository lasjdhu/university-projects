/**
 * IIS Project
 * @brief Provider definitions and Router init
 * @author Dmitrii Ivanushkin
 */
import { QueryClientProvider } from "@tanstack/react-query";
import { RouterProvider } from "react-router/dom";
import { queryClient } from "@/lib/config/queryClient";
import { router } from "@/lib/config/router";
import localizedFormat from "dayjs/plugin/localizedFormat";
import dayjs from "dayjs";
import Toaster from "./components/Toaster";
import { UserProvider } from "./lib/contexts/UserProvider";

function App() {
  dayjs.extend(localizedFormat);

  return (
    <QueryClientProvider client={queryClient}>
      <UserProvider>
        <RouterProvider router={router} />
      </UserProvider>
      <Toaster />
    </QueryClientProvider>
  );
}

export default App;
