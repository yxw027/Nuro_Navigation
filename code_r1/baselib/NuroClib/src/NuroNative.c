#include "../NuroNative.h"

#define NURO_ALIGNED4(x) ((((x) + 3) >> 2) << 2)
#define NURO_MOD4(x) (((int)x) & 0x3)
#define NURO_AL_FLAT4(x)  ((int)x - (((int)x) & 0x3))
#define NURO_STRING_HOLE 0x7efefeff
#define NURO_WSTRING_HOLE 0x7ffeffff

#define MAX_NUM_VAL        9

#define WF_LONG_MIN        0x80000000L
#define WF_LONG_MAX        0x7fffffffL

#define PREF_16_STR        "0x"
#define PREF_16_WSTR        ws16_pref
#define POSITIVE_SIGN_CHAR '+'
#define NEGTIVE_SIGN_CHAR  '-'
#define WF_INVAL           (-1)

#define SKIP_WHITESPACE(p) do for (; isspace_char_(*p); p++); while (0)

#define SKIP_A_SIGNER(p) do {\
    ___nuro_char_t c = *p;\
    if (POSITIVE_SIGN_CHAR == c || NEGTIVE_SIGN_CHAR == c)\
        p++;\
} while (0)

#define SKIP_WHITESPACE_W(p) do for (; isspace_wchar_(*p); p++); while (0)

#define SKIP_A_SIGNER_W(p) do {\
	___nuro_wchar_t c = *p;\
	if (POSITIVE_SIGN_CHAR == c || NEGTIVE_SIGN_CHAR == c)\
		p++;\
} while (0)

#define _TO_LOWER(c)  (( ('A' <= (c) ) && ( (c) <= 'Z' ) ) ? ((c) - 'A' + 'a') : (c)) 

#define _ABS(n) ((n) > 0 ? (n) : (-(n)))
#define _MIN(a, b) (((a) < (b)) ? (a) : (b))
#define _MAX(a, b) (((a) > (b)) ? (a) : (b))

#define _PI 3.141593

#define _____TrigonometricBase	1000

static ___nuro_long_t _____TrigonometricTable[91][2] = 
//base 1000
{
	{0,0},			{17,17},		{35,35},		{52,52},		{70,70},
	{87,87},		{105,105},		{122,123},		{139,141},		{156,158},
	{174,176},		{191,194},		{208,213},		{225,231},		{242,249},
	{259,268},		{276,287},		{292,306},		{309,325},		{326,344},
	{342,364},		{358,384},		{375,404},		{391,424},		{407,445},
	{423,466},		{438,488},		{454,510},		{469,532},		{485,554},
	{500,577},		{515,601},		{530,625},		{545,649},		{559,675},
	{574,700},		{588,727},		{602,754},		{616,781},		{629,810},
	{643,839},		{656,869},		{669,900},		{682,933},		{695,966},
	{707,1000},		{719,1036},		{731,1072},		{743,1111},		{755,1150},
	{766,1192},		{777,1235},		{788,1280},		{799,1327},		{809,1376},
	{819,1428},		{829,1483},		{839,1540},		{848,1600},		{857,1664},
	{866,1732},		{875,1804},		{883,1881},		{891,1963},		{899,2050},
	{906,2145},		{914,2246},		{921,2356},		{927,2475},		{934,2605},
	{940,2747},		{946,2904},		{951,3078},		{956,3271},		{961,3487},
	{966,3732},		{970,4011},		{974,4331},		{978,4705},		{982,5145},
	{985,5671},		{988,6314},		{990,7115},		{993,8144},		{995,9514},
	{996,11430},	{998,14301},	{999,19081},	{999,28636},	{1000,57290},
	{1000,100000}
};

// static variables
___nuro_wchar_t ws16_pref[3] = {'0', 'x', 0};

// static functions for local implements.
static ___nuro_int32_t isspace_char_(___nuro_int32_t c);

static ___nuro_long_t charval_(___nuro_int32_t val, ___nuro_int32_t base, ___nuro_int32_t max_num);

static ___nuro_int32_t checkbase_(const ___nuro_char_t *nptr);

static ___nuro_int32_t (*inc_table[17])(___nuro_int32_t *sum, ___nuro_int32_t val);

static ___nuro_bool_t is_in(const ___nuro_char_t *s, ___nuro_int32_t c);

static ___nuro_int32_t isspace_wchar_(___nuro_int32_t c);

static ___nuro_long_t wcharval_(___nuro_int32_t val, ___nuro_int32_t base, ___nuro_int32_t max_num);

static ___nuro_int32_t checkbase_w_(const ___nuro_wchar_t *nptr);

static ___nuro_bool_t is_in_w(const ___nuro_wchar_t *ws, ___nuro_int32_t wc);

static void strrev(___nuro_char_t *p);

static void wstrrev(___nuro_wchar_t *p);

static ___nuro_double_t _pow_i(___nuro_double_t num, ___nuro_int32_t n);

static ___nuro_double_t _pow_f(___nuro_double_t num,___nuro_double_t m);

static ___nuro_double_t _pow_ff(___nuro_double_t num,___nuro_double_t m);


//////////////////////////////////////////////////////////////////////////////////////////

nuVOID  *core_malloc_impl   (nuSIZE n)
{
	(void)n;
	return NULL;
}

nuVOID   core_free_impl     (nuVOID *p)
{
	(nuVOID)p;
}

nuVOID  *core_memset_impl   (nuVOID *s, nuINT c, nuSIZE n)
{
	___nuro_int8_t *ptr8_s = (___nuro_int8_t *)s;
	___nuro_int8_t *ptr8_e = (___nuro_int8_t *)s + n;
	___nuro_int8_t *ptr8_p = ptr8_s;
	___nuro_int8_t *ptr8_4n;
    ___nuro_int32_t c4 = (((___nuro_uint32_t)c) << 24 | ((___nuro_uint32_t)c) << 16 | ((___nuro_uint32_t)c) << 8 | ((___nuro_uint32_t)c));

	for (; (ptr8_p < ptr8_e) && (NURO_MOD4(ptr8_p) != 0); 
		*ptr8_p++ = (___nuro_int8_t)c);

	if (ptr8_p == ptr8_e)
		return s;

	ptr8_4n = ptr8_p + NURO_AL_FLAT4(n - (ptr8_p - ptr8_s));

	for (; ptr8_p < ptr8_4n; ptr8_p += 4)
	{
		*((___nuro_int32_t *)ptr8_p) = c4;
	}

	for (; (ptr8_p < ptr8_e); *ptr8_p++ = (___nuro_int8_t)c);
	
	return s;
}

// NOTE:when "dest == src" is true, do nothing, and the result is undefined!
nuVOID  *core_memcpy_impl   (nuVOID *dest, const nuVOID *src, nuSIZE n)
{
    if (dest != src)
    {
        ___nuro_int8_t *ptr8_form = (___nuro_int8_t *)src;
        ___nuro_int8_t *ptr8_to = (___nuro_int8_t *)dest;
        ___nuro_size_t n_f4 = (n >> 2);
        ___nuro_size_t n_copyed = 0;


		if (!(NURO_MOD4(ptr8_to)|| NURO_MOD4(ptr8_form)))
		{
			for (; n_copyed < n_f4; n_copyed += 4)
			{
				*((___nuro_int32_t *)(ptr8_to + n_copyed)) = *((___nuro_int32_t *)(ptr8_form + n_copyed));
			}
		}

        for (; n_copyed < n; n_copyed++)
        {
            ptr8_to[n_copyed] = ptr8_form[n_copyed];
        }
    }

    return dest;
}

