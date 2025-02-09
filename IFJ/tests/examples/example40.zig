const ifj = @import("ifj24.zig");
pub fn main() void {
    if(ifj.readi32()) |A|{ifj.write(A);ifj.write("\n");}
    else{ifj.write("null\n");}
}
