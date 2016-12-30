LINUX_ROOT="/home/linux/linux-4.7.2"

./from-kernel.bash ISO_8859_1  < "$LINUX_ROOT"/fs/nls/nls_iso8859-1.c  > iso_8859_1.h
./from-kernel.bash ISO_8859_2  < "$LINUX_ROOT"/fs/nls/nls_iso8859-2.c  > iso_8859_2.h
./from-kernel.bash ISO_8859_3  < "$LINUX_ROOT"/fs/nls/nls_iso8859-3.c  > iso_8859_3.h
./from-kernel.bash ISO_8859_4  < "$LINUX_ROOT"/fs/nls/nls_iso8859-4.c  > iso_8859_4.h
./from-kernel.bash ISO_8859_5  < "$LINUX_ROOT"/fs/nls/nls_iso8859-5.c  > iso_8859_5.h
./from-kernel.bash ISO_8859_6  < "$LINUX_ROOT"/fs/nls/nls_iso8859-6.c  > iso_8859_6.h
./from-kernel.bash ISO_8859_7  < "$LINUX_ROOT"/fs/nls/nls_iso8859-7.c  > iso_8859_7.h
# ./from-kernel.bash ISO_8859_8 < "$LINUX_ROOT"/fs/nls/nls_iso8859-8.c  > iso_8859_8.h
./from-kernel.bash ISO_8859_9  < "$LINUX_ROOT"/fs/nls/nls_iso8859-9.c  > iso_8859_9.h
./from-kernel.bash ISO_8859_13 < "$LINUX_ROOT"/fs/nls/nls_iso8859-13.c > iso_8859_13.h
./from-kernel.bash ISO_8859_14 < "$LINUX_ROOT"/fs/nls/nls_iso8859-14.c > iso_8859_14.h
./from-kernel.bash ISO_8859_15 < "$LINUX_ROOT"/fs/nls/nls_iso8859-15.c > iso_8859_15.h

./from-kernel.bash CP737  < "$LINUX_ROOT"/fs/nls/nls_cp737.c  > cp737.h
./from-kernel.bash CP775  < "$LINUX_ROOT"/fs/nls/nls_cp775.c  > cp775.h
./from-kernel.bash CP850  < "$LINUX_ROOT"/fs/nls/nls_cp850.c  > cp850.h
./from-kernel.bash CP852  < "$LINUX_ROOT"/fs/nls/nls_cp852.c  > cp852.h
./from-kernel.bash CP855  < "$LINUX_ROOT"/fs/nls/nls_cp855.c  > cp855.h
./from-kernel.bash CP857  < "$LINUX_ROOT"/fs/nls/nls_cp857.c  > cp857.h
./from-kernel.bash CP860  < "$LINUX_ROOT"/fs/nls/nls_cp860.c  > cp860.h
./from-kernel.bash CP861  < "$LINUX_ROOT"/fs/nls/nls_cp861.c  > cp861.h
./from-kernel.bash CP862  < "$LINUX_ROOT"/fs/nls/nls_cp862.c  > cp862.h
./from-kernel.bash CP863  < "$LINUX_ROOT"/fs/nls/nls_cp863.c  > cp863.h
./from-kernel.bash CP864  < "$LINUX_ROOT"/fs/nls/nls_cp864.c  > cp864.h
./from-kernel.bash CP865  < "$LINUX_ROOT"/fs/nls/nls_cp865.c  > cp865.h
./from-kernel.bash CP866  < "$LINUX_ROOT"/fs/nls/nls_cp866.c  > cp866.h
./from-kernel.bash CP869  < "$LINUX_ROOT"/fs/nls/nls_cp869.c  > cp869.h
./from-kernel.bash CP874  < "$LINUX_ROOT"/fs/nls/nls_cp874.c  > cp874.h
./from-kernel.bash CP932  < "$LINUX_ROOT"/fs/nls/nls_cp932.c  > cp932.h
./from-kernel.bash CP936  < "$LINUX_ROOT"/fs/nls/nls_cp936.c  > cp936.h
./from-kernel.bash CP949  < "$LINUX_ROOT"/fs/nls/nls_cp949.c  > cp949.h
./from-kernel.bash CP950  < "$LINUX_ROOT"/fs/nls/nls_cp950.c  > cp950.h

./from-kernel.bash CP1250  < "$LINUX_ROOT"/fs/nls/nls_cp1250.c  > cp1250.h
./from-kernel.bash CP1251  < "$LINUX_ROOT"/fs/nls/nls_cp1251.c  > cp1251.h
./from-kernel.bash CP1255  < "$LINUX_ROOT"/fs/nls/nls_cp1255.c  > cp1255.h

./from-kernel.bash MAC_CELTIC  < "$LINUX_ROOT"/fs/nls/nls_mac-celtic.c > mac_celtic.h

mac-celtic
mac-centeuro
mac-croatian
mac-cyrillic
mac-gaelic
mac-greek
mac-iceland
mac-inuit
mac-roman
mac-romanian
mac-turkish 
 