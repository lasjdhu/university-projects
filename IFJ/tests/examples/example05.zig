const ifj = @import("ifj24.zig");
pub fn main() void {
    const a: i32 = 50*(decrement(10,10)+40/2);
    const b: i32 = 10 + decrement(10,10);
    add(a,10);
    ifj.write(b);
    ifj.write("\n");
    return;
}

pub fn decrement(n: i32, m: i32) i32 {
    return n-m;
}

pub fn add(n: i32, m: i32) void {
    const a: i32 = n+m;
    ifj.write(a);
    ifj.write("\n");
}