// when "dest == src" is true, do nothing, end the result is undefined!
nuVOID  *core_memmove_impl  (nuVOID *dest, const nuVOID *src, nuSIZE n)
{
    if (dest < src)
    {
        dest = core_memcpy_impl(dest, src, n);
    }
    else if (dest > src) 
    {
        ___nuro_int8_t *ptr8_form = (___nuro_int8_t *)src + n;
        ___nuro_int8_t *ptr8_to = (___nuro_int8_t *)dest + n;
        ___nuro_size_t n_f4 = (n >> 2);
        ___nuro_size_t n_copyed = 0;

		if (!(NURO_MOD4(ptr8_to)|| NURO_MOD4(ptr8_form)))
		{
			for (; n_copyed < n_f4; n_copyed += 4)
			{
				*(___nuro_int32_t *)ptr8_to = *(___nuro_int32_t *)ptr8_form;
				ptr8_to -= sizeof (___nuro_int32_t);
				ptr8_form -= sizeof (___nuro_int32_t);
			}
		}

        for (; n_copyed < n; n_copyed++)
        {
            *ptr8_to-- = *ptr8_form--;
        }
    }
    
    return dest;
}

nuINT    core_memcmp_impl   (const nuVOID *s1, const nuVOID *s2, nuSIZE n)
{
    ___nuro_uint8_t *ptru8_s1 = (___nuro_uint8_t *)s1;
    ___nuro_uint8_t *ptru8_s2 = (___nuro_uint8_t *)s2;
    ___nuro_int32_t n_ret = 0;

    if (1 == n)
    {
        n_ret = (*ptru8_s1) - (*ptru8_s2);
    }
    else
    {
        while (n--)
        {
            if ((*ptru8_s1) > (*ptru8_s2))
            {
                n_ret = 1;
                break;
            }
            else if ((*ptru8_s1) < (*ptru8_s2))
            {
                n_ret = -1;
                break;
            }
            ptru8_s1++;
            ptru8_s2++;
        }
    }

    return n_ret;
}

/// format string print functions
nuINT    core_sscanf_impl   (const nuCHAR *buffer, const nuCHAR *format , ...)
{
	return 0;
}

nuINT    core_sprintf_impl  (nuCHAR *buffer, const nuCHAR *format, ...)
{
	return 0;
}

/// format string print functions
nuINT    core_swscanf_impl  (const nuWCHAR *buffer, const nuWCHAR *format , ...)
{
	return 0;
}

nuINT    core_swprintf_impl (nuWCHAR *buffer, const nuWCHAR *format, ...)
{
	return 0;
}

/// string functions

nuSIZE   core_strlen_impl   (const nuCHAR *s)
{
	const ___nuro_char_t *char_ptr;
	const ___nuro_long_t *longword_ptr;
	register ___nuro_long_t longword, magic_bits;

	for (char_ptr = s; 
		((___nuro_long_t)char_ptr & (sizeof (___nuro_long_t) - 1)) != 0;
		char_ptr++)
	{
		if ('\0' == *char_ptr)
			return char_ptr - s;
	}

	longword_ptr = (___nuro_long_t *)char_ptr;

	magic_bits = NURO_STRING_HOLE;

	for (;;)
	{
		longword = *longword_ptr++;

		if ((((longword + magic_bits) ^ ~longword) & ~magic_bits) != 0)
		{
			const ___nuro_char_t *cp = (const ___nuro_char_t *)(longword_ptr - 1);

			if (0 == cp[0])
				return cp - s;

			if (0 == cp[1])
				return cp - s + 1;

			if (0 == cp[2])
				return cp - s + 2;

			if (0 == cp[3])
				return cp - s + 3;
		}
	}
}

nuCHAR  *core_strcpy_impl   (nuCHAR *dest, const nuCHAR *src)
{
    ___nuro_char_t *char_ptr_des = dest;
    const ___nuro_char_t *char_ptr_src = src;

    for (;;)
    {
        if ('\0' == *char_ptr_src)
        {
            *char_ptr_des = '\0';
            break;
        }

        *char_ptr_des++ = *char_ptr_src++;
    }

    return dest;
}

nuCHAR  *core_strncpy_impl  (nuCHAR *dest, const nuCHAR *src, nuSIZE n)
{
    ___nuro_char_t *char_ptr_des = dest;
    const ___nuro_char_t *char_ptr_src = src;

    for (;;)
    {
		if (0 == n)
		{
			break;
		}

        if ('\0' == *char_ptr_src)
        {
            *char_ptr_des = '\0';
            break;
        }

        *char_ptr_des++ = *char_ptr_src++;
        n--;
    }

    return dest;
}

nuCHAR  *core_strcat_impl   (nuCHAR *dest, const nuCHAR *src)
{
    ___nuro_int32_t n = core_strlen_impl(dest);
    ___nuro_char_t *char_ptr_des = dest + n;
    const ___nuro_char_t *char_ptr_src = src;
   
    for (;;)
    {
        if ('\0' == *char_ptr_src)
        {
            *char_ptr_des = '\0';
            break;
        }

        *char_ptr_des++ = *char_ptr_src++;
    }
    
    return dest;
}

nuCHAR  *core_strncat_impl  (nuCHAR *dest, const nuCHAR *src, nuSIZE n)
{
    ___nuro_char_t *char_ptr_des = dest + core_strlen_impl(dest);
    const ___nuro_char_t *char_ptr_src = src;
   
    for (;;)
    {
        if ('\0' == *char_ptr_src || 0 == n)
        {
            *char_ptr_des = '\0';
            break;
        }

        *char_ptr_des++ = *char_ptr_src++;
        n--;
    }
    
    return dest;
}

nuINT    core_strcmp_impl   (const nuCHAR *s1, const nuCHAR *s2)
{
    const ___nuro_char_t *char_ptr_s1 = s1;
    const ___nuro_char_t *char_ptr_s2 = s2;
    nuINT nRet = 0;
    ___nuro_char_t c1, c2;

    for (;;)
    {
        c1 = *char_ptr_s1++;
        c2 = *char_ptr_s2++;

        if ('\0' == c1 && '\0' == c2)
        {
            break;
        }
        else if ('\0' == c1)
        {
            nRet = -1;
            break;
        }
        else if ('\0' == c2)
        {
            nRet = 1;
            break;
        }

        if (c1 > c2)
        {
            nRet = 1;
            break;
        }
        else if (c1 < c2)
        {
            nRet = -1;
            break;
        }
    }

    return nRet;
}

nuINT    core_strncmp_impl  (const nuCHAR *s1, const nuCHAR *s2, nuSIZE n)
{
    const ___nuro_char_t *char_ptr_s1 = s1;
    const ___nuro_char_t *char_ptr_s2 = s2;
    nuINT nRet = 0;
    ___nuro_char_t c1, c2;

    for (;;)
    {
        c1 = *char_ptr_s1++;
        c2 = *char_ptr_s2++;

        if (0 == n)
            break;

        if ('\0' == c1 && '\0' == c2)
        {
            break;
        }
        else if ('\0' == c1)
        {
            nRet = -1;
            break;
        }
        else if ('\0' == c2)
        {
            nRet = 1;
            break;
        }

        if (c1 > c2)
        {
            nRet = 1;
            break;
        }
        else if (c1 < c2)
        {
            nRet = -1;
            break;
        }

        n--;
    }

    return nRet;
}

