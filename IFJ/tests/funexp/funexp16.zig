const ifj = @import("ifj24.zig");
pub fn main() void {
    const a = bigsum(
        bigsum(10, , 5, 5),                //25
        bigsum(500, 0-500, 0, 0),           //0
        bigsum(100, 0-100, 0-100, 0-100)   //-200
    );
    ifj.write(a);
    ifj.write("\n");
}

pub fn bigsum(a: i32, b: i32, c: i32,d: i32) i32{
    return a+b+c+d;
}

