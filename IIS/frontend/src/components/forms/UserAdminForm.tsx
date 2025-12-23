/**
 * IIS Project
 * @brief Form for changing user info by admin
 * @author Albert Tikaiev
 */
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import { userEditSchema, type UserEditFormData } from "@/lib/schemas/userEdit";
import { Eye, EyeOff } from "lucide-react";
import { useState } from "react";

interface UserAdminFormProps {
  initialData: { email: string; password: undefined };
  onSubmit: (data: UserEditFormData) => void;
  isSubmitting?: boolean;
}

export default function UserAdminForm({
  initialData,
  onSubmit,
  isSubmitting,
}: UserAdminFormProps) {
  const [showPassword, setShowPassword] = useState(false);

  const {
    register,
    handleSubmit,
    formState: { errors },
  } = useForm<UserEditFormData>({
    defaultValues: initialData,
    resolver: zodResolver(userEditSchema),
  });

  return (
    <form className="space-y-4" onSubmit={handleSubmit(onSubmit)}>
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
          <p className="text-red-500 text-sm mt-2">{errors.email.message}</p>
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
          <p className="text-red-500 text-sm mt-2">{errors.password.message}</p>
        )}
      </div>

      <button
        type="submit"
        className="w-full btn-primary mt-8"
        disabled={isSubmitting}
      >
        Save
      </button>
    </form>
  );
}
