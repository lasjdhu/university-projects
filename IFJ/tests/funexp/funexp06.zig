const ifj = @import("ifj24.zig");
pub fn main() void {
    ifj.write(ifj.concat(ifj.chr(97),ifj.chr(104)));
    ifj.write("\n");
}


