/**
 * IIS Project
 * @brief Logins screen
 * @author Dmitrii Ivanushkin
 */
import { useState } from "react";
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import { Link, useNavigate } from "react-router";
import { loginSchema, type LoginFormData } from "@/lib/schemas/login";
import { useMutation } from "@tanstack/react-query";
import { login } from "@/lib/api/auth/login";
import toast from "react-hot-toast";
import { Eye, EyeOff } from "lucide-react";
import { useUser } from "@/lib/contexts/useUser";

export default function Login() {
  const navigate = useNavigate();
  const { setUser } = useUser();
  const [showPassword, setShowPassword] = useState(false);

  const {
    register,
    handleSubmit,
    formState: { errors, isSubmitting },
  } = useForm<LoginFormData>({
    resolver: zodResolver(loginSchema),
  });

  const { mutate: loginMutate } = useMutation({
    mutationFn: login,
    onSuccess: (data) => {
      setUser(data);
      toast.success("Successfully logged in");
      navigate(`/profile`);
    },
  });

  const onSubmit = (data: LoginFormData) => loginMutate(data);

  return (
    <main className="flex-1 flex items-center justify-center bg-gray-100 pt-16">
      <div className="w-full card sm:max-w-md mx-4">
        <h1 className="text-xl font-bold leading-tight tracking-tight text-gray-900 md:text-2xl mb-8">
          Log in to your account
        </h1>

        <form
          className="space-y-4 md:space-y-6"
          onSubmit={handleSubmit(onSubmit)}
        >
          <div>
            <label
              htmlFor="email"
              className="block mb-2 text-sm font-medium text-gray-900"
            >
              Email
            </label>
            <input
              type="email"
              id="email"
              placeholder="name@company.com"
              className="input-field"
              {...register("email")}
            />
            {errors.email && (
              <p className="text-red-500 text-sm mt-2">
                {errors.email.message}
              </p>
            )}
          </div>

          <div className="relative">
            <label
              htmlFor="password"
              className="block mb-2 text-sm font-medium text-gray-900"
            >
              Password
            </label>
            <input
              type={showPassword ? "text" : "password"}
              id="password"
              placeholder="••••••••"
              className="input-field pr-10"
              {...register("password")}
            />
            <button
              type="button"
              className="absolute right-4 top-10 text-gray-500 cursor-pointer"
              onClick={() => setShowPassword((prev) => !prev)}
            >
              {showPassword ? (
                <EyeOff className="h-5 w-5" />
              ) : (
                <Eye className="h-5 w-5" />
              )}
            </button>
            {errors.password && (
              <p className="text-red-500 text-sm mt-2">
                {errors.password.message}
              </p>
            )}
          </div>

          <button
            type="submit"
            className="w-full btn-primary"
            disabled={isSubmitting}
          >
            Log in
          </button>

          <p className="text-center text-sm font-light text-gray-500">
            Don’t have an account yet?{" "}
            <Link to="/signup" className="link">
              Sign up
            </Link>
          </p>
        </form>
      </div>
    </main>
  );
}
