/**
 * IIS Project
 * @brief Team avatar, possibility to upload a new one
 * @author Dmitrii Ivanushkin, Albert Tikaiev
 */
import { Pen } from "lucide-react";
import { useState, type MouseEventHandler } from "react";

interface AvatarProps {
  image: string | null | undefined;
  name: string;
  width: number;
  height: number;
  onClick?: MouseEventHandler<HTMLDivElement>;
}

export default function Avatar({
  image,
  name,
  width,
  height,
  onClick,
}: AvatarProps) {
  const [loaded, setLoaded] = useState(false);
  const [error, setError] = useState(false);

  const getInitials = (name: string): string => {
    if (!name) return "?";
    const words = name.trim().split(/\s+/);
    if (words.length === 1) return words[0].charAt(0).toUpperCase();
    return words[0].charAt(0).toUpperCase() + words[1].charAt(0).toUpperCase();
  };

  const initials = getInitials(name);
  const showFallback = !image || error || !loaded;

  return (
    <div
      className={`relative aspect-square rounded-full overflow-hidden flex items-center justify-center bg-gray-100 ${onClick && "cursor-pointer"}`}
      style={{ width, height }}
      onClick={onClick}
    >
      {showFallback && (
        <div
          className="absolute inset-0 flex items-center justify-center rounded-full bg-primary-500 
                     text-white font-bold uppercase"
          style={{ fontSize: Math.min(width, height) / 3 }}
        >
          {initials}
        </div>
      )}

      {image && !error && (
        <img
          src={image}
          alt={`${name} avatar`}
          className={`w-full h-full object-cover rounded-full transition-opacity duration-300 ${
            loaded ? "opacity-100" : "opacity-0"
          }`}
          onLoad={() => setLoaded(true)}
          onError={() => setError(true)}
        />
      )}

      {onClick && (
        <div
          className="
            absolute inset-0 rounded-full bg-black/60
            opacity-0 hover:opacity-100
            transition-opacity duration-200
            flex items-center justify-center
          "
        >
          <Pen />
        </div>
      )}
    </div>
  );
}
