const ifj = @import("ifj24.zig");
pub fn main() void {
    const a = (50);
    ifj.write(a);
    foo(a);
}

pub fn foo(i: i32) void {
    const a = 50 + i;
    ifj.write(a);
}

