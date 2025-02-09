// Main argument
const ifj = @import("ifj24.zig");
pub fn main(i: i32) void {
    const x = 10 + i;
    ifj.write(x);
}

