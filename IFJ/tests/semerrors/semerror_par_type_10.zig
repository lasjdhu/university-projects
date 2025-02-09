// Foo have only i32 arguments
const ifj = @import("ifj24.zig");
pub fn main() void {
    const x = 10;
    ifj.write(x);
    const b = foo(x, 12.3);
    ifj.write(b);
}

pub fn foo(a: i32, b: i32) i32 {
    const x = 10;
    ifj.write(x);
    return (x+a)*b;
}

