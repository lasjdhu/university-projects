// Program 17: Fibonacci
const ifj = @import("ifj24.zig");

pub fn main() void {
    ifj.write("Enter the position in Fibonacci sequence: ");
    const position = ifj.readi32();

    if (position) |pos| {
        if (pos < 0) {
            ifj.write("Negative positions are not valid.\n");
        } else {
            const result = fibonacci(pos);
            ifj.write("Fibonacci result is: ");
            ifj.write(result);
            ifj.write("\n");
        }
    } else {
        ifj.write("Invalid input.\n");
    }
}

pub fn fibonacci(n: i32) i32 {
    if (n == 0) {
        return 0;
    } else {
        if (n == 1) {
            return 1;
        } else {
            const n1 = n - 1;
            const n2 = n - 2;
            return fibonacci(n1) + fibonacci(n2);
        }
    }
}
