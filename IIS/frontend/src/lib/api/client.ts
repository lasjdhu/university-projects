/**
 * IIS Project
 * @brief Axios setup, handling refreshes and toast emits
 * @author Dmitrii Ivanushkin
 */
import axios, {
  AxiosError,
  type AxiosRequestConfig,
  type AxiosResponse,
} from "axios";
import toast from "react-hot-toast";
import { refreshToken } from "./auth/refresh";

const API_URL = import.meta.env.VITE_API_URL;

export const api = axios.create({
  baseURL: API_URL,
  withCredentials: true,
});

interface FailedQueueItem {
  resolve: (token: string | PromiseLike<string | null> | null) => void;
  reject: (error: unknown) => void;
}

interface ApiErrorResponse {
  message?: string;
}

let isRefreshing = false;
let failedQueue: FailedQueueItem[] = [];

const processQueue = (error: unknown, token: string | null = null) => {
  failedQueue.forEach(({ resolve, reject }) => {
    if (error) reject(error);
    else resolve(token);
  });
  failedQueue = [];
};

api.interceptors.response.use(
  (response: AxiosResponse) => response,
  async (
    error: AxiosError<ApiErrorResponse> & {
      config: AxiosRequestConfig & { _retry?: boolean };
    },
  ) => {
    const originalRequest = error.config;

    const skipEndpoints = [
      "/auth/login",
      "/auth/register",
      "/auth/refresh",
      "/auth/user/me",
    ];
    const isSkipped = skipEndpoints.some((url) =>
      originalRequest.url?.includes(url),
    );

    if (error.response?.status === 401 && !isSkipped) {
      if (originalRequest._retry) {
        return Promise.reject(error);
      }

      if (isRefreshing) {
        return new Promise<string | null>((resolve, reject) => {
          failedQueue.push({ resolve, reject });
        })
          .then(() => api(originalRequest))
          .catch((err) => Promise.reject(err));
      }

      originalRequest._retry = true;
      isRefreshing = true;

      try {
        await refreshToken();
        isRefreshing = false;
        processQueue(null);
        return api(originalRequest);
      } catch (err) {
        isRefreshing = false;
        processQueue(err, null);
        return Promise.reject(err);
      }
    }

    const skipToast = originalRequest.url?.includes("/auth/user/me");

    if (!skipToast) {
      if (error.response?.data?.message) {
        toast.error(error.response.data.message);
      } else if (error.message) {
        toast.error(error.message);
      } else {
        toast.error("Something went wrong");
      }
    }

    return Promise.reject(error);
  },
);
