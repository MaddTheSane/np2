AS		= masm
AOPT	= /ML /DNP2
OBJ		= ..\obj\np2tool

INCS	= np2tool.inc np2tool.x86

all: $(OBJ) pwoff.com hostdrv.com

$(OBJ):
	if not exist $(OBJ) mkdir $(OBJ)


pwoff.com: $(OBJ)\pwoff.exe
	exe2bin $(OBJ)\pwoff pwoff.com

$(OBJ)\pwoff.exe: $(OBJ)\pwoff.obj
	link $(OBJ)\pwoff,$(OBJ)\pwoff;

$(OBJ)\pwoff.obj: pwoff.asm $(INCS)
	$(AS) $(AOPT) $(*B),$*,$*;


hostdrv.com: $(OBJ)\hostdrv.exe
	exe2bin $(OBJ)\hostdrv hostdrv.com

$(OBJ)\hostdrv.exe: $(OBJ)\hostdrv.obj
	link $(OBJ)\hostdrv,$(OBJ)\hostdrv;

$(OBJ)\hostdrv.obj: hostdrv.asm $(INCS) hostdrv.inc
	$(AS) $(AOPT) $(*B),$*,$*;

