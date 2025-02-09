// Type mismatch in function argument
const ifj = @import("ifj24.zig");
pub fn add(a: i32, b: i32) i32 {
    return a + b;
}

pub fn main() void {
    const result = add(10, "20"); // Second argument should be an integer
}
