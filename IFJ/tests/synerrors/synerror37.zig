const ifj = @import("ifj24.zig");

pub fn main() void {
    var invalidIndex: i32 = 0-1
    invalidIndex = 0;
    const str = ifj.string("Error Handling Example\n");
    const next_invalidIndex = invalidIndex + 1;
    const result = ifj.substring(str, invalidIndex, next_invalidIndex);
    if (result) |valid| {
        ifj.write("Substring: ");
        ifj.write(valid);
        ifj.write("\n");
    } else {
        ifj.write("Error: Invalid substring indices.\n");
    }
}


