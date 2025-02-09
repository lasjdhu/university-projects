const ifj = @import("ifj24.zig");
pub fn main() ?void {
    ifj.write(test());
}

pub fn test() i32{
    return 6;
}

