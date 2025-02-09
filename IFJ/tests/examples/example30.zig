//exponent
const ifj = @import("ifj24.zig");

pub fn main() void {
    const exp: f64 = 2e2;
    const exp2: f64 = exp/2;
    ifj.write(exp);
    ifj.write("\n");
    ifj.write(exp2);
    ifj.write("\n");
    if(exp == exp2*2){
        ifj.write("hell yeah");
        ifj.write("\n");
    }else{
        ifj.write("hell nah");
    }
}
