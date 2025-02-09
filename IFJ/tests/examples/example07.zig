const ifj = @import("ifj24.zig");
pub fn main() void {
    ifj.write("Ahoj\n\"Sve'te \\\x22");
    const a = ifj.readi32();
    if (a) |val| {
       ifj.write(val);
    } else { // a == null
        ifj.write("Faktorial pro null nelze spocitat\n");
    }
}
