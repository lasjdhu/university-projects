// Incorrect number or type of parameters
const ifj = @import("ifj24.zig");
pub fn exampleFunction(a: i32, b: i32) void {
    ifj.write(a);
    ifj.write(b);
}

pub fn main() void {
    exampleFunction(10, "string");
}
