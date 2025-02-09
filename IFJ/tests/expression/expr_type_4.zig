const ifj = @import("ifj24.zig");
pub fn main() void {
    const x: i32 = 5;
    const y: i32 = 10;
    const result = (x > y) + 1;
    ifj.write(result);
}
