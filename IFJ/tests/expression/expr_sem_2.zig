const ifj = @import("ifj24.zig");
pub fn main() void {
    const x: i32 = 5;
    const result2: f64 = x / (2.5 - 2.5);
    ifj.write(result2);
}
