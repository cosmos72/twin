#!/bin/bash
set -e

rm -f utf16_cache.sh
mkdir txt 2>/dev/null || true
cd txt

download() {
  echo "wget $2"
  wget -q -O "$1" "$2"
}

download UnicodeData.txt http://unicode.org/Public/UNIDATA/UnicodeData.txt

for i in 1 2 3 4 5 6 7 8 9 10 11 13 14 15 16
do
  download "iso8859_$i.txt" http://unicode.org/Public/MAPPINGS/ISO8859/"8859-$i.TXT"
done

download "adobe_stdenc.txt" http://unicode.org/Public/MAPPINGS/VENDORS/ADOBE/stdenc.txt
download "adobe_symbol.txt" http://unicode.org/Public/MAPPINGS/VENDORS/ADOBE/symbol.txt

for N in ARABIC.TXT CELTIC.TXT CENTEURO.TXT CROATIAN.TXT CYRILLIC.TXT \
         DEVANAGA.TXT DINGBATS.TXT FARSI.TXT GAELIC.TXT GREEK.TXT \
	 GUJARATI.TXT GURMUKHI.TXT HEBREW.TXT ICELAND.TXT INUIT.TXT \
         ROMAN.TXT ROMANIAN.TXT SYMBOL.TXT THAI.TXT TURKISH.TXT
	 # CHINSIMP.TXT CHINTRAD.TXT CORPCHAR.TXT JAPANESE.TXT KOREAN.TXT # these are already multi-byte... not useful for byte -> utf16 conversions
	 # UKRAINE.TXT # this is empty
do
  n="mac_`echo $N | tr 'A-Z' 'a-z'`"
  download "$n" http://unicode.org/Public/MAPPINGS/VENDORS/APPLE/"$N"
done
mv mac_centeuro.txt mac_centraleurope.txt
mv mac_devanaga.txt mac_devanagari.txt
mv mac_iceland.txt  mac_icelandic.txt


download "nextstep.txt" http://unicode.org/Public/MAPPINGS/VENDORS/NEXT/NEXTSTEP.TXT

for N in CP737.TXT CP775.TXT CP852.TXT CP855.TXT CP857.TXT CP860.TXT CP861.TXT \
         CP862.TXT CP863.TXT CP864.TXT CP865.TXT CP866.TXT CP869.TXT CP874.TXT
	 # CP437.TXT CP850.TXT are implemented manually
do
  n="`echo $N | tr 'A-Z' 'a-z'`"
  download "$n" http://unicode.org/Public/MAPPINGS/VENDORS/MICSFT/PC/"$N"
done

for N in CP874.TXT CP932.TXT CP936.TXT CP949.TXT CP950.TXT CP1250.TXT CP1251.TXT CP1252.TXT \
         CP1253.TXT CP1254.TXT CP1255.TXT CP1256.TXT CP1257.TXT CP1258.TXT
do
  n="`echo $N | tr 'A-Z' 'a-z'`"
  download "$n" http://unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WINDOWS/"$N"
done
 
for N in CYRILLIC.TXT GREEK.TXT ICELAND.TXT LATIN2.TXT ROMAN.TXT TURKISH.TXT
do
  n="ms_mac_`echo $N | tr 'A-Z' 'a-z'`"
  download "$n" http://unicode.org/Public/MAPPINGS/VENDORS/MICSFT/MAC/"$N"
done
mv ms_mac_iceland.txt ms_mac_icelandic.txt

for N in APL-ISO-IR-68.TXT ATARIST.TXT CP424.TXT CP856.TXT CP1006.TXT KOI8-R.TXT KOI8-U.TXT KZ1048.TXT
do
  n="`echo $N | tr 'A-Z-' 'a-z_'`"
  download "$n" http://unicode.org/Public/MAPPINGS/VENDORS/MISC/"$N"
done

cd ..
