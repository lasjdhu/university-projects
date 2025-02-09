// Type mismatch in expression
const ifj = @import("ifj24.zig");
pub fn main() void {
    var a = 10;
    const b = a;
    if(b==4.5){
        ifj.write(b);
    }else{}
}

