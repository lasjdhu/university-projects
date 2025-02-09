// Program 18: Typy a null
const ifj = @import("ifj24.zig");

pub fn main() void {
    const floatValue: f64 = 10.5;
    const intValue = ifj.f2i(floatValue);

    ifj.write("Float value: ");
    ifj.write(floatValue);
    ifj.write("\n");

    ifj.write("Converted to integer: ");
    ifj.write(intValue);
    ifj.write("\n");

    const nullValue: ?i32 = null;
    if (nullValue) |nonNull| {
        ifj.write("This won't be printed because nullValue is null.\n");
        ifj.write(nonNull);
    } else {
        ifj.write("nullValue is null.\n");
    }
}