nuINT    core_stricmp_impl   (const nuCHAR *s1, const nuCHAR *s2)
{
	const ___nuro_char_t *char_ptr_s1 = s1;
	const ___nuro_char_t *char_ptr_s2 = s2;
	nuINT nRet = 0;
	___nuro_char_t c1, c2;

	for (;;)
	{
		c1 = *char_ptr_s1++;
		c2 = *char_ptr_s2++;

		if ('\0' == c1 && '\0' == c2)
		{
			break;
		}
		else if ('\0' == c1)
		{
			nRet = -1;
			break;
		}
		else if ('\0' == c2)
		{
			nRet = 1;
			break;
		}

		c1 = _TO_LOWER(c1);
		c2 = _TO_LOWER(c2);

		if (c1 > c2)
		{
			nRet = 1;
			break;
		}
		else if (c1 < c2)
		{
			nRet = -1;
			break;
		}
	}

	return nRet;
}

nuINT    core_strnicmp_impl  (const nuCHAR *s1, const nuCHAR *s2, nuSIZE n)
{
	const ___nuro_char_t *char_ptr_s1 = s1;
	const ___nuro_char_t *char_ptr_s2 = s2;
	nuINT nRet = 0;
	___nuro_char_t c1, c2;

	for (;;)
	{
		c1 = *char_ptr_s1++;
		c2 = *char_ptr_s2++;

		if (0 == n)
			break;

		if ('\0' == c1 && '\0' == c2)
		{
			break;
		}
		else if ('\0' == c1)
		{
			nRet = -1;
			break;
		}
		else if ('\0' == c2)
		{
			nRet = 1;
			break;
		}

		c1 = _TO_LOWER(c1);
		c2 = _TO_LOWER(c2);

		if (c1 > c2)
		{
			nRet = 1;
			break;
		}
		else if (c1 < c2)
		{
			nRet = -1;
			break;
		}

		n--;
	}

	return nRet;
}

nuCHAR  *core_strstr_impl   (const nuCHAR *haystack, const nuCHAR *needle)
{
    const ___nuro_char_t *hys_ptr = haystack;
    const ___nuro_char_t *nd_ptr = needle;
    const ___nuro_char_t *match_ptr = NULL;

    for (; *hys_ptr; hys_ptr++)
    {
        if (*hys_ptr != *nd_ptr)
        {
            nd_ptr = needle;
            match_ptr = NULL;
            continue;
        }
        
        match_ptr = (!match_ptr) ? hys_ptr : match_ptr;

        if (!*++nd_ptr) 
        {
            break;
        }
    }

    return (___nuro_char_t *)match_ptr;
}

nuCHAR  *core_strchr_impl   (const nuCHAR *s, nuINT c)
{
    ___nuro_char_t *s_ptr = (___nuro_char_t *)s;

    for (; *s_ptr; s_ptr++)
    {
        if (*s_ptr != c)
            continue;
    }

    return ((*s_ptr) ? s_ptr : NULL);
}

nuCHAR  *core_strrchr_impl  (const nuCHAR *s, nuINT c)
{
    ___nuro_char_t *s_ptr = (___nuro_char_t *)s + core_strlen_impl(s) - 1;

    for (; s_ptr < s; s_ptr--)
    {
        if (*s_ptr != c)
            continue;
    }

    return ((s_ptr < s) ? NULL :s_ptr);
}


static ___nuro_int32_t isspace_char_(___nuro_int32_t c)
{   
    switch (c)
    {   
        case ' ': case '\t': case '\f': case '\v': case '\n': 
            return 1;
        default:
            return 0;
    }   
} 

static ___nuro_long_t charval_(___nuro_int32_t val, ___nuro_int32_t base, ___nuro_int32_t max_num)
{   
    ___nuro_long_t num = -1;
    ___nuro_int32_t nval = val - '0';

    if (0 <= nval && nval <= max_num)
    {
        num = nval;
    }
    else if (16 == base)
    {
        switch(val)
        {
            case 'A': case 'a': num = 10; break;
            case 'B': case 'b': num = 11; break;
            case 'C': case 'c': num = 12; break;
            case 'D': case 'd': num = 13; break;
            case 'E': case 'e': num = 14; break;
            case 'F': case 'f': num = 15; break;
        }
    }

    return num;
}

static ___nuro_int32_t checkbase_(const ___nuro_char_t *nptr)
{
    ___nuro_int32_t base = 10;
    ___nuro_int32_t stat = 0;

    for (; stat < 2; nptr++)
    {
        ___nuro_char_t c = *nptr;

        switch(stat)
        {
            case 0:
                {
                    switch(c)
                    {
                        case '0': base = 8; break;
                        default: break;
                    }
                    stat++;
                }
                break;
            case 1:
                {
                    switch(c)
                    {
                        case 'x': case 'X': base = 16; break;
                        default: break;
                    }
                    stat++;
                }
                break;
            default:
                break;
        }
    }

    return base;
}


static ___nuro_int32_t inc8_(___nuro_int32_t *sum, ___nuro_int32_t val)
{               
    ___nuro_int32_t sum_after = (*sum) * 8 + val;
    ___nuro_int32_t sum_before = (sum_after - val) / 8;

    if ((*sum) != sum_before)
        return 0;

    *sum = sum_after;
    return 1;
}   

static ___nuro_int32_t inc10_(___nuro_int32_t *sum, ___nuro_int32_t val)
{   
    ___nuro_int32_t sum_after = (*sum) * 10 + val;
    ___nuro_int32_t sum_before = (sum_after - val) / 10;

    if ((*sum) != sum_before)
        return 0;

    *sum = sum_after;
    return 1;
}   

static ___nuro_int32_t inc16_(___nuro_int32_t *sum, ___nuro_int32_t val)
{   
    ___nuro_int32_t sum_after = (*sum) * 16 + val;
    ___nuro_int32_t sum_before = (sum_after - val) / 16;

    if ((*sum) != sum_before)
        return 0;

    *sum = sum_after;
    return 1;
} 

static ___nuro_int32_t (*inc_table[17])(___nuro_int32_t *sum, ___nuro_int32_t val) =
{   
    NULL,  
    NULL,  NULL,  NULL,  NULL,  NULL,
    NULL,  NULL,  inc8_,  NULL,  inc10_,
    NULL,  NULL,  NULL,  NULL,  NULL, 
    inc16_
};  


nuLONG   core_strtol_impl   (const nuCHAR *nptr, nuCHAR **endptr, nuINT base)
{
    ___nuro_int32_t sum, val, max_num;
    ___nuro_int32_t sign_char = '+';
    ___nuro_int32_t (*incp)(___nuro_int32_t *, ___nuro_int32_t) = NULL;
    ___nuro_int32_t overflow = 0;

    SKIP_WHITESPACE(nptr);
    sign_char = *nptr;
    SKIP_A_SIGNER(nptr);

    base = ((0 == base) ? checkbase_(nptr) : base);

    if (16 == base && 
            0 == core_strnicmp_impl(nptr, PREF_16_STR, 2))
    {
        nptr += 2;
    }

    max_num = ((base - 1) > MAX_NUM_VAL ? MAX_NUM_VAL : (base - 1));

    incp = inc_table[base];

    for (sum = 0; 0 != *nptr; nptr++)
    {
        val = charval_(*nptr, base, max_num);

        if (WF_INVAL == val)
        {
            break;
        }

        if (!incp(&sum, val))
        {
            overflow = 1;
            break;
        }
    }

	if (endptr)
	{
		*endptr = (___nuro_char_t *)nptr;
	}

    if (overflow)
    {
        if (NEGTIVE_SIGN_CHAR == sign_char)
        {
            sum = WF_LONG_MIN;
        }
        else
        {
            sum = WF_LONG_MAX;
        }

    }
    else if (NEGTIVE_SIGN_CHAR == sign_char)
    {
        sum = (~sum) + 1;
    } 

    return sum;
}

