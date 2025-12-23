/**
 * IIS Project
 * @brief Tabs for pages with more info
 * @author Albert Tikaiev
 */
interface FilterOption<T extends string> {
  value: T;
  label: string;
  icon?: React.ReactNode;
}

interface FilterProps<T extends string> {
  options: FilterOption<T>[];
  chosen: T;
  setter: (value: T) => void;
}

export default function Filter<T extends string>({
  options,
  chosen,
  setter,
}: FilterProps<T>) {
  return (
    <div className="mb-4 flex flex-wrap gap-2">
      {options.map((opt) => {
        const isSelected = opt.value === chosen;
        return (
          <button
            key={opt.value}
            onClick={() => setter(opt.value)}
            className={`
              px-4 py-1 rounded-full font-medium text-sm transition
              border border-gray-300 flex items-center gap-2
              hover:bg-primary-100 hover:text-primary-700 cursor-pointer
              ${isSelected ? "bg-primary-600 text-white border-transparent" : "bg-white text-gray-700"}
            `}
          >
            {opt.icon}
            {opt.label}
          </button>
        );
      })}
    </div>
  );
}
