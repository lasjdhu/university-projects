// Void function has expression in return
const ifj = @import("ifj24.zig");
pub fn foo() i32 {
    const x = 10;
    ifj.write(x);
    return;
}
pub fn main() void {
    const x = 10;
    ifj.write(x);
}



