const ifj = @import("ifj24.zig");
pub fn main() void {
    var str = ifj.string("hello");
    var num: i32 = 42;
    var result1 = str + num;
}
