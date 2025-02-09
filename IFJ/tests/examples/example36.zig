const ifj = @import("ifj24.zig");

pub fn main() void{
    const a = ifj.readi32();
    if(a==5){
        ifj.write("You entered 5!");
        ifj.write("\n");
    }else{
        ifj.write("You haven't entered 5!");
        ifj.write("\n");
    }
}