nuDOUBLE core_strtod_impl   (const nuCHAR *nptr, nuCHAR **endptr)
{
	___nuro_double_t sum, sedn = 0.1;
	___nuro_int32_t val;
	___nuro_int32_t sign_char = '+';
	___nuro_int32_t mode = 0; //mode : 0 integer, 1 float
	
	SKIP_WHITESPACE(nptr);
	sign_char = *nptr;
	SKIP_A_SIGNER(nptr);

	for (sum = 0; 0 != *nptr; nptr++)
	{
		val = *nptr;
		if ('.' == val)
		{
			if (0 == mode)
			{
				mode = 1;
			}
			else
			{
				mode = 2;
				break;
			}
			continue;
		}

		val = val - '0';

		if (val < 0 && val > MAX_NUM_VAL)
		{
			break;
		}

		switch (mode)
		{
			case 0:
				sum = sum * 10.0 + val;
				break;
			case 1:
				sum = sum + val * sedn;
				sedn *= 0.1;
				break;
		}
	}	

	if (endptr)
		*endptr = (___nuro_char_t *)nptr;

	if ('-' == sign_char)
		sum = -sum;

	return sum;
}

static ___nuro_bool_t is_in(const ___nuro_char_t *s, ___nuro_int32_t c)
{
	___nuro_bool_t b_in = nuFALSE;

	for (; *s; s++)
	{
		if (*s == c)
		{
			b_in = nuTRUE;
			break;
		}
	}

	return b_in;
}

nuCHAR  *core_strtok_impl   (nuCHAR *str, const nuCHAR *delim)
{
	static ___nuro_char_t *sp = NULL;
	___nuro_char_t *sp_ret = NULL;

	if (str)
    {
		sp = str;
    }
    else if (!sp)
    {
        return NULL;
    }

	if (!*sp)
		return NULL;

	for (; is_in(delim, *sp); sp++);

	if (!*sp)
		return NULL;

	sp_ret = sp;

	for (; *sp; sp++)
	{
		if (is_in(delim, *sp))
		{
			*sp++ = '\0';
			break;
		}
	}

	return sp_ret;
}

nuCHAR  *core_strtok_r_impl (nuCHAR *str, const nuCHAR *delim, nuCHAR** pTag)
{
	___nuro_char_t *sp_ret = NULL;

	if (str)
		*pTag = str;

	if (!*(*pTag))
		return NULL;

	for (; is_in(delim, *(*pTag)); (*pTag)++);

	if (!*(*pTag))
		return NULL;

	sp_ret = (*pTag);

	for (; *(*pTag); (*pTag)++)
	{
		if (is_in(delim, *(*pTag)))
		{
			*(*pTag)++ = '\0';
			break;
		}
	}

	return sp_ret;
}

static void strrev(___nuro_char_t *p)
{
    if (NULL != p)
    {
        ___nuro_char_t c;
        ___nuro_char_t *q = p + core_strlen_impl(p) - 1;

        for (; p < q; p++, q--)
        {
            c = *q;
            *q = *p;
            *p = c;
        }
    }
}

nuCHAR  *core_itoa_impl     (nuLONG i, nuCHAR *nptr, nuINT base)
{
	// base 2 8 10 16
	___nuro_char_t *p = nptr, *p_num = nptr;
	___nuro_int32_t ic;
	___nuro_uint32_t v;
	___nuro_int32_t mask = base - 1;

	if (!nptr)
	{
		return nptr;
	}

	if (2 != base && 
		8 != base && 
	   10 != base && 
	   16 != base)
	{
		return nptr;
	}

	if (i < 0)
	{
		if (10 == base)
		{
			v = (___nuro_uint32_t)-i;
		}
		else
		{
			v = (___nuro_uint32_t)i;
		}
	}
	else
	{
		v = (___nuro_uint32_t)i;
	}
	
	switch (base)
	{
		case 10:
		{
			if (i < 0)
			{
				*p = '-';
				p++;
				p_num = p;
			}

			for (;;)
			{
				ic = v % base;
				*p++ = (___nuro_char_t)(ic  + '0');
				v /= base;

				if (0 == v)
					break;
			}
		}
		break;
		case 2:	
		{
			for (;;)
			{
				ic = v & mask;
				*p++ = (___nuro_char_t)(ic  + '0');
				v >>= 1;

				if (0 == v)
					break;
			}
		}
		break;
		case 8:	
		{
			for (;;)
			{
				ic = v & mask;
				*p++ = (___nuro_char_t)(ic  + '0');
				v >>= 3;

				if (0 == v)
					break;
			}
			
		}
		break;
		case 16:
		{
			for (;;)
			{
				ic = v & mask;
				if (ic < 10)
				{
					*p++ = (___nuro_char_t)(ic  + '0');
				}
				else
				{
					*p++ = (___nuro_char_t)(ic - 10 + 'a');
				}
				v >>= 4;

				if (0 == v)
					break;
			}
		}
		break;
	}

	*p = '\0';
	strrev(p_num);
	
	return nptr;
}

/// wide string functions
nuINT    core_wcslen_impl   (const nuWCHAR *ws)
{
	const ___nuro_wchar_t *wchar_ptr;
	const ___nuro_long_t *longword_ptr;
	register ___nuro_long_t longword, magic_bits;

	for (wchar_ptr = ws; 
		((___nuro_long_t)wchar_ptr & (sizeof (___nuro_long_t) - 1)) != 0;
		wchar_ptr++)
	{
		if ('\0' == *wchar_ptr)
			return wchar_ptr - ws;
	}

	longword_ptr = (___nuro_long_t *)wchar_ptr;

	magic_bits = NURO_WSTRING_HOLE;

	for (;;)
	{
		longword = *longword_ptr++;

		if ((((longword + magic_bits) ^ ~longword) & ~magic_bits) != 0)
		{
			const ___nuro_wchar_t *cp = (const ___nuro_wchar_t *)(longword_ptr - 1);

			if (0 == cp[0])
				return cp - ws;

			if (0 == cp[1])
				return cp - ws + 1;
		}
	}
}

nuWCHAR *core_wcscpy_impl   (nuWCHAR *wsDest, const nuWCHAR *wcsSrc)
{
	___nuro_wchar_t *wchar_ptr_des = wsDest;
	const ___nuro_wchar_t *wchar_ptr_src = wcsSrc;

	for (;;)
	{
		if (0 == *wchar_ptr_src)
		{
			*wchar_ptr_des = 0;
			break;
		}
	*wchar_ptr_des++ = *wchar_ptr_src++;
	}

	return wsDest;
}

nuWCHAR *core_wcsncpy_impl  (nuWCHAR *wsDest, const nuWCHAR *wcsSrc, nuINT nWcsLen)
{
	___nuro_wchar_t *wchar_ptr_des = wsDest;
	const ___nuro_wchar_t *wchar_ptr_src = wcsSrc;

	for (;;)
	{
		if (0 == nWcsLen)
		{
			break;
		}

		if ('\0' == *wchar_ptr_src)
		{
			*wchar_ptr_des = '\0';
			break;
		}

		*wchar_ptr_des++ = *wchar_ptr_src++;
		nWcsLen--;
	}

	return wsDest;
}

