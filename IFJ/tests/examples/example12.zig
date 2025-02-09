const ifj = @import("ifj24.zig");

pub fn build(x : []u8, y : []u8) []u8 {
    const res = ifj. concat(x, y);
    return res;
}

pub fn main() void {
    const a = ifj.string("ahoj ");
    var ct : []u8 = ifj.string("svete");
    ct = build(a, ct);
    ifj.write(ct);
}
