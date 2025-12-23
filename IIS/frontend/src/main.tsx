/**
 * IIS Project
 * @brief Entry point
 * @author Dmitrii Ivanushkin
 */
import { StrictMode } from "react";
import { createRoot } from "react-dom/client";
import App from "./App.tsx";

createRoot(document.getElementById("root")!).render(
  <StrictMode>
    <App />
  </StrictMode>,
);
