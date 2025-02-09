// Expression mismatch
const ifj = @import("ifj24.zig");
pub fn main() void {
    const a: f64 = 10.123;
    const x: i32 = 10 + a;
}

