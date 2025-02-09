// Program 22: another version of strcmp for assembly
const ifj = @import("ifj24.zig");

pub fn main() void {
    const str1 = ifj.string("Error Handling Example\n");
    const str2 = ifj.string("Error Handling Example\n");
    const str3 = ifj.strcmp(str1,str2);
    ifj.write(str3);
    ifj.write("\n");
}

