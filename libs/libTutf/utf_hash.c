
typedef struct {
    hwfont utf16;
    byte ch;
    byte next;
} utf16_hash_entry;

typedef struct {
    TUTF_CONST hwfont * charset; /* charset -> utf16 conversion */
    utf16_hash_entry * base;
    udat   n_power_of_2;
    hwfont cache_utf16;
    byte   cache_ch;
    byte   index[3 /* actually n_power_of_2 */ ];
} utf16_hash_table;

#define NEXT_POWER_OF_2(n) ((n) < 0 ? -1 : (n) <= 1 ? 1 : (n) <= 2 ? 2 : (n) <= 4 ? 4 : (n) <= 8 ? 8 : (n) <= 16 ? 16 : (n) <= 32 ? 32 : (n) <= 64 ? 64 : (n) <= 128 ? 128 : (n) <= 256 ? 256 : -1)

TUTF_INLINE byte utf16_hash(hwfont h, udat mod_power_of_2) {
   return (h ^ (h >> 4) ^ (h >> 8) ^ (h >> 12)) & (mod_power_of_2 - 1);
}

static void utf16_hash_insert_at(utf16_hash_table * table, byte offset, hwfont utf16, byte ch)
{
    if (table != NULL) {
        utf16_hash_entry * e = table->base + offset;
        byte hashkey = utf16_hash(utf16, table->n_power_of_2);

        e->utf16 = utf16;
        e->ch = ch;
        e->next = table->index[hashkey]; /* either another entry with same hash, i.e. a collision, or zero */
        table->index[hashkey] = offset; /* build a linked list as needed */
    }
}

enum { alignment_mask = sizeof(void *) - 1 };

TUTF_INLINE void * align_address(void * address)
{
    return (void *)(((size_t)address + alignment_mask) & ~(size_t)alignment_mask);
}

static utf16_hash_table * utf16_hash_create(TUTF_CONST hwfont charset[0x100], udat n, udat n_power_of_2)
{
    utf16_hash_table * table = calloc(1, (sizeof(utf16_hash_table)
                                          + (n ? n : 1) * sizeof(utf16_hash_entry) /* ensure at least one utf16_hash_entry */
                                          + (n_power_of_2 - 3) + alignment_mask)
                                      & ~(size_t)alignment_mask);
    if (table != NULL)
    {
        hwfont utf16;
        dat i;
        byte offset = 0;

        table->base = (utf16_hash_entry *)align_address(table->index + n_power_of_2);
        table->charset = charset;
        table->n_power_of_2 = n_power_of_2;
        
        for (i = 0; i < 0x100; i++)
        {
            if ((utf16 = charset[i]) == (udat)i)
                continue;
            utf16_hash_insert_at(table, offset, utf16, i);
            offset++;
        }
        /* cache has no 'not present' value... we must initialize it */
        table->cache_utf16 = charset[ table->cache_ch = ' '];
    }
    return table;
}

static hwfont utf16_hash_search(utf16_hash_table * table, hwfont utf16, byte ascii_is_preserved)
{
    if (table != NULL)
    {
        TUTF_CONST utf16_hash_entry *base, *e;
        byte ch, key0_visited;

        if (utf16 == table->cache_utf16)
            return table->cache_ch;
    
        if ((ascii_is_preserved && utf16 >= ' ' && utf16 <= '~') ||
            (utf16 & ~0x00ff) == 0xf000 || /* direct-to-font area */
            (utf16 < 0x100 && table->charset[utf16] == utf16)) /* does c have the same meaning in Unicode and this charset? */
        {
            return utf16 & 0x00ff;
        }
    
        base = table->base;
        e = base + table->index[ utf16_hash(utf16, table->n_power_of_2) ];
        key0_visited = FALSE;
    
        while (e->utf16 != utf16) {
            key0_visited |= e == base;
            e = base + e->next;
            if (e == base && key0_visited)
            {
                e = NULL;
                break;
            }
        }
        if (e) {
            ch = e->ch;
        } else if (table->charset != Tutf_CP437_to_UTF_16) {
            /*
             * try to approximate going through UTF16 -> CP437 -> ASCII
             * cannot be used if table->charset == Tutf_CP437_to_UTF_16: we would get infinite recursion!
             */
            ch = Tutf_UTF_16_to_ASCII(utf16);
        } else {
            ch = '?';
        }
        table->cache_utf16 = utf16;
        table->cache_ch = ch;
        return ch;
    }
    return '?';
}

