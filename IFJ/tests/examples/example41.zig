const ifj = @import("ifj24.zig");
pub fn main() void {
    while(ifj.readi32()) |A| {ifj.write(A);ifj.write("\n");}
}

