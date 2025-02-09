// Program 15: Konkatenace
const ifj = @import("ifj24.zig");

pub fn main() void {
    const greeting = ifj.string("Hello, ");
    const name = ifj.string("World!");
    const combined = ifj.concat(greeting, name);
    ifj.write(combined);
    ifj.write("\n");

    const comparison = ifj.strcmp(greeting, name);
    if (comparison == 0) {
        ifj.write("Strings are equal.\n");
    } else {
        ifj.write("Strings are not equal.\n");
    }
}

