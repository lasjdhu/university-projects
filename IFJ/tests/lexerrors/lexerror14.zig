// Invalid character in \x
const ifj = @import("ifj24.zig");
pub fn main() void {
    var invalid: []u8 = ifj.string("\xFG"); // Invalid character '@' in identifier
}

