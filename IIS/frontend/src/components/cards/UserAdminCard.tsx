/**
 * IIS Project
 * @brief Card used by admin to see user info
 * @author Albert Tikaiev
 */
import type { UserResponse } from "@/lib/api/types";
import { putUserAdmin } from "@/lib/api/users/putUserAdmin";
import { useMutation } from "@tanstack/react-query";
import { useState } from "react";
import toast from "react-hot-toast";
import { Link } from "react-router";
import UserAdminForm from "../forms/UserAdminForm";
import Modal from "../Modal";

interface UserAdminCardProps {
  user: UserResponse;
  refetch: () => void;
}

export default function UserAdminCard({ user, refetch }: UserAdminCardProps) {
  const [edit, setEdit] = useState(false);

  const { mutate: editMutate, isPending: isSubmitting } = useMutation({
    mutationFn: putUserAdmin,
    onSuccess: () => {
      refetch();
      setEdit(false);
      toast.success("Successfully changed user");
    },
  });

  return (
    <article className="w-full flex items-center justify-between p-4 h-22">
      <h2 className="ml-4 heading-2">
        {user.name} {user.surname}
      </h2>
      <div className="flex">
        <button className="btn-ghost mr-4" onClick={() => setEdit(true)}>
          Edit user
        </button>
        <Link to={`/players/${user.id}`} className="btn-secondary">
          View
        </Link>
      </div>

      <Modal open={edit} onClose={setEdit} title="Edit user">
        <UserAdminForm
          initialData={{ email: user.email, password: undefined }}
          onSubmit={(formData) => editMutate({ id: user.id, data: formData })}
          isSubmitting={isSubmitting}
        />
      </Modal>
    </article>
  );
}
