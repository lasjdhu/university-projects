/**
 * IIS Project
 * @brief Toasts setup
 * @author Dmitrii Ivanushkin
 */
import { Toaster as RHToaster } from "react-hot-toast";

export default function Toaster() {
  return (
    <RHToaster
      position="bottom-right"
      reverseOrder={false}
      toastOptions={{
        style: {
          color: "#fff",
        },
        success: {
          style: {
            background: "#16a34a",
          },
        },
        error: {
          style: {
            background: "#dc2626",
          },
        },
        duration: 10000,
      }}
    />
  );
}
