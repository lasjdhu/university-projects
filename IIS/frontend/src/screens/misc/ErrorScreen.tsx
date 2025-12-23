/**
 * IIS Project
 * @brief Error screen
 * @author Dmitrii Ivanushkin
 */
interface ErrorScreenProps {
  message: string;
}

export default function ErrorScreen({ message }: ErrorScreenProps) {
  return (
    <main className="flex-1 flex flex-col bg-gray-100 items-center justify-center p-16 pb-0 gap-4">
      <h1 className="text-4xl heading-1">An error occured</h1>
      <pre className="text-xl heading-2">{message}</pre>
    </main>
  );
}
