
VERSION_ID		equ		"20040122"


HDRVIF			struc
drive_no			db		?
dosver				dw		?
sda_off				dw		?
sda_seg				dw		?
HDRVIF			ends


LOL				struc
lol_pad1			db		22 dup(?)
lol_cds				dd		?
lol_pad2			db		7 dup(?)
lol_lastdrv			db		?
LOL				ends

CDS				struc
cds_cur				db		67 dup(?)
cds_flag			dw		?
cds_padding			db		10 dup(?)
cds_root			dw		?
CDS				ends

