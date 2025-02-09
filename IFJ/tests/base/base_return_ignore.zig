// Cannot ignore return value
const ifj = @import("ifj24.zig");
pub fn getValue() i32 {
    return 42;
}

pub fn main() void {
    getValue();
}
