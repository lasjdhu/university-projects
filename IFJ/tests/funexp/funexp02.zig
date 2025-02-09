const ifj = @import("ifj24.zig");
pub fn main() void {
    const a = ifj.readi32();
    if(a) |A| {
        ifj.write(A+500);
        ifj.write("\n");
    }else{}
}