nuWCHAR *core_wcscat_impl   (nuWCHAR *wsDest, const nuWCHAR *wcsSrc)
{
	___nuro_int32_t n = core_wcslen_impl(wsDest);
	___nuro_wchar_t *wchar_ptr_des = wsDest + n;
	const ___nuro_wchar_t *wchar_ptr_src = wcsSrc;

	for (;;)
	{
		if (0 == *wchar_ptr_src)
		{
			*wchar_ptr_des = 0;
			break;
		}

		*wchar_ptr_des++ = *wchar_ptr_src++;
	}

	return wsDest;
}

nuWCHAR *core_wcsncat_impl  (nuWCHAR *wsDest, const nuWCHAR *wcsSrc, nuINT nWcsLen)
{
	___nuro_wchar_t *wchar_ptr_des = wsDest + core_wcslen_impl(wsDest);
	const ___nuro_wchar_t *wchar_ptr_src = wcsSrc;

	for (;;)
	{
		if (0 == *wchar_ptr_src || 0 == nWcsLen)
		{
			*wchar_ptr_des = 0;
			break;
		}

		*wchar_ptr_des++ = *wchar_ptr_src++;
		nWcsLen--;
	}

	return wsDest;
}

nuINT    core_wcscmp_impl   (const nuWCHAR *ws1, const nuWCHAR *ws2)
{
	const ___nuro_wchar_t *wchar_ptr_s1 = ws1;
	const ___nuro_wchar_t *wchar_ptr_s2 = ws2;
	nuINT nRet = 0;
	___nuro_wchar_t wc1, wc2;

	for (;;)
	{
		wc1 = *wchar_ptr_s1++;
		wc2 = *wchar_ptr_s2++;

		if (0 == wc1 && 0 == wc2)
		{
			break;
		}
		else if (0 == wc1)
		{
			nRet = -1;
			break;
		}
		else if (0 == wc2)
		{
			nRet = 1;
			break;
		}

		if (wc1 > wc2)
		{
			nRet = 1;
			break;
		}
		else if (wc1 < wc2)
		{
			nRet = -1;
			break;
		}
	}

	return nRet;
}

nuINT    core_wcsncmp_impl  (const nuWCHAR *ws1, const nuWCHAR *ws2, nuSIZE n)
{
	const ___nuro_wchar_t *wchar_ptr_s1 = ws1;
	const ___nuro_wchar_t *wchar_ptr_s2 = ws2;
	nuINT nRet = 0;
	___nuro_wchar_t wc1, wc2;

	for (;;)
	{
		wc1 = *wchar_ptr_s1++;
		wc2 = *wchar_ptr_s2++;

		if (0 == n)
			break;

		if (0 == wc1 && 0 == wc2)
		{
			break;
		}
		else if (0 == wc1)
		{
			nRet = -1;
			break;
		}
		else if (0 == wc2)
		{
			nRet = 1;
			break;
		}

		if (wc1 > wc2)
		{
			nRet = 1;
			break;
		}
		else if (wc1 < wc2)
		{
			nRet = -1;
			break;
		}

		n--;
	}

	return nRet;
}

nuINT    core_wcsicmp_impl   (const nuWCHAR *ws1, const nuWCHAR *ws2)
{
	const ___nuro_wchar_t *wchar_ptr_s1 = ws1;
	const ___nuro_wchar_t *wchar_ptr_s2 = ws2;
	nuINT nRet = 0;
	___nuro_wchar_t wc1, wc2;

	for (;;)
	{
		wc1 = *wchar_ptr_s1++;
		wc2 = *wchar_ptr_s2++;

		if (0 == wc1 && 0 == wc2)
		{
			break;
		}
		else if (0 == wc1)
		{
			nRet = -1;
			break;
		}
		else if (0 == wc2)
		{
			nRet = 1;
			break;
		}

		wc1 = _TO_LOWER(wc1);
		wc2 = _TO_LOWER(wc2);

		if (wc1 > wc2)
		{
			nRet = 1;
			break;
		}
		else if (wc1 < wc2)
		{
			nRet = -1;
			break;
		}
	}

	return nRet;
}

nuINT    core_wcsnicmp_impl  (const nuWCHAR *ws1, const nuWCHAR *ws2, nuSIZE n)
{
	const ___nuro_wchar_t *wchar_ptr_s1 = ws1;
	const ___nuro_wchar_t *wchar_ptr_s2 = ws2;
	nuINT nRet = 0;
	___nuro_wchar_t wc1, wc2;

	for (;;)
	{
		wc1 = *wchar_ptr_s1++;
		wc2 = *wchar_ptr_s2++;

		if (0 == n)
			break;

		if (0 == wc1 && 0 == wc2)
		{
			break;
		}
		else if (0 == wc1)
		{
			nRet = -1;
			break;
		}
		else if (0 == wc2)
		{
			nRet = 1;
			break;
		}

		wc1 = _TO_LOWER(wc1);
		wc2 = _TO_LOWER(wc2);

		if (wc1 > wc2)
		{
			nRet = 1;
			break;
		}
		else if (wc1 < wc2)
		{
			nRet = -1;
			break;
		}

		n--;
	}

	return nRet;
}

nuWCHAR *core_wcsstr_impl   (const nuWCHAR *haystack, const nuWCHAR *needle)
{
	const ___nuro_wchar_t *hys_ptr = haystack;
	const ___nuro_wchar_t *nd_ptr = needle;
	const ___nuro_wchar_t *match_ptr = NULL;

	for (; *hys_ptr; hys_ptr++)
	{
		if (*hys_ptr != *nd_ptr)
		{
			nd_ptr = needle;
			match_ptr = NULL;
			continue;
		}

		match_ptr = (!match_ptr) ? hys_ptr : match_ptr;

		if (!*++nd_ptr) 
		{
			break;
		}
	}

	return (___nuro_wchar_t *)match_ptr;
}

nuWCHAR *core_wcschr_impl   (const nuWCHAR *ws, nuINT wc)
{
	___nuro_wchar_t *ws_ptr = (___nuro_wchar_t *)ws;

	for (; *ws_ptr; ws_ptr++)
	{
		if (*ws_ptr != wc)
			continue;
	}

	return ((*ws_ptr) ? ws_ptr : NULL);
}

nuWCHAR *core_wcsrchr_impl  (const nuWCHAR *ws, nuINT wc)
{
	___nuro_wchar_t *ws_ptr = (___nuro_wchar_t *)ws + core_wcslen_impl(ws) - 1;

	for (; ws_ptr < ws; ws_ptr--)
	{
		if (*ws_ptr != wc)
			continue;
	}

	return ((ws_ptr < ws) ? NULL : ws_ptr);
}

static ___nuro_int32_t isspace_wchar_(___nuro_int32_t c)
{   
	switch (c)
	{   
	case ' ': case '\t': case '\f': case '\v': case '\n': 
		return 1;
	default:
		return 0;
	}   
} 

static ___nuro_long_t wcharval_(___nuro_int32_t val, ___nuro_int32_t base, ___nuro_int32_t max_num)
{   
	___nuro_long_t num = -1;
	___nuro_int32_t nval = val - '0';

	if (0 <= nval && nval <= max_num)
	{
		num = nval;
	}
	else if (16 == base)
	{
		switch(val)
		{
		case 'A': case 'a': num = 10; break;
		case 'B': case 'b': num = 11; break;
		case 'C': case 'c': num = 12; break;
		case 'D': case 'd': num = 13; break;
		case 'E': case 'e': num = 14; break;
		case 'F': case 'f': num = 15; break;
		}
	}

	return num;
}

