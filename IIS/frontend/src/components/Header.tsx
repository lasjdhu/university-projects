/**
 * IIS Project
 * @brief Navigation, logo, auth buttons
 * @author Dmitrii Ivanushkin
 */
import { useState } from "react";
import { Link, useNavigate } from "react-router";
import { Menu, X, LogOut, User } from "lucide-react";
import { logout } from "@/lib/api/auth/logout";
import { useMutation } from "@tanstack/react-query";
import toast from "react-hot-toast";
import { useUser } from "@/lib/contexts/useUser";

export default function Header() {
  const navigate = useNavigate();
  const [isOpen, setIsOpen] = useState(false);
  const { user, setUser, isLoading } = useUser();

  const { mutate: loginMutate } = useMutation({
    mutationFn: logout,
    onSuccess: (data) => {
      setUser(null);
      toast.success(data.message);
      navigate("/");
    },
  });

  return (
    <header className="fixed top-0 left-0 w-full bg-white shadow-sm z-50">
      <div className="flex items-center justify-between px-6 md:px-24 h-16">
        <Link to="/" className="text-2xl font-black">
          <span className="text-primary-500">FIT</span>ourney
        </Link>

        <nav className="hidden md:flex items-center gap-8">
          <Link to="/tournaments" className="link">
            Tournaments
          </Link>
          <Link to="/teams" className="link">
            Teams
          </Link>
          <Link to="/players" className="link">
            Players
          </Link>
          {user?.role === "Admin" && (
            <Link to="/admin" className="link">
              Admin panel
            </Link>
          )}
        </nav>

        <div className="hidden md:flex items-center gap-3">
          {isLoading ? (
            <></>
          ) : user ? (
            <>
              <Link to="/profile" className="flex items-center gap-2 link">
                <User className="h-4 w-4" />
                <span>
                  {user.name} {user.surname}
                </span>
              </Link>

              <button
                className="btn-ghost flex items-center gap-1"
                onClick={() => loginMutate()}
              >
                <LogOut className="h-4 w-4" /> Logout
              </button>
            </>
          ) : (
            <>
              <Link to="/login" className="btn-ghost">
                Log in
              </Link>
              <Link to="/signup" className="btn-primary">
                Sign up
              </Link>
            </>
          )}
        </div>

        <button
          className="md:hidden p-2 rounded-md hover:bg-gray-100 transition"
          onClick={() => setIsOpen((prev) => !prev)}
          aria-label="Toggle menu"
        >
          {isOpen ? <X className="h-6 w-6" /> : <Menu className="h-6 w-6" />}
        </button>
      </div>

      <div
        className={`absolute top-16 left-0 w-full transition-all duration-300 md:hidden ${
          isOpen
            ? "max-h-screen opacity-100"
            : "max-h-0 opacity-0 pointer-events-none"
        }`}
      >
        <div
          className="absolute inset-0 bg-black/40 backdrop-blur-[1px]"
          onClick={() => setIsOpen(false)}
        />

        <div
          className="relative bg-white shadow-lg w-full p-6 flex flex-col gap-4"
          onClick={(e) => e.stopPropagation()}
        >
          <nav className="flex flex-col items-start gap-4">
            <Link
              to="/tournaments"
              className="link w-full"
              onClick={() => setIsOpen(false)}
            >
              Tournaments
            </Link>
            <Link
              to="/teams"
              className="link w-full"
              onClick={() => setIsOpen(false)}
            >
              Teams
            </Link>
            <Link
              to="/players"
              className="link w-full"
              onClick={() => setIsOpen(false)}
            >
              Players
            </Link>
            {user?.role === "Admin" && (
              <Link
                to="/admin"
                className="link w-full"
                onClick={() => setIsOpen(false)}
              >
                Admin panel
              </Link>
            )}
          </nav>

          <div className="flex w-full justify-center gap-3 pt-4 border-t border-gray-100">
            {isLoading ? (
              <></>
            ) : user ? (
              <div className="flex flex-col gap-2 pt-4">
                <Link
                  to="/profile"
                  className="flex items-center justify-center gap-2 text-center w-full link"
                  onClick={() => setIsOpen(false)}
                >
                  <User className="h-4 w-4" />
                  <span>
                    {user.name} {user.surname}
                  </span>
                </Link>

                <button
                  className="btn-ghost flex items-center gap-1"
                  onClick={() => {
                    setIsOpen(false);
                    loginMutate();
                  }}
                >
                  <LogOut className="h-4 w-4" /> Logout
                </button>
              </div>
            ) : (
              <>
                <Link
                  to="/login"
                  className="btn-ghost flex-1 text-center"
                  onClick={() => setIsOpen(false)}
                >
                  Log in
                </Link>
                <Link
                  to="/signup"
                  className="btn-primary flex-1 text-center"
                  onClick={() => setIsOpen(false)}
                >
                  Sign up
                </Link>
              </>
            )}
          </div>
        </div>
      </div>
    </header>
  );
}
