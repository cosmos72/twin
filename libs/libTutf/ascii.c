/*
 *
 *  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 */

hwfont T_CAT3(Tutf_,T_MAP(ASCII),_to_UTF_16)[0x100] = {
#define EL(x) T_UTF(UTF_16,x),
	T_LIST(ASCII,EL)
#undef EL
};

hwfont T_CAT(Tutf_CP437_to_,T_MAP(ASCII))[0x100] = {
        ' ','O','O','v','@','g','!','.','.','o','o','t','g','f','f','*',
	'>','<','|','!','q','S','_','|','^','v','>','<','L','-','^','v',
	
	' ','!','"','#','$','%','&','\'','(',')','*','+',',','-','.','/',
	'0','1','2','3','4','5','6','7','8','9',':',';','<','=','>','?',
	'@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
	'P','Q','R','S','T','U','V','W','X','Y','Z','[','\\',']','^','_',
	'`','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
	'p','q','r','s','t','u','v','w','x','y','z','{','|','}','~','@',

	'A','C','u','e','a','a','a','a','c','e','e','e','i','i','i','A',
	'A','E','E','e','o','o','o','u','u','y','O','U','c','L','Y','P',
	'f','a','o','u','n','N','a','o','?','-','-','%','%','!','<','>',
	'.','*','@','|','+','+','+','+','+','+','|','+','+','+','+','+',
	'+','+','+','+','-','+','+','+','+','+','+','+','+','=','+','+',
	'+','+','+','+','+','+','+','+','+','+','+','#','_','[',']','"',
	'a','B','c','p','S','s','m','t','P','H','W','d','&','p','e','n',
	'=','+','>','<','T','J','/','=','o','.','.','V','n','2','*',' '
};

hwfont T_CAT(Tutf_UTF_16_to_,T_MAP(ASCII))(hwfont c) {
    if (c < 0x20 /*' '*/ || c > 0x7E /*'~'*/) {
	/* not plain 7-bit ASCII, try to approximate */
	c = T_CAT(Tutf_CP437_to_,T_MAP(ASCII)) [ Tutf_UTF_16_to_CP437(c) ];
    }
    /* else c = c; */
    return c;
}

