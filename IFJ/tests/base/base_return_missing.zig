// Ignoring explicit return type
const ifj = @import("ifj24.zig");
pub fn calculateSum(a: i32, b: i32) i32 {
    ifj.write(a);
    ifj.write(b);
}

pub fn main() void {}
