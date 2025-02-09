// Return value of other type
const ifj = @import("ifj24.zig");
pub fn main() void {
    const x = 10;
    ifj.write(x);
}

pub fn foo() i32 {
    const x = 10;
    ifj.write(x);
    return 11.2;
}

