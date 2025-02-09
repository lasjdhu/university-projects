const ifj = @import("ifj24.zig");
pub fn main() void {
    var str2 = ifj.string("world");
    var float: f64 = 3.14;
    var result2 = str2 * float;
}
