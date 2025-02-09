// Program 16: While a matematika
const ifj = @import("ifj24.zig");

pub fn main() void {
    ifj.write("Enter a number to compute its sum of squares: ");
    const num = ifj . readi32();

    if (num) |n| {
        if (n < 0) {
            ifj.write("Negative numbers are not allowed.\n");
        } else {
            var sum: i32 = 0;
            var i: i32 = 1;
            while (i <= n) {
                sum = sum + (i * i);
                i = i + 1;
            }
            ifj.write("The sum of squares is: ");
            ifj.write(sum);
            ifj.write("\n");
        }
    } else {
        ifj.write("Invalid input.\n");
    }
}
