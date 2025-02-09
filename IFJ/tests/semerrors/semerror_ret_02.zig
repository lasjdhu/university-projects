// Void function has expression in return
const ifj = @import("ifj24.zig");
pub fn main() void {
    const x = 10;
    ifj.write(x);
}

pub fn foo() void {
    const x = 10;
    ifj.write(x);
    return x;
}

