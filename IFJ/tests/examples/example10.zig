const ifj = @import("ifj24.zig");pub fn main() void {
    const a :f64 = 2.3;    
    const b = a + 10;
    ifj.write(b);    
    const c = ifj.f2i(b);
    ifj.write("\n");    
    ifj.write(c);
    _ = doda();    
    return;
}
pub fn doda() i32 {
    ifj.write("popppa");    
    return 32;
}
