/**
 * IIS Project
 * @brief Card with inviting mechanism
 * @author Albert Tikaiev, Dmitrii Ivanushkin
 */
import { putInviteResolve } from "@/lib/api/teamplayer/putInviteResolve";
import type { TeamPlayerInvite } from "@/lib/api/types";
import { useMutation } from "@tanstack/react-query";
import toast from "react-hot-toast";
import { Link } from "react-router";

interface TeamInviteProps {
  invite: TeamPlayerInvite;
  callback: () => void;
}

export default function TeamInvite({ invite, callback }: TeamInviteProps) {
  const { mutate: inviteMutate } = useMutation({
    mutationFn: putInviteResolve,
    onSuccess: () => {
      toast.success("Invite was proccessed successfully");
      callback();
    },
  });

  return (
    <article className="flex justify-between p-4 items-center">
      <div>
        <Link to={`/players/${invite.manager_id}`} className="link font-medium">
          {invite.manager_name} {invite.manager_surname}
        </Link>
        <span> invites you to join the </span>
        <Link to={`/teams/${invite.team_id}`} className="link font-medium">
          {invite.team_name}
        </Link>
        <span> team!</span>
      </div>
      <div className="flex gap-2">
        <button
          className="flex-1 md:flex-none btn-secondary border-green-500 hover:bg-green-100"
          onClick={() =>
            inviteMutate({ team_id: invite.team_id, result: "Accept" })
          }
        >
          Accept
        </button>
        <button
          className="flex-1 md:flex-none btn-secondary border-red-500 hover:bg-red-100"
          onClick={() =>
            inviteMutate({ team_id: invite.team_id, result: "Reject" })
          }
        >
          Reject
        </button>
      </div>
    </article>
  );
}
