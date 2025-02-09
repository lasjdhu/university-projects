const ifj = @import("ifj24.zig");
pub fn main() void {
    const a: []u8 = ifj.string("a");
    const b: []u8 = ifj.string("b");
    ifj.write(ifj.concat(a, b));
    ifj.write("\n");
}

