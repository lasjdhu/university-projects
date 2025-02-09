// Invalid character in identifier
const ifj = @import("ifj24.zig");
pub fn main() void {
    const invalid: f64 = 1e4; // Invalid character '@' in identifier
    ifj.write(invalid);
}