obj = canvas.obj cga.obj editor.obj geometry.obj init.obj input.obj keyb.obj list.obj timer.obj ui.obj ui_elem.obj views.obj
bin = 437.exe

CC = wcc
CFLAGS = -zq -otx
LD = wlink
INCLUDE = include
LIB = /usr/share/openwatcom/lib286/dos/:/usr/share/openwatcom/lib286

$(bin): $(obj)
	$(LD) name $@ file { $(obj) } libpath $(LIB) form dos

.c.obj:
	$(CC) -i$(INCLUDE) -fo=$@ $(CFLAGS) $<

clean: .symbolic
	rm *.obj
	rm *.err
	rm $(bin)

run: .symbolic
	dosbox $(bin)
