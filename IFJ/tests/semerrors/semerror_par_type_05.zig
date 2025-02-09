// Type mismatch in return value
const ifj = @import("ifj24.zig");
pub fn getNumber() i32 {
    return "hello"; // Function should return an integer
}

pub fn main() void {
    const num = getNumber();
}
