const ifj = @import("ifj24.zig");
pub fn main() void {
    const a = 10;
    const b = funexp(funexp(a));
    ifj.write(b);
}

pub fn funexp(i: i32) i32{
    return i;
}

