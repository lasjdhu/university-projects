const ifj = @import("ifj24.zig");
pub fn main() void {
    var invalid: []u8 = ifj.string("\x()"); // Invalid character '@' in identifier
}
