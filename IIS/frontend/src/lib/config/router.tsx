/**
 * IIS Project
 * @brief Router setup
 * @author Dmitrii Ivanushkin
 */
import RootLayout from "@/components/RootLayout";
import Admin from "@/screens/admin";
import Home from "@/screens/home";
import Login from "@/screens/login";
import ErrorScreen from "@/screens/misc/ErrorScreen";
import Players from "@/screens/players";
import Player from "@/screens/players/[id]";
import Profile from "@/screens/profile";
import Signup from "@/screens/signup";
import Teams from "@/screens/teams";
import Team from "@/screens/teams/[id]";
import Tournaments from "@/screens/tournaments";
import Tournament from "@/screens/tournaments/[id]";
import { createBrowserRouter } from "react-router";

export const router = createBrowserRouter(
  [
    {
      path: "/",
      Component: RootLayout,
      children: [
        { index: true, Component: Home },
        { path: "login", Component: Login },
        { path: "signup", Component: Signup },
        { path: "admin", Component: Admin },
        { path: "profile", Component: Profile },
        {
          path: "tournaments",
          children: [
            { index: true, Component: Tournaments },
            { path: ":id", Component: Tournament },
          ],
        },
        {
          path: "teams",
          children: [
            { index: true, Component: Teams },
            { path: ":id", Component: Team },
          ],
        },
        {
          path: "players",
          children: [
            { index: true, Component: Players },
            { path: ":id", Component: Player },
          ],
        },
        {
          path: "*",
          Component: () => <ErrorScreen message="404 Page not found" />,
        },
      ],
    },
  ],
  { basename: import.meta.env.BASE_URL },
);
