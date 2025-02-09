// No return in function with non void return type
const ifj = @import("ifj24.zig");
pub fn main() void {
    const x = 10;
    ifj.write(x);
}

pub fn foo() i32 {
    const x = 10;
    ifj.write(x);
}
