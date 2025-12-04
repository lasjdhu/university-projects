/**
 * IIS Project
 * @brief Type definitions of responses
 * @author Dmitrii Ivanushkin, Albert Tikaiev
 */
export type QueryParams = {
  page?: string;
  limit?: string;
  state?: string;
  detail?: string;
  search?: string;
  team?: string;
  user?: string;
};

export interface PaginationResponse<T> {
  data: T[];
  total_records: number;
  total_pages: number;
  page: number;
  limit: number;
}

export type Role = ["Admin", "Registered"];

export type BaseUserResponse = {
  id: number;
  email: string;
  name: string;
  surname: string;
};
export type UserResponse = BaseUserResponse & {
  role: Role[number];
};

export type LogoutResponse = {
  message: string;
};

export type ParticipantResponse = {
  id: number;
  user_id: number | null;
  team_id: number | null;
  name: string;
};

export type ParticipantWithConflictsResponse = ParticipantResponse & {
  conflicts: string[];
};

export type TournamentType = ["Person", "Team"];
export type TournamentState = ["Accepted", "Pending", "Rejected"];
export type TournamentResponse = {
  id: number;
  name: string;
  discipline: string;
  expected_members: number;
  type: TournamentType[number];
};

export type TournamentDetailBase = TournamentResponse & {
  prize: number;
  min_limit: number | null;
  max_limit: number | null;
};
export type TournamentDetailResponse = TournamentDetailBase & {
  manager: PlayerResponse;
  participants: ParticipantResponse[] | null;
};
export type TournamentDetailWithState = TournamentDetailBase & {
  state: TournamentState[number];
};
export type TournamentAdminDetailResponse = TournamentDetailWithState & {
  manager: PlayerResponse;
};
export type TournamentProfileDetailResponse = TournamentDetailWithState & {
  participant_requests: ParticipantWithConflictsResponse[];
};
export type TournamentStatePut = {
  id: number;
  state: TournamentState[number];
};

export type TeamResponse = {
  id: number;
  image?: string; // URL
  description: string;
  name: string;
  manager_id: number;
};
export type TeamDetailResponse = Omit<TeamResponse, "manager_id"> & {
  since: string;
  manager: PlayerResponse;
  players: PlayerResponse[] | null;
  winnings: number;
  winrate: WinrateValues;
  disciplines: DisciplineValues[] | null;
  activity: ActivityValues[] | null;
};

export type PlayerResponse = Pick<UserResponse, "id" | "name" | "surname">;
export type PlayerStateResponse = PlayerResponse & {
  player_id: number;
  state: "Invited" | "Active" | "Inactive";
};
export type PlayerDetailResponse = PlayerResponse & {
  winnings: number;
  winrate: WinrateValues;
  disciplines: DisciplineValues[] | null;
  activity: ActivityValues[] | null;
};

export type OverviewResponse = {
  tournaments_count: number;
  teams_count: number;
  players_count: number;
  tournaments: TournamentResponse[];
  teams: TeamResponse[];
  players: PlayerResponse[];
};

export type MatchParticipant = {
  id: number | null;
  name: string | null;
  is_winner: boolean;
  result_text: string | null;
};

export type ResolvedMatchParticipant = {
  id: number;
  name: string;
  is_winner: boolean;
  score: string;
};

export type MatchDetailed = {
  id: number;
  date?: string;
  tournament_name: string;
  tournament_id: number;
  first: ResolvedMatchParticipant;
  second: ResolvedMatchParticipant;
  type: "Team" | "Person";
};

export type BracketMatch = {
  id: number;
  name: string;
  next_match_id: number | null;
  tournament_round_text: string;
  date: string | null;
  participants: MatchParticipant[];
};

export type TournamentBracketResponse = {
  matches: BracketMatch[] | null;
};

export type TeamAndPlayers = Omit<TeamResponse, "manager_id"> & {
  confilct_tournaments: string[] | null;
  players: PlayerStateResponse[];
};

export type TeamPlayerInvite = {
  team_id: number;
  team_name: string;
  manager_id: number;
  manager_name: string;
  manager_surname: string;
};

export type ProfileResponse = {
  managing?: TeamAndPlayers[];
  team_invites?: TeamPlayerInvite[];
  created_tournaments?: TournamentProfileDetailResponse[];
};

export type WinrateValues = {
  wins: number;
  loses: number;
  percentage: number;
};

export type DisciplineValues = {
  name: number;
  tournaments: number;
};

export type ActivityValues = {
  month: string;
  personal: number;
  teams: number;
};
