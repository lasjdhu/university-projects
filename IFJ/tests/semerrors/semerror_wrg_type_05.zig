// Type mismatch
const ifj = @import("ifj24.zig");
pub fn main() void {
    const x = ifj.readstr();
    var a = 0;
    if (x) |inpOk| {
        a = 10+inpOk;
    } 
}
