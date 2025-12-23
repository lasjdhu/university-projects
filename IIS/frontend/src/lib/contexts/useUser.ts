/**
 * IIS Project
 * @brief Hook for retrieving the user data
 * @author Dmitrii Ivanushkin
 */
import { createContext, useContext } from "react";
import type { UserResponse } from "../api/types";

interface UserContextType {
  user: UserResponse | null;
  setUser: React.Dispatch<React.SetStateAction<UserResponse | null>>;
  isLoading: boolean;
}

export const UserContext = createContext<UserContextType | undefined>(
  undefined,
);

export const useUser = () => {
  const ctx = useContext(UserContext);
  if (!ctx) throw new Error("useUser must be used within a UserProvider");
  return ctx;
};
