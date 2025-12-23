/**
 * IIS Project
 * @brief Signup screen
 * @author Dmitrii Ivanushkin
 */
import { useState } from "react";
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import { Link, useNavigate } from "react-router";
import { signupSchema, type SignupFormData } from "@/lib/schemas/signup";
import { useMutation } from "@tanstack/react-query";
import { signup } from "@/lib/api/auth/signup";
import toast from "react-hot-toast";
import { Eye, EyeOff } from "lucide-react";

export default function Signup() {
  const navigate = useNavigate();
  const [showPassword, setShowPassword] = useState(false);
  const [showConfirmPassword, setShowConfirmPassword] = useState(false);

  const {
    register,
    handleSubmit,
    formState: { errors, isSubmitting },
  } = useForm<SignupFormData>({
    resolver: zodResolver(signupSchema),
  });

  const { mutate: signupMutate } = useMutation({
    mutationFn: signup,
    onSuccess: () => {
      toast.success("Successfully signed up. Please log in to your account");
      navigate("/login");
    },
  });

  const onSubmit = (data: SignupFormData) => {
    signupMutate({
      name: data.name,
      surname: data.surname,
      email: data.email,
      password: data.password,
    });
  };

  return (
    <main className="flex-1 flex items-center justify-center bg-gray-100 pt-16">
      <div className="w-full card sm:max-w-md mx-4">
        <h1 className="text-xl font-bold leading-tight tracking-tight text-gray-900 md:text-2xl mb-8">
          Create an account
        </h1>

        <form
          className="space-y-4 md:space-y-6"
          onSubmit={handleSubmit(onSubmit)}
        >
          <div className="flex gap-2">
            <div>
              <label
                htmlFor="name"
                className="block mb-2 text-sm font-medium text-gray-900"
              >
                First name
              </label>
              <input
                type="text"
                id="name"
                placeholder="John"
                className="input-field"
                {...register("name")}
              />
              {errors.name && (
                <p className="text-red-500 text-sm mt-2">
                  {errors.name.message}
                </p>
              )}
            </div>
            <div>
              <label
                htmlFor="surname"
                className="block mb-2 text-sm font-medium text-gray-900"
              >
                Surname
              </label>
              <input
                type="text"
                id="surname"
                placeholder="Doe"
                className="input-field"
                {...register("surname")}
              />
              {errors.surname && (
                <p className="text-red-500 text-sm mt-2">
                  {errors.surname.message}
                </p>
              )}
            </div>
          </div>

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
              onClick={() => setShowPassword((p) => !p)}
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

          <div className="relative">
            <label
              htmlFor="confirm_password"
              className="block mb-2 text-sm font-medium text-gray-900"
            >
              Confirm password
            </label>
            <input
              type={showConfirmPassword ? "text" : "password"}
              id="confirm_password"
              placeholder="••••••••"
              className="input-field pr-10"
              {...register("confirm_password")}
            />
            <button
              type="button"
              className="absolute right-4 top-10 text-gray-500 cursor-pointer"
              onClick={() => setShowConfirmPassword((p) => !p)}
            >
              {showConfirmPassword ? (
                <EyeOff className="h-5 w-5" />
              ) : (
                <Eye className="h-5 w-5" />
              )}
            </button>
            {errors.confirm_password && (
              <p className="text-red-500 text-sm mt-2">
                {errors.confirm_password.message}
              </p>
            )}
          </div>

          <button
            type="submit"
            className="w-full btn-primary"
            disabled={isSubmitting}
          >
            Sign up
          </button>

          <p className="text-center text-sm font-light text-gray-500">
            Already have an account?{" "}
            <Link to="/login" className="link">
              Log in
            </Link>
          </p>
        </form>
      </div>
    </main>
  );
}
