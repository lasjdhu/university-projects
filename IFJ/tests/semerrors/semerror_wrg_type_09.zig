// Type mismatch in expression
const ifj = @import("ifj24.zig");
pub fn main() void {
    const a: i32 = 5;
    ifj.write(a+4.5);
}

