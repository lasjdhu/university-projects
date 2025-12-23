/**
 * IIS Project
 * @brief Loading screen
 * @author Dmitrii Ivanushkin
 */
import Spinner from "@/components/Spinner";

export default function LoadingScreen() {
  return (
    <main className="flex-1 flex flex-col bg-gray-100 items-center justify-center p-16 pb-0">
      <Spinner />
    </main>
  );
}
