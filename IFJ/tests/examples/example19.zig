// Program 19: While a substring
const ifj = @import("ifj24.zig");

pub fn main() void {
    const text = ifj.string("Hello, IFJ24!");
    ifj.write("Original text: ");
    ifj.write(text);
    ifj.write("\n");

    var index: i32 = 0;
    while (index < ifj.length(text)) {
        const next_index = index+1;
        const char = ifj.substring(text, index, next_index);
        ifj.write("Character at index ");
        ifj.write(index);
        ifj.write(": ");
        ifj.write(char);
        ifj.write("\n");
        index = index + 1;
    }
}
