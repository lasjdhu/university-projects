const ifj = @import("ifj24.zig");
pub fn main() void {    
    const str1 : []u8 = ifj.string("asdaaaaaaa");
    const str2 : ?[]u8 = ifj.substring(str1,1,3);
    if(str2) |S|{
        ifj.write(S);
    }
    else{
        ifj.write("error");
    }
    return; 
}

