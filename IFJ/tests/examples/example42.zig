const ifj = @import("ifj24.zig");
pub fn main() void {
    var temp = retnull1();
    ifj.write(temp);
    ifj.write("\n");
    temp = retnull2();
    ifj.write(temp);
    ifj.write("\n");
    temp = retnull3();
    ifj.write(temp);
    ifj.write("\n");
}

pub fn retnull1() ?i32 {
    return 10;
}

pub fn retnull2() ?i32 {
    return ifj.readi32();
}

pub fn retnull3() ?i32 {
    return null;
}

