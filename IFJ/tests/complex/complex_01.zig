const ifj = @import("ifj24.zig");

pub fn main() void {
    ifj.write("Vyberte moznost (1 - Rekurzivne, 2 - Iterativne): ");
    const mode = ifj.readi32();
    if (mode) |MODE| {
        ifj.write("Zadejte zaklad: ");
        const base = ifj.readi32();
        ifj.write("Zadejte exponent: ");
        const exp = ifj.readi32();

        if (base) |BASE| {
            if (exp) |EXP| {
                if (EXP < 0) {
                    ifj.write("Exponent nelze zpracovat jako zaporne cislo!\n");
                } else {
                    if (MODE == 1) {
                        const vysl = power_recursive(BASE, EXP);
                        ifj.write("Rekurzivni vysledek: ");
                        ifj.write(vysl);
                        ifj.write("\n");
                    } else {
                        if (MODE == 2) {
                        const vysl = power_iterative(BASE, EXP);
                        ifj.write("Iterativni vysledek: ");
                        ifj.write(vysl);
                        ifj.write("\n");
                        } else{
                            ifj.write("Neplatna moznost, zkuste znovu.\n");
                        }
                    } 
                }
            } else{
                ifj.write("Chyba pri nacitani zakladu nebo exponentu!\n");
            }
        }else {
            ifj.write("Chyba pri nacitani zakladu nebo exponentu!\n");
        }

    } else {
        ifj.write("Chyba pri nacitani moznosti!\n");
    }
}

pub fn power_recursive(base: i32, exp: i32) i32 {
    if (exp == 0) {
        return 1;
    } else {
        return base * power_recursive(base, exp- 1);
    }
}

pub fn power_iterative(base: i32, exp: i32) i32 {
    var result: i32 = 1;
    var i: i32 = 0;
    while (i < exp) {
        result = result * base;
        i = i + 1;
    }
    return result;
}

pub fn decrement(n: i32, m: i32) i32 {
    return n - m;
}
