
static const UINT8 keytable[8][0x60] = {
	{	//												normal
		//	 ESC,  １,  ２,  ３,  ４,  ５,  ６,  ７		; 00h
			0x1b, '1', '2', '3', '4', '5', '6', '7',
		//	  ８,  ９,  ０,  −,  ＾,  ￥,  BS, TAB		; 08h
			 '8', '9', '0', '-', '^','\\',0x08,0x09,
		//	  Ｑ,  Ｗ,  Ｅ,  Ｒ,  Ｔ,  Ｙ,  Ｕ,  Ｉ		; 10h
			 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
		//	  Ｏ,  Ｐ,  ＠,  ［, Ret,  Ａ,  Ｓ,  Ｄ		; 18h
			 'o', 'p', '@', '[',0x0d, 'a', 's', 'd',
		//	  Ｆ,  Ｇ,  Ｈ,  Ｊ,  Ｋ,  Ｌ,  ；,  ：		; 20h
			 'f', 'g', 'h', 'j', 'k', 'l', ';', ':',
		//    ］,  Ｚ,  Ｘ,  Ｃ,  Ｖ,  Ｂ,  Ｎ,  Ｍ		; 28h
			 ']', 'z', 'x', 'c', 'v', 'b', 'n', 'm',
		//    ，,  ．,  ／,  ＿, SPC,XFER,RLUP,RLDN		; 30h
			 ',', '.', '/',0xff, ' ',0x35,0x00,0x00,
		//	 INS, DEL,  ↑,  ←,  →,  ↓,HMCR,HELP		; 38h
			0x00,0x00,0x00,0x00,0x00,0x00,0x3e,0x00,
		//	<−>,<／>,<７>,<８>,<９>,<＊>,<４>,<５>		; 40h
			 '-', '/', '7', '8', '9', '*', '4', '5',
		//	<６>,<＋>,<１>,<２>,<３>,<＝>,<０>,<，>		; 48h
			 '6', '+', '1', '2', '3', '=', '0', ',',
		//	<．>,NFER,    ,    ,    ,    , f.1, f.2		; 50h
			 '.',0x51,0xff,0xff,0xff,0xff,0x62,0x63,
		//	 f.3, f.4, f.5, f.6, f.7, f.8, f.9, f.10	; 58h
			0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b
	},

	{	//												shift
		//	 ESC,  １,  ２,  ３,  ４,  ５,  ６,  ７		; 00h
			0x1b, '!','\"', '#', '$', '%', '&','\'',
		//	  ８,  ９,  ０,  −,  ＾,  ￥,  BS, TAB		; 08h
			 '(', ')', '0', '=', '`', '|',0x08,0x09,
		//	  Ｑ,  Ｗ,  Ｅ,  Ｒ,  Ｔ,  Ｙ,  Ｕ,  Ｉ		; 10h
			 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
		//	  Ｏ,  Ｐ,  ＠,  ［, Ret,  Ａ,  Ｓ,  Ｄ		; 18h
			 'O', 'P', '~', '{',0x0d, 'A', 'S', 'D',
		//	  Ｆ,  Ｇ,  Ｈ,  Ｊ,  Ｋ,  Ｌ,  ；,  ：		; 20h
			 'F', 'G', 'H', 'J', 'K', 'L', '+', '*',
		//    ］,  Ｚ,  Ｘ,  Ｃ,  Ｖ,  Ｂ,  Ｎ,  Ｍ		; 28h
			 '}', 'Z', 'X', 'C', 'V', 'B', 'N', 'M',
		//    ，,  ．,  ／,  ＿, SPC,XFER,RLUP,RLDN		; 30h
			 '<', '>', '?', '_', ' ',0xa5,0x00,0x00,
		//	 INS, DEL,  ↑,  ←,  →,  ↓,HMCR,HELP		; 38h
			0x00,0x00,0x00,0x00,0x00,0x00,0xae,0x00,
		//	<−>,<／>,<７>,<８>,<９>,<＊>,<４>,<５>		; 40h
			 '-', '/', '7', '8', '9', '*', '4', '5',
		//	<６>,<＋>,<１>,<２>,<３>,<＝>,<０>,<，>		; 48h
			 '6', '+', '1', '2', '3', '=', '0', ',',
		//	<．>,NFER,    ,    ,    ,    , f.1, f.2		; 50h
			 '.',0xa1,0xff,0xff,0xff,0xff,0x82,0x83,
		//	 f.3, f.4, f.5, f.6, f.7, f.8, f.9, f.10	; 58h
			0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b
	},

	{	//												CAPS
		//	 ESC,  １,  ２,  ３,  ４,  ５,  ６,  ７		; 00h
			0x1b, '1', '2', '3', '4', '5', '6', '7',
		//	  ８,  ９,  ０,  −,  ＾,  ￥,  BS, TAB		; 08h
			 '8', '9', '0', '-', '^','\\',0x08,0x09,
		//	  Ｑ,  Ｗ,  Ｅ,  Ｒ,  Ｔ,  Ｙ,  Ｕ,  Ｉ		; 10h
			 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
		//	  Ｏ,  Ｐ,  ＠,  ［, Ret,  Ａ,  Ｓ,  Ｄ		; 18h
			 'O', 'P', '@', '[',0x0d, 'A', 'S', 'D',
		//	  Ｆ,  Ｇ,  Ｈ,  Ｊ,  Ｋ,  Ｌ,  ；,  ：		; 20h
			 'F', 'G', 'H', 'J', 'K', 'L', ';', ':',
		//    ］,  Ｚ,  Ｘ,  Ｃ,  Ｖ,  Ｂ,  Ｎ,  Ｍ		; 28h
			 ']', 'Z', 'X', 'C', 'V', 'B', 'N', 'M',
		//    ，,  ．,  ／,  ＿, SPC,XFER,RLUP,RLDN		; 30h
			 ',', '.', '/',0xff, ' ',0x35,0x00,0x00,
		//	 INS, DEL,  ↑,  ←,  →,  ↓,HMCR,HELP		; 38h
			0x00,0x00,0x00,0x00,0x00,0x00,0x3e,0x00,
		//	<−>,<／>,<７>,<８>,<９>,<＊>,<４>,<５>		; 40h
			 '-', '/', '7', '8', '9', '*', '4', '5',
		//	<６>,<＋>,<１>,<２>,<３>,<＝>,<０>,<，>		; 48h
			 '6', '+', '1', '2', '3', '=', '0', ',',
		//	<．>,NFER,    ,    ,    ,    , f.1, f.2		; 50h
			 '.',0xa1,0xff,0xff,0xff,0xff,0x62,0x63,
		//	 f.3, f.4, f.5, f.6, f.7, f.8, f.9, f.10	; 58h
			0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b
	},

	{	//												shift + caps
		//	 ESC,  １,  ２,  ３,  ４,  ５,  ６,  ７		; 00h
			0x1b, '!','\"', '#', '$', '%', '&','\'',
		//	  ８,  ９,  ０,  −,  ＾,  ￥,  BS, TAB		; 08h
			 '(', ')', '0', '=', '`', '|',0x08,0x09,
		//	  Ｑ,  Ｗ,  Ｅ,  Ｒ,  Ｔ,  Ｙ,  Ｕ,  Ｉ		; 10h
			 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
		//	  Ｏ,  Ｐ,  ＠,  ［, Ret,  Ａ,  Ｓ,  Ｄ		; 18h
			 'o', 'p', '~', '{',0x0d, 'a', 's', 'd',
		//	  Ｆ,  Ｇ,  Ｈ,  Ｊ,  Ｋ,  Ｌ,  ；,  ：		; 20h
			 'f', 'g', 'h', 'j', 'k', 'l', '+', '*',
		//    ］,  Ｚ,  Ｘ,  Ｃ,  Ｖ,  Ｂ,  Ｎ,  Ｍ		; 28h
			 '}', 'z', 'x', 'c', 'v', 'b', 'n', 'm',
		//    ，,  ．,  ／,  ＿, SPC,XFER,RLUP,RLDN		; 30h
			 '<', '>', '?', '_', ' ',0xa5,0x00,0x00,
		//	 INS, DEL,  ↑,  ←,  →,  ↓,HMCR,HELP		; 38h
			0x00,0x00,0x00,0x00,0x00,0x00,0xae,0x00,
		//	<−>,<／>,<７>,<８>,<９>,<＊>,<４>,<５>		; 40h
			 '-', '/', '7', '8', '9', '*', '4', '5',
		//	<６>,<＋>,<１>,<２>,<３>,<＝>,<０>,<，>		; 48h
			 '6', '+', '1', '2', '3', '=', '0', ',',
		//	<．>,NFER,    ,    ,    ,    , f.1, f.2		; 50h
			 '.',0xa1,0xff,0xff,0xff,0xff,0x82,0x83,
		//	 f.3, f.4, f.5, f.6, f.7, f.8, f.9, f.10	; 58h
			0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b
	},

	{	//												kana (+caps)
		//	 ESC,  １,  ２,  ３,  ４,  ５,  ６,  ７		; 00h
			0x1b,0xc7,0xcc,0xb1,0xb3,0xb4,0xb5,0xd4,
		//	  ８,  ９,  ０,  −,  ＾,  ￥,  BS, TAB		; 08h
			0xd5,0xd6,0xdc,0xce,0xcd,0xb0,0x08,0x09,
		//	  Ｑ,  Ｗ,  Ｅ,  Ｒ,  Ｔ,  Ｙ,  Ｕ,  Ｉ		; 10h
			0xc0,0xc3,0xb2,0xbd,0xb6,0xdd,0xc5,0xc6,
		//	  Ｏ,  Ｐ,  ＠,  ［, Ret,  Ａ,  Ｓ,  Ｄ		; 18h
			0xd7,0xbe,0xde,0xdf,0x0d,0xc1,0xc4,0xbc,
		//	  Ｆ,  Ｇ,  Ｈ,  Ｊ,  Ｋ,  Ｌ,  ；,  ：		; 20h
			0xca,0xb7,0xb8,0xcf,0xc9,0xd8,0xda,0xb9,
		//    ］,  Ｚ,  Ｘ,  Ｃ,  Ｖ,  Ｂ,  Ｎ,  Ｍ		; 28h
			0xd1,0xc2,0xbb,0xbf,0xcb,0xba,0xd0,0xd3,
		//    ，,  ．,  ／,  ＿, SPC,XFER,RLUP,RLDN		; 30h
			0xc8,0xd9,0xd2,0xdb, ' ',0x35,0x00,0x00,
		//	 INS, DEL,  ↑,  ←,  →,  ↓,HMCR,HELP		; 38h
			0x00,0x00,0x00,0x00,0x00,0x00,0x3e,0x00,
		//	<−>,<／>,<７>,<８>,<９>,<＊>,<４>,<５>		; 40h
			 '-', '/', '7', '8', '9', '*', '4', '5',
		//	<６>,<＋>,<１>,<２>,<３>,<＝>,<０>,<，>		; 48h
			 '6', '+', '1', '2', '3', '=', '0', ',',
		//	<．>,NFER,    ,    ,    ,    , f.1, f.2		; 50h
			 '.',0x51,0xff,0xff,0xff,0xff,0x62,0x63,
		//	 f.3, f.4, f.5, f.6, f.7, f.8, f.9, f.10	; 58h
			0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b
	},

	{	//												kana + shift (+caps)
		//	 ESC,  １,  ２,  ３,  ４,  ５,  ６,  ７		; 00h
			0x1b,0xc7,0xcc,0xa7,0xa9,0xaa,0xab,0xac,
		//	  ８,  ９,  ０,  −,  ＾,  ￥,  BS, TAB		; 08h
			0xad,0xae,0xa6,0xce,0xcd,0xb0,0x08,0x09,
		//	  Ｑ,  Ｗ,  Ｅ,  Ｒ,  Ｔ,  Ｙ,  Ｕ,  Ｉ		; 10h
			0xc0,0xc3,0xa8,0xbd,0xb6,0xdd,0xc5,0xc6,
		//	  Ｏ,  Ｐ,  ＠,  ［, Ret,  Ａ,  Ｓ,  Ｄ		; 18h
			0xd7,0xbe,0xde,0xa2,0x0d,0xc1,0xc4,0xbc,
		//	  Ｆ,  Ｇ,  Ｈ,  Ｊ,  Ｋ,  Ｌ,  ；,  ：		; 20h
			0xca,0xb7,0xb8,0xcf,0xc9,0xd8,0xda,0xb9,
		//    ］,  Ｚ,  Ｘ,  Ｃ,  Ｖ,  Ｂ,  Ｎ,  Ｍ		; 28h
			0xa3,0xaf,0xbb,0xbf,0xcb,0xba,0xd0,0xd3,
		//    ，,  ．,  ／,  ＿, SPC,XFER,RLUP,RLDN		; 30h
			0xa4,0xa1,0xa5,0xdb, ' ',0xa5,0x00,0x00,
		//	 INS, DEL,  ↑,  ←,  →,  ↓,HMCR,HELP		; 38h
			0x00,0x00,0x00,0x00,0x00,0x00,0xae,0x00,
		//	<−>,<／>,<７>,<８>,<９>,<＊>,<４>,<５>		; 40h
			 '-', '/', '7', '8', '9', '*', '4', '5',
		//	<６>,<＋>,<１>,<２>,<３>,<＝>,<０>,<，>		; 48h
			 '6', '+', '1', '2', '3', '=', '0', ',',
		//	<．>,NFER,    ,    ,    ,    , f.1, f.2		; 50h
			 '.',0xa1,0xff,0xff,0xff,0xff,0x82,0x83,
		//	 f.3, f.4, f.5, f.6, f.7, f.8, f.9, f.10	; 58h
			0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b
	},

	{	//												grph
		//	 ESC,  １,  ２,  ３,  ４,  ５,  ６,  ７		; 00h
			0x1b,0xff,0xff,0xff,0xff,0xf2,0xf3,0xf4,
		//	  ８,  ９,  ０,  −,  ＾,  ￥,  BS, TAB		; 08h
			0xf5,0xf6,0xf7,0x8c,0x8b,0xf1,0x08,0x09,
		//	  Ｑ,  Ｗ,  Ｅ,  Ｒ,  Ｔ,  Ｙ,  Ｕ,  Ｉ		; 10h
			0x9c,0x9d,0xe4,0xe5,0xee,0xef,0xf0,0xe8,
		//	  Ｏ,  Ｐ,  ＠,  ［, Ret,  Ａ,  Ｓ,  Ｄ		; 18h
			0xe9,0x8d,0x8a,0xff,0x0d,0x9e,0x9f,0xe6,
		//	  Ｆ,  Ｇ,  Ｈ,  Ｊ,  Ｋ,  Ｌ,  ；,  ：		; 20h
			0xe7,0xec,0xed,0xea,0xeb,0x8e,0x89,0x94,
		//    ］,  Ｚ,  Ｘ,  Ｃ,  Ｖ,  Ｂ,  Ｎ,  Ｍ		; 28h
			0xff,0x80,0x81,0x82,0x83,0x84,0x85,0x86,
		//    ，,  ．,  ／,  ＿, SPC,XFER,RLUP,RLDN		; 30h
			0x87,0x88,0x97,0xff,0x20,0x35,0x00,0x00,
		//	 INS, DEL,  ↑,  ←,  →,  ↓,HMCR,HELP		; 38h
			0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x00,
		//	<−>,<／>,<７>,<８>,<９>,<＊>,<４>,<５>		; 40h
			 '-', '/',0x98,0x91,0x99,0x95,0xe1,0xe2,
		//	<６>,<＋>,<１>,<２>,<３>,<＝>,<０>,<，>		; 48h
			0xe3,0xe0,0x93,0x8f,0x92,0x96,0x9a,0x90,
		//	<．>,NFER,    ,    ,    ,    , f.1, f.2		; 50h
			0x9b,0x51,0xff,0xff,0xff,0xff,0xff,0xff,
		//	 f.3, f.4, f.5, f.6, f.7, f.8, f.9, f.10	; 58h
			0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
	},

	{	//												ctrl
		//	 ESC,  １,  ２,  ３,  ４,  ５,  ６,  ７		; 00h
			0x1b,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		//	  ８,  ９,  ０,  −,  ＾,  ￥,  BS, TAB		; 08h
			0xff,0xff,0xff,0xff,0x1e,0x1c,0x08,0x09,
		//	  Ｑ,  Ｗ,  Ｅ,  Ｒ,  Ｔ,  Ｙ,  Ｕ,  Ｉ		; 10h
			0x11,0x17,0x05,0x12,0x14,0x19,0x15,0x09,
		//	  Ｏ,  Ｐ,  ＠,  ［, Ret,  Ａ,  Ｓ,  Ｄ		; 18h
			0x0f,0x10,0x00,0x1b,0x0d,0x01,0x13,0x04,
		//	  Ｆ,  Ｇ,  Ｈ,  Ｊ,  Ｋ,  Ｌ,  ；,  ：		; 20h
			0x06,0x07,0x08,0x0a,0x0b,0x0c,0xff,0xff,
		//    ］,  Ｚ,  Ｘ,  Ｃ,  Ｖ,  Ｂ,  Ｎ,  Ｍ		; 28h
			0x1d,0x1a,0x18,0x03,0x16,0x02,0x0e,0x0d,
		//    ，,  ．,  ／,  ＿, SPC,XFER,RLUP,RLDN		; 30h
			0xff,0xff,0xff,0x1f,0x20,0xb5,0x00,0x00,
		//	 INS, DEL,  ↑,  ←,  →,  ↓,HMCR,HELP		; 38h
			0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x00,
		//	<−>,<／>,<７>,<８>,<９>,<＊>,<４>,<５>		; 40h
			 '-', '/', '7', '8', '9', '*', '4', '5',
		//	<６>,<＋>,<１>,<２>,<３>,<＝>,<０>,<，>		; 48h
			 '6', '+', '1', '2', '3', '=', '0', ',',
		//	<．>,NFER,    ,    ,    ,    , f.1, f.2		; 50h
			 '.',0xb1,0xff,0xff,0xff,0xff,0x92,0x93,
		//	 f.3, f.4, f.5, f.6, f.7, f.8, f.9, f.10	; 58h
			0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b
	}
};

