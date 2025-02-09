const ifj = @import("ifj24.zig");
pub fn main() void {
    //ifj.write(ifj.concat(ifj.chr(97),ifj.concat(ifj.chr(104),ifj.concat(ifj.chr(111),ifj.chr(106)))));
    ifj.write(ifj.concat(ifj.concat( ifj.concat(ifj.chr(97), ifj.chr(104)), ifj.chr(111) ), ifj.chr(106) ));
    ifj.write("\n");
}


