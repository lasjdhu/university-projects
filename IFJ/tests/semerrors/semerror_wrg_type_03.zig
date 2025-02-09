// Type mismatch in assignment
const ifj = @import("ifj24.zig");
pub fn main() void {
    var x: i32 = 10;
    x = "hello"; // Assigning a string to an integer variable
}

