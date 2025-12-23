/**
 * IIS Project
 * @brief Custom input that works like autocomplete combobox
 * @author Albert Tikaiev, Dmitrii Ivanushkin
 */
import type { BaseUserResponse } from "@/lib/api/types";
import { useState, useRef } from "react";

export default function EmailSearchInput({
  value,
  onChange,
  suggestions = [],
  error,
}: {
  value: string;
  onChange: (v: string) => void;
  suggestions: BaseUserResponse[] | null;
  error?: string;
}) {
  const [open, setOpen] = useState(false);
  const wrapperRef = useRef<HTMLDivElement>(null);

  return (
    <div
      className="relative w-full"
      ref={wrapperRef}
      onBlur={(e) => {
        if (!e.currentTarget.contains(e.relatedTarget as Node)) {
          setOpen(false);
        }
      }}
    >
      <div className="relative">
        <input
          type="email"
          value={value}
          placeholder="name@company.com"
          className="input-field"
          onChange={(e) => {
            onChange(e.target.value);
            setOpen(true);
          }}
          onFocus={() => setOpen(true)}
        />
      </div>

      {error && <p className="text-red-500 text-sm mt-1 absolute">{error}</p>}

      {open && suggestions && suggestions.length > 0 && (
        <div className="absolute z-50 w-full bg-white border border-gray-200 rounded-lg mt-1">
          <div
            className="max-h-72 rounded-b-lg overflow-hidden overflow-y-auto
            [&::-webkit-scrollbar]:w-2
            [&::-webkit-scrollbar-thumb]:rounded-full
            [&::-webkit-scrollbar-track]:bg-gray-100
            [&::-webkit-scrollbar-thumb]:bg-gray-300"
          >
            {suggestions.map((s) => (
              <button
                key={s.id}
                type="button"
                className="flex items-center cursor-pointer py-2 px-4 w-full text-sm text-gray-800 hover:bg-gray-100 "
                onMouseDown={() => {
                  onChange(s.email);
                  setOpen(false);
                }}
              >
                <div>{s.email}</div>
              </button>
            ))}
          </div>
        </div>
      )}
    </div>
  );
}
