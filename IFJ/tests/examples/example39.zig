const ifj = @import("ifj24.zig");
pub fn exampleFunction(a: i32, b: f64) void {
    ifj.write(a);
    ifj.write(b);
}

pub fn main() void {
    const a = ifj.chr(100);
    ifj.write(a);
    const b: f64 = 1.5;
    _ = exampleFunction(10, b)//asdas
    ;
    ifj.write(\\meow
        \\       mmmmm    //// a saaa   
        \\ aaa a's;da ""
        );
    if(5==5){
        if(5<6){
            if(5>4){
                if(5>=4){
                    if(5<=5.5){
                        if(5!=6){
                            ifj.write(\\Yep
                                );
                        }else{}
                    }else{}
                }else{}
            }else{}
        }else{}
    }else{}
    ifj.write(gg(1,2,3,4,5,6,7));
    exampleFunction(hh(1,2), ifj.i2f(hh(1,2)));
    exampleFunction(gg(1,2,3,4,5,6,7), ifj.i2f(gg(1,2,3,4,5,6,7)) );
}

pub fn gg(a: i32, b:i32, c: i32, g: i32, f: i32, h: i32, j: i32) i32{
    const temp: f64 = 10.5/10.5;
    return a*b*c*g*f*g*j*j*h*g*f*a*b*10*ifj.f2i(temp);
}

pub fn hh(a: i32, b:i32) i32{
    const temp: f64 = 10.5/10.5;
    return a*b*ifj.f2i(temp);
}

