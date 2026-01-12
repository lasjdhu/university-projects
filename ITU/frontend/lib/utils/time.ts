/**
 * Project: ITU
 * Author: Dmitrii Ivanushkin
 * File: Helper functions for date parsing and relative time calculation.
 * Contains parseTimestamp to handle specific backend date formats and useTimeAgo
 * hook for live updates of last updated labels.
 */
import { useState, useEffect } from "react";

/**
 * Parses a timestamp string into a Date object.
 */
export function parseTimestamp(timestamp: string): Date {
  if (!timestamp) return new Date();
  let cleanTimestamp = timestamp.replace(/(\.\d{3})\d+/, "$1");

  const isUTC =
    !cleanTimestamp.endsWith("Z") && !/[+-]\d{2}:\d{2}$/.test(cleanTimestamp);

  if (isUTC) {
    cleanTimestamp += "Z";
  }

  return new Date(cleanTimestamp);
}

/**
 * React hook that returns a human-readable "time ago" string
 */
export function useTimeAgo(timestamp?: string) {
  const [timeAgo, setTimeAgo] = useState("just now");

  useEffect(() => {
    if (!timestamp) return;

    const update = () => {
      const date = parseTimestamp(timestamp);
      const diff = Math.max(
        0,
        Math.floor((Date.now() - date.getTime()) / 1000),
      );

      if (diff < 5) setTimeAgo("just now");
      else if (diff < 60) setTimeAgo(`${diff} seconds ago`);
      else if (diff < 3600) setTimeAgo(`${Math.floor(diff / 60)} minutes ago`);
      else if (diff < 86400) setTimeAgo(`${Math.floor(diff / 3600)} hours ago`);
      else setTimeAgo(`${Math.floor(diff / 86400)} days ago`);
    };

    update();
    const interval = setInterval(update, 1000);
    return () => clearInterval(interval);
  }, [timestamp]);

  return timeAgo;
}