static ___nuro_int32_t checkbase_w_(const ___nuro_wchar_t *nptr)
{
	___nuro_int32_t base = 10;
	___nuro_int32_t stat = 0;

	for (; stat < 2; nptr++)
	{
		___nuro_wchar_t wc = *nptr;

		switch(stat)
		{
		case 0:
			{
				switch(wc)
				{
				case '0': base = 8; break;
				default: break;
				}
				stat++;
			}
			break;
		case 1:
			{
				switch(wc)
				{
				case 'x': case 'X': base = 16; break;
				default: break;
				}
				stat++;
			}
			break;
		default:
			break;
		}
	}

	return base;
}

nuLONG   core_wcstol_impl   (const nuWCHAR *nptr, nuWCHAR **endptr, nuINT base)
{
	___nuro_int32_t sum, val, max_num;
	___nuro_int32_t sign_char = '+';
	___nuro_int32_t (*incp)(___nuro_int32_t *, ___nuro_int32_t) = NULL;
	___nuro_int32_t overflow = 0;

	SKIP_WHITESPACE_W(nptr);
	sign_char = *nptr;
	SKIP_A_SIGNER_W(nptr);

	base = ((0 == base) ? checkbase_w_(nptr) : base);

	if (16 == base && 
		0 == core_wcsnicmp_impl(nptr, PREF_16_WSTR, 2))
	{
		nptr += 2;
	}

	max_num = ((base - 1) > MAX_NUM_VAL ? MAX_NUM_VAL : (base - 1));

	incp = inc_table[base];

	for (sum = 0; 0 != *nptr; nptr++)
	{
		val = wcharval_(*nptr, base, max_num);

		if (WF_INVAL == val)
		{
			break;
		}

		if (!incp(&sum, val))
		{
			overflow = 1;
			break;
		}
	}

	if (endptr)
	{
		*endptr = (___nuro_wchar_t *)nptr;
	}

	if (overflow)
	{
		if (NEGTIVE_SIGN_CHAR == sign_char)
		{
			sum = WF_LONG_MIN;
		}
		else
		{
			sum = WF_LONG_MAX;
		}

	}
	else if (NEGTIVE_SIGN_CHAR == sign_char)
	{
		sum = (~sum) + 1;
	} 

	return sum;
}

nuDOUBLE core_wcstod_impl   (const nuWCHAR *nptr, nuWCHAR **endptr)
{
	___nuro_double_t sum, sedn = 0.1;
	___nuro_int32_t val;
	___nuro_int32_t sign_wchar = '+';
	___nuro_int32_t mode = 0; //mode : 0 integer, 1 float

	SKIP_WHITESPACE_W(nptr);
	sign_wchar = *nptr;
	SKIP_A_SIGNER_W(nptr);

	for (sum = 0; 0 != *nptr; nptr++)
	{
		val = *nptr;
		if ('.' == val)
		{
			if (0 == mode)
			{
				mode = 1;
			}
			else
			{
				mode = 2;
				break;
			}
			continue;
		}

		val = val - '0';

		if (val < 0 && val > MAX_NUM_VAL)
		{
			break;
		}

		switch (mode)
		{
		case 0:
			sum = sum * 10.0 + val;
			break;
		case 1:
			sum = sum + val * sedn;
			sedn *= 0.1;
			break;
		}
	}	

	if (endptr)
		*endptr = (___nuro_wchar_t *)nptr;

	if ('-' == sign_wchar)
		sum = -sum;

	return sum;
}

static ___nuro_bool_t is_in_w(const ___nuro_wchar_t *ws, ___nuro_int32_t wc)
{
	___nuro_bool_t b_in = nuFALSE;

	for (; *ws; ws++)
	{
		if (*ws == wc)
		{
			b_in = nuTRUE;
			break;
		}
	}

	return b_in;
}

nuWCHAR *core_wcstok_impl   (nuWCHAR *wstr, const nuWCHAR *delim)
{
	static ___nuro_wchar_t *wsp = NULL;
	___nuro_wchar_t *wsp_ret = NULL;

	if (wstr)
	{
		wsp = wstr;
	}
	else if (!wsp)
	{
		return NULL;
	}

	if (!*wsp)
		return NULL;

	for (; is_in_w(delim, *wsp); wsp++);

	if (!*wsp)
		return NULL;

	wsp_ret = wsp;

	for (; *wsp; wsp++)
	{
		if (is_in_w(delim, *wsp))
		{
			*wsp++ = '\0';
			break;
		}
	}

	return wsp_ret;
}

nuWCHAR *core_wcstok_r_impl (nuWCHAR *wstr, const nuWCHAR *delim, nuWCHAR** pTag)
{
	___nuro_wchar_t *wsp_ret = NULL;

	if (wstr)
		*pTag = wstr;

	if (!*(*pTag))
		return NULL;

	for (; is_in_w(delim, *(*pTag)); (*pTag)++);

	if (!*(*pTag))
		return NULL;

	wsp_ret = (*pTag);

	for (; *(*pTag); (*pTag)++)
	{
		if (is_in_w(delim, *(*pTag)))
		{
			*(*pTag)++ = '\0';
			break;
		}
	}

	return wsp_ret;
}

static void wstrrev(___nuro_wchar_t *p)
{
	if (NULL != p)
	{
		___nuro_wchar_t c;
		___nuro_wchar_t *q = p + core_wcslen_impl(p) - 1;

		for (; p < q; p++, q--)
		{
			c = *q;
			*q = *p;
			*p = c;
		}
	}
}

nuWCHAR *core_itow_impl     (nuLONG i, nuWCHAR *nptr, nuINT base)
{
	// base 2 8 10 16
	___nuro_wchar_t *p = nptr, *p_num = nptr;
	___nuro_int32_t ic;
	___nuro_uint32_t v;
	___nuro_int32_t mask = base - 1;

	if (!nptr)
	{
		return nptr;
	}

	if (2 != base && 
		8 != base && 
		10 != base && 
		16 != base)
	{
		return nptr;
	}

	if (i < 0)
	{
		if (10 == base)
		{
			v = (___nuro_uint32_t)-i;
		}
		else
		{
			v = (___nuro_uint32_t)i;
		}
	}
	else
	{
		v = (___nuro_uint32_t)i;
	}

	switch (base)
	{
	case 10:
		{
			if (i < 0)
			{
				*p = '-';
				p++;
				p_num = p;
			}

			for (;;)
			{
				ic = v % base;
				*p++ = (___nuro_char_t)(ic  + '0');
				v /= base;

				if (0 == v)
					break;
			}
		}
		break;
	case 2:	
		{
			for (;;)
			{
				ic = v & mask;
				*p++ = (___nuro_char_t)(ic  + '0');
				v >>= 1;

				if (0 == v)
					break;
			}
		}
		break;
	case 8:	
		{
			for (;;)
			{
				ic = v & mask;
				*p++ = (___nuro_char_t)(ic  + '0');
				v >>= 3;

				if (0 == v)
					break;
			}

		}
		break;
	case 16:
		{
			for (;;)
			{
				ic = v & mask;
				if (ic < 10)
				{
					*p++ = (___nuro_char_t)(ic  + '0');
				}
				else
				{
					*p++ = (___nuro_char_t)(ic - 10 + 'a');
				}
				v >>= 4;

				if (0 == v)
					break;
			}
		}
		break;
	}

	*p = '\0';
	wstrrev(p_num);

	return nptr;
}


