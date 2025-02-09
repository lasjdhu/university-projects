const ifj = @import("ifj24.zig");
pub fn main() void {
    funexp(5+5);
}

pub fn funexp(i: i32) void{
    ifj.write(i);
    ifj.write("\n");
}
