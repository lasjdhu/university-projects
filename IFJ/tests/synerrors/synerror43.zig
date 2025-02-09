const ifj = @import("ifj24.zig");
pub fn main() void {
    const a: i32 = 5 + *3;
    ifj.write(a);
}
