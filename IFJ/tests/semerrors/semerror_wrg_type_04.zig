// Type mismatch in nullable expression
const ifj = @import("ifj24.zig");
pub fn main() void {
    var x: i32 = 10;
    x = ifj.readi32();
}