/// string transform function
nuCHAR  *core_WcstoAsc_impl       (nuCHAR *pDes, nuSIZE nAscMax, const nuWCHAR *pSrc, nuSIZE nWcsMax)
{
	if(NULL == pDes || NULL == pSrc)
	{
		return NULL;
	}
	
	{
		___nuro_uint32_t i = 0;
		
		for(i = 0; 
			(i < nWcsMax) && (i < nAscMax) && (pSrc[i] !=  0) && (pSrc[i] < 256); 
			i++)
		{
			pDes[i] = (___nuro_char_t)pSrc[i];
		}

		pDes[i] = 0;
	}

	return pDes;
}

nuWCHAR *core_AsctoWcs_impl       (nuWCHAR *pDes, nuSIZE nWcsMax, const nuCHAR *pSrc, nuSIZE nAscMax)
{
	if(pDes == NULL || pSrc == NULL)
	{
		return NULL;
	}

	{
		___nuro_uint32_t i=0;
		
		for(i = 0; 
			(i < nWcsMax) && (i < nAscMax) && (0 != pSrc[i]); 
			i++)
		{
			pDes[i] = pSrc[i];
		}

		pDes[i] = 0;
	}

	return pDes;
}

nuBOOL   core_WcsSameAs_impl      (nuWCHAR *pA, nuWCHAR *pB)
{
	___nuro_wchar_t A = *pA, B = *pB;
	
	if (A == B)
	{
		return nuTRUE;
	}

	A = _TO_LOWER(A);
	B = _TO_LOWER(B);

	if (A == B)
	{
		return nuTRUE;
	}
	else
	{
		return nuFALSE;
	}
}

nuBOOL   core_WcsFuzzyJudge_O_impl(nuWCHAR *pDes, nuWCHAR *pKey, nuUINT desLen, nuUINT keyLen)
{
	if (desLen<keyLen)
	{
		return nuFALSE;
	}
	
	{
		___nuro_uint32_t i = 0;

		for(i = 0; i < desLen; ++i)
		{
			if(core_WcsSameAs_impl(pDes + i, pKey))
			{
				if (1 == keyLen)
				{
					return nuTRUE;
				}
				else
				{
					--keyLen;
					++pKey;
				}
			}
			else if ((desLen - i - 1) < keyLen)
			{
				return nuFALSE;
			}
		}
	}

	return nuFALSE;
}

nuBOOL   core_WcsFuzzyJudge_impl  (nuWCHAR *pDes, nuWCHAR *pKey, nuUINT desLen, nuUINT keyLen, nuWORD endWord)
{
	if (!endWord)
	{
		return core_WcsFuzzyJudge_O_impl(pDes, pKey, desLen, keyLen);
	}

	if (desLen<keyLen)
	{
		return nuFALSE;
	}

	{
		___nuro_uint32_t i = 0;

		for (i = 0; i < desLen; ++i)
		{
			if (pDes[i] == endWord)
			{
				return nuFALSE;
			}

			if (core_WcsSameAs_impl(pDes + i, pKey))
			{
				if (1 == keyLen)
				{
					return nuTRUE;
				}
				else
				{
					--keyLen;
					++pKey;
				}
			}
			else if ((desLen - i - 1) < keyLen)
			{
				return nuFALSE;
			}
		}
	}
	return nuFALSE;
}

nuINT    core_WcsKeyMatch_impl    (nuWCHAR *pDes, nuWCHAR *pKey, nuUINT desLen, nuUINT keyLen)
{
	___nuro_uint32_t i;

	if (!desLen || !keyLen || desLen < keyLen)
	{
		return -1;
	}
	
	for (i = 0; i < desLen; ++i)
	{
		if ((desLen - i) < keyLen)
		{
			return -1;
		}

		if (!core_wcsncmp_impl(&pDes[i], pKey, keyLen))
		{
			return i;
		}
	}

	return -1;
}

nuBOOL   core_WcsDelSpSign_impl   (nuWCHAR *pDes, nuWCHAR *pSrc, nuUINT desLen, nuUINT srcLen)
{
	if (!desLen || !srcLen || desLen<srcLen)
	{
		return nuFALSE;
	}

	{
		___nuro_uint32_t i;
		___nuro_int32_t iCount = 0;
		
		for (i = 0; i < srcLen; i++)
		{
			if ('-' != pSrc[i] &&
				' ' != pSrc[i])
			{
				pDes[iCount++] = pSrc[i];
			}

			if (0 == pSrc[i])
			{
				break;
			}
		}
	}

	return nuTRUE;
}



nuDOUBLE core_cos_impl  (nuLONG nAngle)
{
	___nuro_int32_t i = 0;
	___nuro_double_t dcos = 0, epsi = 1;
	___nuro_double_t x = nAngle * _PI / 180;

	do
	{
		dcos = dcos + epsi;
		i++;
		epsi = x * x * epsi / ((2 * i - 1) * (2 * i));
		epsi = -epsi;
	} 
	while(_ABS(epsi)>=0.0000002);

	return dcos;
}

nuDOUBLE core_sin_impl  (nuLONG nAngle)
{
	___nuro_int32_t i = 0;
	___nuro_double_t x = nAngle * _PI / 180;
	___nuro_double_t dsin = 0, epsi = x;

	do
	{
		dsin = dsin + epsi;
		i++;
		epsi = epsi * x * x / ((2 * i) * (2 * i + 1));
		epsi = -epsi; 
	}
	while(_ABS(epsi)>=0.0000002);

	return dsin;
}

nuDOUBLE core_tan_impl  (nuLONG nAngle)
{
	return core_sin_impl(nAngle) / core_cos_impl(nAngle);
}

nuDOUBLE core_atan2_impl(nuLONG dy, nuLONG dx)
{
	return core_latan2_impl(dy, dx) * _PI / 180;
}

nuDOUBLE core_sqrt_impl (nuDOUBLE dValue)
{
	___nuro_double_t x0, x1;

	x1 = dValue / 2;

	if (dValue < 0000000.1)
	{
		return 0.0;
	}

	do 
	{
		x0 = x1;
		x1 = (x0 + dValue / x0) / 2;
	}
	while (_ABS(x0 - x1) >= 0.00001);

	return x1;
}

static ___nuro_double_t _pow_i(___nuro_double_t num, ___nuro_int32_t n)
{
   ___nuro_double_t powint=1;
   ___nuro_int32_t i;

   for (i = 1; i <= n; i++) 
   {
	   powint *= num;
   }
   
   return powint;
}

static ___nuro_double_t _pow_f(___nuro_double_t num,___nuro_double_t m) 
{
    ___nuro_int32_t i, j;
    ___nuro_double_t powf = 0, x, tmpm = 1;
    
	x = num - 1;
    
	for (i = 1; tmpm > 1e-12 || tmpm < -1e-12; i++)
	{	       
		for(j=1,tmpm=1;j<=i;j++)
		{
			tmpm *= (m - j + 1) * x / j;
		}

		powf += tmpm;
	}
    
	return powf + 1;
}

static ___nuro_double_t _pow_ff(___nuro_double_t num,___nuro_double_t m)
{
    if (num == 0 && m != 0) 
	{
		return 0;
	}
	else if (num == 0 && m == 0)
	{
		return 1;
	}
    else if (num < 0 && m - (___nuro_int32_t)m != 0)
	{
		return 0;
	}

    if (num > 2) 
	{
	    num = 1 / num;
		m = -m;	
	}

	if (m < 0)
	{
		return 1 /_pow_ff(num,-m);
	}

    if ((m - (___nuro_int32_t)m) == 0)
	{
		return _pow_i(num, (___nuro_int32_t)m);
	}
    else 
	{
		return _pow_f(num, m - (___nuro_int32_t)m) * _pow_i(num,(___nuro_int32_t)m);
	}
}

