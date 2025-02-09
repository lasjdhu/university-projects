// 2
const ifj = @import("ifj24.zig");

pub fn main() void {
    const result  addAndDouble(5, 10);
    ifj.write("The result of addAndDouble(5, 10) is: ");
    ifj.write(result);
    ifj.write("\n");
}

pub fn addAndDouble(x: i32, y: i32) f64 {
    const sum = x + y + ();
    return sum * 2.5;
}

