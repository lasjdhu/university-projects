const ifj = @import("ifj24.zig");

pub fn main() void {
    const a = increment();
    ifj.write(a);
    ifj.write("\n");
}

pub fn increment() i32{
    return 2;
}
