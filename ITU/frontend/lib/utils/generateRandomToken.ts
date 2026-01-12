//created by xkrasoo00 on November 1st, 2025

export function generateToken() {
  return (
    Math.random().toString(32).slice(2) + Math.random().toString(32).slice(2)
  );
}
