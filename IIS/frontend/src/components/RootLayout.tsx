/**
 * IIS Project
 * @brief Layout of app
 * @author Dmitrii Ivanushkin
 */
import Footer from "@/components/Footer";
import Header from "@/components/Header";
import { Outlet } from "react-router";

export default function RootLayout() {
  return (
    <div className="flex flex-col min-h-screen">
      <Header />
      <Outlet />
      <Footer />
    </div>
  );
}
