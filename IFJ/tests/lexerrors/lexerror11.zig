// Unclosed multi-line string literal
const ifj = @import("ifj24.zig");
pub fn main() void {
    var str = \\This is a multi-line string
    that is not closed properly;
}
