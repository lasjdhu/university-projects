// Foo doesn't have arguments
const ifj = @import("ifj24.zig");
pub fn main() void {
    const x = 10;
    ifj.write(x);
    const b = foo(x);
    ifj.write(b);
}

pub fn foo() i32 {
    const x = 10;
    ifj.write(x);
    return x;
}

