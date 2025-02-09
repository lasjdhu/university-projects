// Unused variable in another scope
const ifj = @import("ifj24.zig");
pub fn main() void {
    const a: i32 = 100;
    if(a==0){
        const meow: f64 = 10.23;
    }
    ifj.write(a);
    return;
}

