const ifj = @import("ifj24.zig");
pub fn main() void {
    const a :f64 = 2.3;
    const b = a + 10;
    ifj.write(b);
    ifj.write("\n");
    const c = ifj.f2i(b);
    //here
    if((c>10) == 0){
        ifj.write("meow");
    }
    ifj.write(c);
    ifj.write("\n");
    _ = doda();
    return;
}

pub fn doda() i32 {
    ifj.write(
        \\HEHE
        \\opakujeme ifj
    );
    return 32;
}

