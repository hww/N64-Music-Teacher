sm.bin: demo.lnk boot.obj static.obj main.obj \
        cfb.obj zbuf.obj snmain.obj sprite.obj textbar.obj \
	playseq.obj cont.obj

        psylink /p /m /c /s @demo.lnk,demo.bin,demo.sym,demo.map

       runcart -f -a 0x80200000 -h \n64\bin\ultra1.v64 \
       -n '.oOo. Dynamix .oOo.' demo.bin

#        sec demo.bin
#        setcsum demo.bin $80200000
#        ias s demo.bin

snmain.obj: snmain.s

.c.obj:
	ccn6432 -I. -I/n64/include -I/n64/include/PR -c -g -O2 -G0 $*.c

.s.obj:
	asmn6432 /zd /l /ow- /g $*,$*
