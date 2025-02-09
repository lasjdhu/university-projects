const ifj = @import("ifj24.zig");
pub fn main() void {
    _ = yoy(5+5 - 1 0, 0);
}

pub fn yoy(a: i32, b : i32) void{
    ifj.write(a+b);
}
