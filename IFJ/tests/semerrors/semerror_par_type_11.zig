// Foo have return type and is not ignored in main
const ifj = @import("ifj24.zig");
pub fn main() void {
    const x = 10;
    ifj.write(x);
    foo(x, x);
}

pub fn foo(a: i32, b: i32) i32 {
    const x = 10;
    ifj.write(x);
    return (x+a)*b;
}


