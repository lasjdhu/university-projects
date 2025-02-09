const ifj = @import("ifj24.zig");

pub fn main() void{
    const a: ?i32 = 5;
    if(a) |_|{
        ifj.write("not null\n");
    }else{}
}


