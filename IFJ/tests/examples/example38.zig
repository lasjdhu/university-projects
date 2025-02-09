const ifj = @import("ifj24.zig");

pub fn main() void{
    var a: ?i32 = 5;
    while(a) |_|{
        ifj.write("not null\n");
        a=null;
    }
}

