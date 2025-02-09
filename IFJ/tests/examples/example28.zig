//call of a same function not once 
const ifj = @import("ifj24.zig");

pub fn main() void {
    var a = 12;
    ifj.write(a);
    ifj.write("\n");
    a = decrement(a);
    ifj.write(a);
    ifj.write("\n");
    a = decrement(a);
    ifj.write(a);
    ifj.write("\n");
}

pub fn decrement(a : i32) i32 {
    return a-1;
}
