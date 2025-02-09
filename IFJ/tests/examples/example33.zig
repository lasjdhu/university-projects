const ifj = @import("ifj24.zig");

pub fn main() void{
    const a = popa(5+5);
    ifj.write(a);
}

pub fn popa(a : i32 ) i32{
    return a - 2;
}
