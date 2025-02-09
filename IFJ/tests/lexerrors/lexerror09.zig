// Multiple invalid escapes
const ifj = @import("ifj24.zig");
pub fn main() void {
    var str = "Multiple \k \xZZ \\ problems";
}
