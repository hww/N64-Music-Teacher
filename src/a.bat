rem MIDI compiling Ø‡Æ¢•‡®‚Ï ·onfig.h
del *.cvt
del *.cmp
del *.sbk
del *.sbk
del *.bin
midicvt airBAsH.mid airBAsH.mi0
midicomp airBAsH.mi0 airBAsH.mic
pause
sbc -oDEMO blowdown.mic brwnjug.mic cumnrnd.mic jinglbel.mic YEST2.MIC PROMENAD.MIC podnebom.mic nonsopiu.mic airBAsH.mic
c.bat
