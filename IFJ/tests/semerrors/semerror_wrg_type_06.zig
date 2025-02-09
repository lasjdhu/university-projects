// Type mismatch
const ifj = @import("ifj24.zig");
pub fn main() void {
    var x  = ifj.string("meow1");
    x = "meow2";
}

