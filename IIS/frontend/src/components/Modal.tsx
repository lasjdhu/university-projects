/**
 * IIS Project
 * @brief Pop-up window
 * @author Dmitrii Ivanushkin
 */
import {
  Dialog,
  DialogBackdrop,
  DialogPanel,
  DialogTitle,
} from "@headlessui/react";
import { X } from "lucide-react";
import { type ReactNode } from "react";

interface ModalProps {
  open: boolean;
  onClose: (open: boolean) => void;
  title?: string;
  children: ReactNode;
}

export default function Modal({ open, onClose, title, children }: ModalProps) {
  return (
    <Dialog open={open} onClose={onClose} className="relative z-50">
      <DialogBackdrop
        transition
        className="fixed inset-0 bg-gray-900/50 transition-opacity data-closed:opacity-0 
                   data-enter:duration-300 data-enter:ease-out 
                   data-leave:duration-200 data-leave:ease-in"
      />

      <div className="fixed inset-0 z-50 w-screen overflow-y-auto">
        <div className="flex min-h-full items-end justify-center p-4 text-center sm:items-center sm:p-0">
          <DialogPanel
            transition
            className="relative transform overflow-hidden rounded-2xl bg-white text-left shadow-xl
                       transition-all sm:my-8 sm:w-full sm:max-w-md 
                       data-closed:translate-y-4 data-closed:opacity-0 
                       data-enter:duration-300 data-enter:ease-out 
                       data-leave:duration-200 data-leave:ease-in 
                       data-closed:sm:translate-y-0 data-closed:sm:scale-95"
          >
            <button
              onClick={() => onClose(false)}
              className="absolute top-4 right-4 text-gray-400 hover:text-gray-600 cursor-pointer"
            >
              <X className="h-5 w-5" />
            </button>

            <div className="bg-white px-6 py-6">
              {title && (
                <DialogTitle
                  as="h2"
                  className="text-xl font-bold text-gray-900 mb-6"
                >
                  {title}
                </DialogTitle>
              )}

              {children}
            </div>
          </DialogPanel>
        </div>
      </div>
    </Dialog>
  );
}
