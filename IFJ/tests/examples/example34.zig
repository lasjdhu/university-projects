const ifj = @import("ifj24.zig");

pub fn main() void{
    const a: ?i32 = null;
    if(a==null){
        ifj.write("null");
        ifj.write("\n");
    }else{}
}
