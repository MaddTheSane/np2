AS		= masm
AOPT	= /ML /DNP2
OBJ		= ..\obj\romrel

all: $(OBJ)

$(OBJ):
	if not exist $(OBJ) mkdir $(OBJ)

