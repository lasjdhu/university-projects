# wave.tcl: TCL script for GTKWave simulator with signal definitions 
# Author(s): Lukas Kekely (ikekely@fit.vutbr.cz)

gtkwave::/Edit/Insert_Comment "Main Interface"
gtkwave::addSignalsFromList [list clk rst]
gtkwave::addSignalsFromList [list din_state]
gtkwave::/Edit/Color_Format/Blue
gtkwave::addSignalsFromList [list din dout dout_vld]
gtkwave::/Edit/Insert_Blank

gtkwave::/Edit/Insert_Comment "Other Signals"
  gtkwave::addSignalsFromList [list current_state CNT_BIT CNT_CYCLE BIT_EN CYCLE_EN]

gtkwave::/Edit/UnHighlight_All
gtkwave::/Time/Zoom/Zoom_Best_Fit
