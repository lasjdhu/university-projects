/**
 * IIS Project
 * @brief Context for storing the user data
 * @author Dmitrii Ivanushkin
 */
import React, { useEffect, useState } from "react";
import type { UserResponse } from "../api/types";
import { useQuery } from "@tanstack/react-query";
import { UserContext } from "./useUser";
import { getMe } from "../api/auth/getMe";

export const UserProvider = ({ children }: { children: React.ReactNode }) => {
  const [user, setUser] = useState<UserResponse | null>(null);

  const { data, isLoading } = useQuery({
    queryKey: ["me"],
    queryFn: getMe,
    retry: false,
  });

  useEffect(() => {
    if (data) setUser(data);
  }, [data]);

  return (
    <UserContext.Provider value={{ user, setUser, isLoading }}>
      {children}
    </UserContext.Provider>
  );
};
