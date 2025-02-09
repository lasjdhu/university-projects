const ifj = @import("ifj24.zig");
pub fn main() void {
    const x: i32 = 5;
    const result1: i32 = x / 0;
    ifj.write(result1);
}
