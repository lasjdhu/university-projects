// Shadowing with another types is also forbidden
const ifj = @import("ifj24.zig");
pub fn main() void {
    var a: i32 = 100;
    if(100>500){
        var a: f64 = 1230.123;
    }
}

