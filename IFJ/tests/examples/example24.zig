const ifj = @import("ifj24.zig");

pub fn main() void {
    const a: i32 = 100;
    if(a <= 100){
        return;
    }
    else{}
    ifj.write("returned");
}

