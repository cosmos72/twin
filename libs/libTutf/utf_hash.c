
typedef struct {
    byte utf32[3]; // actually store only 24 bits: Unicode needs slightly less than 21 bits
    byte ch;
    byte next;
} utf32_hash_entry;

typedef struct {
    TUTF_CONST hwfont * charset; /* charset -> utf32 conversion */
    utf32_hash_entry * base;
    hwfont cache_utf32;
    udat   n_power_of_2;
    byte   cache_ch;
    byte   index[1 /* actually n_power_of_2 */ ];
} utf32_hash_table;

#define NEXT_POWER_OF_2(n) ((n) < 0 ? -1 : (n) <= 1 ? 1 : (n) <= 2 ? 2 : (n) <= 4 ? 4 : (n) <= 8 ? 8 : (n) <= 16 ? 16 : (n) <= 32 ? 32 : (n) <= 64 ? 64 : (n) <= 128 ? 128 : (n) <= 256 ? 256 : -1)

TUTF_INLINE byte utf32_hash(hwfont h, udat mod_power_of_2) {
   return (h ^ (h >> 6) ^ (h >> 12) ^ (h >> 18)) & (mod_power_of_2 - 1);
}

TUTF_INLINE hwfont utf32_get(const utf32_hash_entry * e) {
    return (hwfont)e->utf32[0] | ((hwfont)e->utf32[1] << 8) | ((hwfont)e->utf32[2] << 16);
}

TUTF_INLINE void utf32_set(utf32_hash_entry * e, hwfont utf32) {
    e->utf32[0] = utf32 & 0xFF;
    e->utf32[1] = (utf32 >> 8) & 0xFF;
    e->utf32[2] = (utf32 >> 16) & 0xFF;
}

static void utf32_hash_insert_at(utf32_hash_table * table, byte offset, hwfont utf32, byte ch)
{
    if (table != NULL) {
        utf32_hash_entry * e = table->base + offset;
        byte hashkey = utf32_hash(utf32, table->n_power_of_2);

        utf32_set(e, utf32);
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

static utf32_hash_table * utf32_hash_create(TUTF_CONST hwfont charset[0x100], udat n, udat n_power_of_2)
{
    utf32_hash_table * table = calloc(1, (sizeof(utf32_hash_table)
                                          + (n ? n : 1) * sizeof(utf32_hash_entry) /* ensure at least one utf32_hash_entry */
                                          + (n_power_of_2 - 1) + alignment_mask)
                                      & ~(size_t)alignment_mask);
    if (table != NULL)
    {
        hwfont utf32;
        dat i;
        byte offset = 0;

        table->base = (utf32_hash_entry *)align_address(table->index + n_power_of_2);
        table->charset = charset;
        table->n_power_of_2 = n_power_of_2;
        
        for (i = 0; i < 0x100; i++)
        {
            if ((utf32 = charset[i]) == (udat)i)
                continue;
            utf32_hash_insert_at(table, offset, utf32, i);
            offset++;
        }
        /* cache has no 'not present' value... we must initialize it */
        table->cache_utf32 = charset[ table->cache_ch = ' '];
    }
    return table;
}

static hwfont utf32_hash_search(utf32_hash_table * table, hwfont utf32, byte ascii_is_preserved)
{
    if (table != NULL)
    {
        TUTF_CONST utf32_hash_entry *base, *e;
        byte ch, key0_visited;

        if (utf32 == table->cache_utf32)
            return table->cache_ch;
    
        if ((ascii_is_preserved && utf32 >= ' ' && utf32 <= '~') ||
            (utf32 & ~0x00ff) == 0xf000 || /* direct-to-font area */
            (utf32 < 0x100 && table->charset[utf32] == utf32)) /* does c have the same meaning in Unicode and this charset? */
        {
            return utf32 & 0x00ff;
        }
    
        base = table->base;
        e = base + table->index[ utf32_hash(utf32, table->n_power_of_2) ];
        key0_visited = tfalse;
    
        while (utf32_get(e) != utf32) {
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
        } else if (table->charset != Tutf_CP437_to_UTF_32) {
            /*
             * try to approximate going through UTF_32 -> CP437 -> ASCII
             * cannot be used if table->charset == Tutf_CP437_to_UTF_32: we would get infinite recursion!
             */
            ch = Tutf_UTF_32_to_ASCII(utf32);
        } else {
            ch = '?';
        }
        table->cache_utf32 = utf32;
        table->cache_ch = ch;
        return ch;
    }
    return '?';
}

