/**
 * IIS Project
 * @brief Tournament bracket
 * @author Dmitrii Ivanushkin
 */
import { useEffect, useRef } from "react";
import { createBracket } from "bracketry";
import type { BracketMatch } from "@/lib/api/types";
import { transformMatches } from "@/lib/utils/transformMatches";

interface TournamentBracketProps {
  matches: BracketMatch[];
  onMatchClick?: (match: BracketMatch) => void;
  canSeeMatchClick: boolean;
}

export default function TournamentBracket({
  matches,
  onMatchClick,
  canSeeMatchClick,
}: TournamentBracketProps) {
  const containerRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    const container = containerRef.current;
    if (!container) return;

    const data = transformMatches(matches);

    container.innerHTML = "";

    createBracket(data, container, {
      width: "100%",
      height: "100%",
      rootBgColor: "rgb(243 244 246)",
      wrapperBorderColor: "transparent",
      roundTitlesBorderColor: "transparent",
      scrollGutterBorderColor: "transparent",
      navGutterBorderColor: "transparent",
      liveMatchBorderColor: "transparent",
      hoveredMatchBorderColor: "#0cbbeb",
      verticalScrollMode: "native",
      rootFontFamily: "Inter, sans-serif",
      roundTitlesFontFamily: "Inter, sans-serif",
      roundTitlesFontSize: 18,
      roundTitleColor: "#111827",
      matchTextColor: "#1f2937",
      matchFontSize: 14,
      playerTitleFontFamily: "Inter, sans-serif",
      highlightedPlayerTitleColor: "#0cbbeb",
      scoreFontFamily: "Inter, sans-serif",
      connectionLinesWidth: 2,
      connectionLinesColor: "#d1d5db",
      highlightedConnectionLinesColor: "#0cbbeb",
      matchMaxWidth: 400,
      matchMinVerticalGap: 20,
      matchHorMargin: 20,
      matchAxisMargin: 4,
      oneSidePlayersGap: 2,
      liveMatchBgColor: "rgba(68, 201, 133, 0.1)",
      distanceBetweenScorePairs: 12,
      matchStatusBgColor: "#f3f4f6",
      navButtonSvgColor: "#0cbbeb",
      navButtonArrowSize: 34,
      navButtonPadding: "4px",
      useClassicalLayout: true,
      getRoundTitleElement: (_round, index) => {
        const div = document.createElement("div");
        div.textContent = `Round ${index + 1}`;
        div.style.fontWeight = "bold";
        div.style.color = "#111827";
        div.style.fontFamily = "Inter, sans-serif";
        div.style.fontSize = "18px";
        div.style.textAlign = "center";
        return div;
      },
      onMatchClick: canSeeMatchClick
        ? (match) => {
            const ogMatch = matches.find((m) => m.id === match.originalId);
            if (ogMatch) onMatchClick?.(ogMatch);
          }
        : undefined,
    });

    return () => {
      if (container) container.innerHTML = "";
    };
  }, [matches, onMatchClick, canSeeMatchClick]);

  return (
    <div
      className="overflow-auto"
      ref={containerRef}
      style={{ minHeight: 400 }}
    />
  );
}
