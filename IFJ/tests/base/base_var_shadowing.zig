// Cannot shadow existing variable in same block
const ifj = @import("ifj24.zig");
pub fn main() void {
    var y: i32 = 5;
    var y: i32 = 7;
}
