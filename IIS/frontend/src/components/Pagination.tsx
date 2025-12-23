/**
 * IIS Project
 * @brief Pagination UI
 * @author Dmitrii Ivanushkin
 */
import { ChevronLeftIcon, ChevronRightIcon } from "lucide-react";

interface PaginationProps {
  totalItems: number;
  itemsPerPage: number;
  currentPage: number;
  onPageChange: (page: number) => void;
}

export default function Pagination({
  totalItems,
  itemsPerPage,
  currentPage,
  onPageChange,
}: PaginationProps) {
  const totalPages = Math.ceil(totalItems / itemsPerPage);

  const startItem = totalItems === 0 ? 0 : (currentPage - 1) * itemsPerPage + 1;
  const endItem =
    totalItems === 0 ? 0 : Math.min(startItem + itemsPerPage - 1, totalItems);

  const visiblePages = () => {
    const delta = 2;
    const range: number[] = [];
    for (
      let i = Math.max(1, currentPage - delta);
      i <= Math.min(totalPages, currentPage + delta);
      i++
    ) {
      range.push(i);
    }
    return range;
  };

  return (
    <div className="flex items-center justify-between rounded-bl-lg rounded-br-lg py-3 px-6 border-t border-b border-gray-200 shadow bg-white">
      <div className="flex flex-col gap-3 w-full md:flex-row items-center md:justify-between">
        <p className="text-sm text-gray-700">
          Showing <span className="font-medium">{startItem}</span> to{" "}
          <span className="font-medium">{endItem}</span> of{" "}
          <span className="font-medium">{totalItems}</span> results
        </p>

        <nav
          aria-label="Pagination"
          className="isolate inline-flex -space-x-px rounded-md shadow-xs bg-white self-auto"
        >
          <button
            onClick={() => onPageChange(currentPage - 1)}
            disabled={currentPage === 1}
            className="relative inline-flex items-center justify-center w-10 h-10 rounded-l-md text-gray-400 inset-ring inset-ring-gray-300 hover:bg-gray-50 focus:z-20 focus:outline-offset-0 disabled:opacity-40 cursor-pointer disabled:cursor-default"
          >
            <span className="sr-only">Previous</span>
            <ChevronLeftIcon aria-hidden="true" className="size-5" />
          </button>

          {visiblePages().map((page) => (
            <button
              key={page}
              onClick={() => onPageChange(page)}
              aria-current={page === currentPage ? "page" : undefined}
              className={`relative inline-flex items-center justify-center w-10 h-10 text-sm font-semibold focus:z-20 ${
                page === currentPage
                  ? "z-10 bg-primary-600 text-white focus-visible:outline-2 focus-visible:outline-offset-2 focus-visible:outline-primary-600"
                  : "text-gray-900 inset-ring inset-ring-gray-300 hover:bg-gray-50 focus:outline-offset-0 cursor-pointer disabled:cursor-default"
              }`}
            >
              {page}
            </button>
          ))}

          <button
            onClick={() => onPageChange(currentPage + 1)}
            disabled={currentPage === totalPages}
            className="relative inline-flex items-center justify-center w-10 h-10 rounded-r-md text-gray-400 inset-ring inset-ring-gray-300 hover:bg-gray-50 focus:z-20 focus:outline-offset-0 disabled:opacity-40 cursor-pointer disabled:cursor-default"
          >
            <span className="sr-only">Next</span>
            <ChevronRightIcon aria-hidden="true" className="size-5" />
          </button>
        </nav>
      </div>
    </div>
  );
}
