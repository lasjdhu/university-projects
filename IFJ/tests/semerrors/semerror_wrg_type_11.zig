const ifj = @import("ifj24.zig");
pub fn main() void {
    const a =ifj.readi32();
    if (a) |A|{
        const b = A + 45.6;
        ifj.write(b);
    }else{}
}

