/**
 * IIS Project
 * @brief Activity indicator
 * @author Dmitrii Ivanushkin
 */
interface SpinnerProps {
  size?: number;
}

export default function Spinner({ size = 4 }: SpinnerProps) {
  return (
    <div
      className="inline-block animate-spin rounded-full border-4 border-solid border-current border-e-transparent align-[-0.125em] motion-reduce:animate-[spin_1.5s_linear_infinite] text-primary-500"
      style={{ height: `${size}rem`, width: `${size}rem` }}
      role="status"
    >
      <span className="sr-only">Loading...</span>{" "}
    </div>
  );
}
