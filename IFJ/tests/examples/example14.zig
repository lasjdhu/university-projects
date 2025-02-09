// Ukazka prace s retezci a vestavenymi funkcemi 
const ifj = @import("ifj24.zig");

pub fn main() void {
  const s1 : []u8 = ifj.string( 
    \\To\tto 
   \\ je 
    \\
    \\ nejaky\n 
\\  text  // ve viceradkovem retezcovem literalu nelze mit komentar
    ); // bile znaky za \\ jsou soucasti retezce, posledni EOL ne; escape sekvence nejsou interpretovany
  ifj.write(s1);
}