nuDOUBLE core_pow_impl  (nuDOUBLE base, nuDOUBLE exp)
{
	return _pow_ff(base, exp);
}


nuLONG   core_GetlBase_impl()
{
	return _____TrigonometricBase;
}

nuLONG   core_latan2_impl  (nuLONG dy, nuLONG dx)
{
	___nuro_long_t i,num;

	if (dx == 0)
	{
		if (dy == 0)	
		{
			return 0;
		}
		else if ( dy > 0 )
		{
			return 90;
		}
		else
		{
			return 270;
		}
	}
	if (dy == 0)
	{
		if (dx > 0)	
		{
			return 0;
		}
		else		
		{
			return 180;
		}
	}



	if (dx > 0 && dy > 0)
	{
		if (dx > dy)
		{
			num = dy * _____TrigonometricBase / dx;
		}
		else
		{
			num = dx*_____TrigonometricBase / dy;
		}
		for (i = 0; i < 45; i++)
		{
			if (_____TrigonometricTable[i][1] <= num && num <= _____TrigonometricTable[i+1][1])
			{
				if (dx > dy)
				{
					return i + 1;
				}
				else
				{
					return 90 - (i + 1);
				}
			}
		}
	}
	else if (dx < 0 && dy > 0)
	{
		if ((-dx) > dy)
		{
			num = dy * _____TrigonometricBase / (-dx);
		}
		else
		{
			num=(-dx) * _____TrigonometricBase / dy;
		}
		for (i = 0; i < 45; i++)
		{
			if (_____TrigonometricTable[i][1] <= num && num <= _____TrigonometricTable[i+1][1])
			{
				if ((-dx) > dy)
				{
					return 180 - (i + 1);
				}
				else
				{
					return 90 + (i + 1);
				}
			}
		}
	}
	else if (dx < 0 && dy < 0)
	{
		if ((-dx) > (-dy))
		{
			num = (-dy) * _____TrigonometricBase / (-dx);
		}
		else
		{
			num = (-dx) * _____TrigonometricBase / (-dy);
		}
		for (i = 0; i < 45; i++)
		{
			if (_____TrigonometricTable[i][1] <= num && num <= _____TrigonometricTable[i+1][1])
			{
				if ((-dx) > (-dy))
				{
					return 180 + (i + 1);
				}
				else
				{
					return 270 - (i + 1);
				}
			}
		}
	}
	else if (dx > 0 && dy < 0)
	{
		if (dx > (-dy))
		{
			num = (-dy) * _____TrigonometricBase / dx;
		}
		else
		{
			num = dx * _____TrigonometricBase / (-dy);
		}
		for (i = 0; i < 45; i++)
		{
			if (_____TrigonometricTable[i][1] <= num && num <= _____TrigonometricTable[i+1][1])
			{
				if (dx > (-dy))
				{
					return 360 - (i + 1);
				}
				else
				{
					return 270 + (i + 1);
				}
			}
		}
	}

	return 0;
}

nuLONG   core_lsin_impl    (nuLONG nDegree)
{
	for (;;)
	{
		if (nDegree < 0)
		{		
			nDegree += 360;
			continue;
		}
		if (nDegree > 360)
		{
			nDegree -= 360;
			continue;
		}
		if (nDegree > 180)
		{
			if (nDegree > 270)	
			{
				return -_____TrigonometricTable[360-nDegree][0];
			}
			else			
			{
				return -_____TrigonometricTable[nDegree-180][0];
			}
		}
		else
		{
			if (nDegree > 90)	
			{
				return _____TrigonometricTable[180-nDegree][0];
			}
			else			
			{
				return _____TrigonometricTable[nDegree][0];
			}
		}
	}
}

nuLONG   core_lcos_impl    (nuLONG nDegree)
{
	for(;;)
	{
		if (nDegree < 0)
		{
			nDegree += 360;
			continue;	
		}

		if (nDegree > 360)
		{	
			nDegree -= 360;	
			continue;	
		}

		if (nDegree > 180) 
		{
			if (nDegree > 270)
			{
				return _____TrigonometricTable[nDegree-270][0];
			}
			else
			{
				return -_____TrigonometricTable[270-nDegree][0];
			}
		}
		else
		{
			if (nDegree > 90)
			{
				return -_____TrigonometricTable[nDegree-90][0];
			}
			else
			{
				return _____TrigonometricTable[90-nDegree][0];
			}
		}
	}
}

nuLONG   core_ltan_impl    (nuLONG nDegree)
{
	for (;;)
	{
		if (nDegree < 0)
		{
			nDegree += 360;	
			continue;	
		}
		
		if (nDegree > 360)
		{
			nDegree -= 360;
			continue;
		}

		if (nDegree > 180) 
		{
			if (nDegree > 270)
			{
				return -_____TrigonometricTable[360-nDegree][1];
			}
			else
			{
				return -_____TrigonometricTable[nDegree-180][1];
			}
		}
		else
		{
			if (nDegree > 90)
			{
				return _____TrigonometricTable[180-nDegree][1];
			}
			else
			{
				return _____TrigonometricTable[nDegree][1];
			}
		}
	}
}

nuLONG core_lsqrt_impl(nuLONG dx, nuLONG dy)
{
	___nuro_long_t L1 = 0,L2 = 0;
	
	L1 = _MIN(_ABS(dx),_ABS(dy));
	L2 = _MAX(_ABS(dx),_ABS(dy));
	
	if (L1 == 0) 
	{
		return L2;
	}

	if (L1 > 60000)
	{
		return L2 + ((L1 / 100) * L1) / (L2 / 50);
	}
	else
	{
		return L2 + (L1 * L1) / (2 * L2);
	}
}

nuUSHORT core_lQSqrt_impl   (nuULONG a)
{
	___nuro_ulong_t rem = 0;
	___nuro_ulong_t root = 0;
	___nuro_ulong_t divisor = 0;
	___nuro_int32_t i;

	for(i = 0; i < 16; ++i)
	{
		root <<= 1;
		rem = ((rem << 2) + (a >> 30));
		a <<= 2;
		divisor = (root << 1) + 1;

		if(divisor <= rem)
		{
			rem -= divisor;
			++root;
		}
	}
	return (___nuro_uint16_t)(root);
}

nuDOUBLE core_CosJ_impl    (nuDOUBLE nAngle)
{
	___nuro_int32_t i = 0;
	___nuro_double_t dcos = 0, epsi = 1;
	___nuro_double_t x = nAngle;

	do
	{
		dcos = dcos + epsi;
		i++;
		epsi = x * x * epsi / ((2 * i - 1) * (2 * i));
		epsi = -epsi;
	}
	while(_ABS(epsi)>=0.0000002);

	return dcos;
}

nuDOUBLE core_SinJ_impl    (nuDOUBLE nAngle)
{
	___nuro_int32_t i = 0;
	___nuro_double_t x = nAngle;
	___nuro_double_t dsin = 0, epsi = x;

	do
	{
		dsin = dsin + epsi;
		i++;
		epsi = epsi * x * x / ((2 * i) * (2 * i + 1));
		epsi = -epsi; 
	}
	while(_ABS(epsi)>=0.0000002);

	return dsin;
}

