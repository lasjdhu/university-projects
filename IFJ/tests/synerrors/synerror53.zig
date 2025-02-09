const ifj = @import("ifj24.zig");
pub fn main() void {
    if( (test() == 6) == 1){
        ifj.write("meow");
    }else {}
}

pub fn test() i32{
    return 6;
}

