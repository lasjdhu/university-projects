// Const variable can't be modified
const ifj = @import("ifj24.zig");
pub fn main() void {
    const x: i32 = 10;
    x = 20; // 'x' is const
}

