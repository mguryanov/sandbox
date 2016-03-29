/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010-2015 Facebook, Inc. (http://www.facebook.com)     |
   | Copyright (c) 1998-2010 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
*/
//#include "hphp/zend/zend-html.h"
#include "zend-html.h"
#include "html-table-php.h"

#include <unicode/uchar.h>
#include <unicode/utf8.h>

#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <climits>
#include <ctype.h>

#if defined(__GNUC__) && __GNUC__ >= 4
#define LIKELY(x)   (__builtin_expect((x), 1))
#define UNLIKELY(x) (__builtin_expect((x), 0))
#else
#define LIKELY(x)   (x)
#define UNLIKELY(x) (x)
#endif


namespace HPHP {

///////////////////////////////////////////////////////////////////////////////
// UTF-8 entity tables

//using namespace entity_charset_enum;

/*
 * for mapping purposes been used
 *      https://www.w3.org/TR/html5/entities.json
 *      http://unicode.org/Public/MAPPINGS/ISO8859/8859-1.TXT
 *      http://unicode.org/Public/MAPPINGS/VENDORS/MISC/KOI8-R.TXT
 */

static const struct {
  const char *codeset;
  entity_charset charset;
} charset_map[] = {
  { "ISO-8859-1",     cs_8859_1 },
  { "ISO8859-1",      cs_8859_1 },
  { "ISO-8859-5",     cs_8859_5 },
  { "ISO8859-5",      cs_8859_5 },
  { "ISO-8859-15",    cs_8859_15 },
  { "ISO8859-15",     cs_8859_15 },
  { "utf-8",          cs_utf_8 },
  { "cp866",          cs_cp866 },
  { "866",            cs_cp866 },
  { "ibm866",         cs_cp866 },
  { "cp1251",         cs_cp1251 },
  { "Windows-1251",   cs_cp1251 },
  { "win-1251",       cs_cp1251 },
  { "cp1252",         cs_cp1252 },
  { "Windows-1252",   cs_cp1252 },
  { "1252",           cs_cp1252 },
  { "BIG5",           cs_big5 },
  { "950",            cs_big5 },
  { "GB2312",         cs_gb2312 },
  { "936",            cs_gb2312 },
  { "BIG5-HKSCS",     cs_big5hkscs },
  { "Shift_JIS",      cs_sjis },
  { "SJIS",           cs_sjis },
  { "932",            cs_sjis },
  { "EUCJP",          cs_eucjp },
  { "KOI8-R",         cs_koi8r },
  { "koi8-ru",        cs_koi8r },
  { "koi8r",          cs_koi8r },
  /* Missing support for these at the moment
  { "EUC-JP",         cs_eucjp },
  { "MacRoman",       cs_macroman },
  */
  { nullptr,          cs_numelems }
};

///////////////////////////////////////////////////////////////////////////////

entity_charset determine_charset(const char *charset_hint) {
  entity_charset charset = cs_numelems;

  if (charset_hint == nullptr) {
    // default to utf-8
    return cs_utf_8;
  }

  size_t len = strlen(charset_hint);

  /* now walk the charset map and look for the codeset */
  for (int i = 0; charset_map[i].codeset; i++) {
    if (len == strlen(charset_map[i].codeset) &&
      strncasecmp(charset_hint, charset_map[i].codeset, len) == 0) {
      charset = charset_map[i].charset;
      break;
    }
  }

  return charset;
}

///////////////////////////////////////////////////////////////////////////////

/*
 *  refactoring : begin
 */


#define EMPTY_SWITCH_DEFAULT_CASE()


static inline ulong zend_inline_hash_func(
                                const char *arKey,
                                uint nKeyLength)
{
  register ulong hash = 5381;

  /* variant with the hash unrolled eight times */
  for (; nKeyLength >= 8; nKeyLength -= 8) {
    hash = ((hash << 5) + hash) + *arKey++;
    hash = ((hash << 5) + hash) + *arKey++;
    hash = ((hash << 5) + hash) + *arKey++;
    hash = ((hash << 5) + hash) + *arKey++;
    hash = ((hash << 5) + hash) + *arKey++;
    hash = ((hash << 5) + hash) + *arKey++;
    hash = ((hash << 5) + hash) + *arKey++;
    hash = ((hash << 5) + hash) + *arKey++;
  }
  switch (nKeyLength) {
    case 7: hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
    case 6: hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
    case 5: hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
    case 4: hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
    case 3: hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
    case 2: hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
    case 1: hash = ((hash << 5) + hash) + *arKey++; break;
    case 0: break;
EMPTY_SWITCH_DEFAULT_CASE()
  }
  return hash;
}


/* Macro for disabling flag of translation of non-basic entities where this isn't supported.
 * Not appropriate for html_entity_decode/htmlspecialchars_decode */
#define LIMIT_ALL(all, doctype, charset) do { \
        (all) = (all) && \
                !CHARSET_PARTIAL_SUPPORT((charset)) && \
                ((doctype) != static_cast<int64_t>( EntBitmask::ENT_BM_XML1 )); \
} while (0)


/* {{{ */
static inline void map_to_unicode(unsigned code, const enc_to_uni *table, unsigned *res)
{
    /* only single byte encodings are currently supported; assumed code <= 0xFF */
    *res = table->inner[ENT_ENC_TO_UNI_STAGE1(code)]->uni_cp[ENT_ENC_TO_UNI_STAGE2(code)];
}
/* }}} */


/* {{{ unescape_inverse_map */
static const entity_ht *unescape_inverse_map(int all, int flags)
{
    int document_type =
            flags & static_cast<int64_t>( EntBitmask::ENT_BM_DOC_TYPE );

    if (all) {
        switch (document_type) {
        case static_cast<int64_t>( EntBitmask::ENT_BM_HTML401 ):
        case static_cast<int64_t>( EntBitmask::ENT_BM_XHTML ): /* but watch out for &apos;...*/
            return &ent_ht_html4;
        case static_cast<int64_t>( EntBitmask::ENT_BM_HTML5 ):
            return &ent_ht_html5;
        default:
            return &ent_ht_be_apos;
        }
    } else {
        switch (document_type) {
        case static_cast<int64_t>( EntBitmask::ENT_BM_HTML401 ):
            return &ent_ht_be_noapos;
        default:
            return &ent_ht_be_apos;
        }
    }
}
/* }}} */


/* {{{ determine_entity_table
 * Entity table to use. Note that entity tables are defined in terms of
 * unicode code points */
static entity_table_opt determine_entity_table(int all, int doctype)
{
    entity_table_opt retval = {nullptr,nullptr};

    assert(!(doctype == static_cast<int64_t>( EntBitmask::ENT_BM_XML1 ) && all ));

    if (all) {
            retval.ms_table =
                (doctype == static_cast<int64_t>( EntBitmask::ENT_BM_HTML5 )) ?
                    entity_ms_table_html5 : entity_ms_table_html4;
    } else {
            retval.table =
                (doctype == static_cast<int64_t>( EntBitmask::ENT_BM_HTML401 )) ?
                    stage3_table_be_noapos_00000 : stage3_table_be_apos_00000;
    }
    return retval;
}
/* }}} */


#define MB_FAILURE(pos, advance) do { \
        *cursor = pos + (advance); \
        *status = false; \
        return 0; \
} while (0)

#define CHECK_LEN(pos, chars_need) ((str_len - (pos)) >= (chars_need))

/* valid as single byte character or leading byte */
#define utf8_lead(c)  ((c) < 0x80 || ((c) >= 0xC2 && (c) <= 0xF4))
/* whether it's actually valid depends on other stuff;
 * this macro cannot check for non-shortest forms, surrogates or
 * code points above 0x10FFFF */
#define utf8_trail(c) ((c) >= 0x80 && (c) <= 0xBF)

#define gb2312_lead(c) ((c) != 0x8E && (c) != 0x8F && (c) != 0xA0 && (c) != 0xFF)
#define gb2312_trail(c) ((c) >= 0xA1 && (c) <= 0xFE)

#define sjis_lead(c) ((c) != 0x80 && (c) != 0xA0 && (c) < 0xFD)
#define sjis_trail(c) ((c) >= 0x40  && (c) != 0x7F && (c) < 0xFD)

/* {{{ get_next_char */
static inline unsigned int get_next_char(entity_charset charset,
                                         const u_char *str,
                                         size_t str_len,
                                         size_t *cursor,
                                         bool *status)
{
    size_t pos = *cursor;
    unsigned int this_char = 0;

    *status = true;
    assert(pos <= str_len);

    if (!CHECK_LEN(pos, 1))
            MB_FAILURE(pos, 1);

    switch (charset) {
    case cs_utf_8:
            {
                    /* We'll follow strategy 2. from section 3.6.1 of UTR #36:
                     * "In a reported illegal byte sequence, do not include any
                     *  non-initial byte that encodes a valid character or is a leading
                     *  byte for a valid sequence." */
                    u_char c;
                    c = str[pos];
                    if (c < 0x80) {
                            this_char = c;
                            pos++;
                    } else if (c < 0xc2) {
                            MB_FAILURE(pos, 1);
                    } else if (c < 0xe0) {
                            if (!CHECK_LEN(pos, 2))
                                    MB_FAILURE(pos, 1);

                            if (!utf8_trail(str[pos + 1])) {
                                    MB_FAILURE(pos, utf8_lead(str[pos + 1]) ? 1 : 2);
                            }
                            this_char = ((c & 0x1f) << 6) |
                                        (str[pos + 1] & 0x3f);

                            if (this_char < 0x80) { /* non-shortest form */
                                    MB_FAILURE(pos, 2);
                            }
                            pos += 2;
                    } else if (c < 0xf0) {
                            size_t avail = str_len - pos;

                            if (avail < 3 ||
                                            !utf8_trail(str[pos + 1]) || !utf8_trail(str[pos + 2])) {
                                    if (avail < 2 || utf8_lead(str[pos + 1]))
                                            MB_FAILURE(pos, 1);
                                    else if (avail < 3 || utf8_lead(str[pos + 2]))
                                            MB_FAILURE(pos, 2);
                                    else
                                            MB_FAILURE(pos, 3);
                            }

                            this_char = ((c & 0x0f) << 12) |
                                        ((str[pos + 1] & 0x3f) << 6) |
                                        (str[pos + 2] & 0x3f);

                            if (this_char < 0x800) { /* non-shortest form */
                                    MB_FAILURE(pos, 3);
                            } else if (this_char >= 0xd800 && this_char <= 0xdfff) { /* surrogate */
                                    MB_FAILURE(pos, 3);
                            }
                            pos += 3;
                    } else if (c < 0xf5) {
                            size_t avail = str_len - pos;

                            if (avail < 4 ||
                                            !utf8_trail(str[pos + 1]) || !utf8_trail(str[pos + 2]) ||
                                            !utf8_trail(str[pos + 3])) {
                                    if (avail < 2 || utf8_lead(str[pos + 1]))
                                            MB_FAILURE(pos, 1);
                                    else if (avail < 3 || utf8_lead(str[pos + 2]))
                                            MB_FAILURE(pos, 2);
                                    else if (avail < 4 || utf8_lead(str[pos + 3]))
                                            MB_FAILURE(pos, 3);
                                    else
                                            MB_FAILURE(pos, 4);
                            }

                            this_char = ((c & 0x07) << 18) |
                                        ((str[pos + 1] & 0x3f) << 12) |
                                        ((str[pos + 2] & 0x3f) << 6) |
                                        (str[pos + 3] & 0x3f);

                            if (this_char < 0x10000 || this_char > 0x10FFFF) { /* non-shortest form or outside range */
                                    MB_FAILURE(pos, 4);
                            }
                            pos += 4;
                    } else {
                            MB_FAILURE(pos, 1);
                    }
            }
            break;

    case cs_big5:
            /* reference http://demo.icu-project.org/icu-bin/convexp?conv=big5 */
            {
                    u_char c = str[pos];
                    if (c >= 0x81 && c <= 0xFE) {
                            u_char next;
                            if (!CHECK_LEN(pos, 2))
                                    MB_FAILURE(pos, 1);

                            next = str[pos + 1];

                            if ((next >= 0x40 && next <= 0x7E) ||
                                            (next >= 0xA1 && next <= 0xFE)) {
                                    this_char = (c << 8) | next;
                            } else {
                                    MB_FAILURE(pos, 1);
                            }
                            pos += 2;
                    } else {
                            this_char = c;
                            pos += 1;
                    }
            }
            break;

    case cs_big5hkscs:
            {
                    u_char c = str[pos];
                    if (c >= 0x81 && c <= 0xFE) {
                            u_char next;
                            if (!CHECK_LEN(pos, 2))
                                    MB_FAILURE(pos, 1);

                            next = str[pos + 1];

                            if ((next >= 0x40 && next <= 0x7E) ||
                                            (next >= 0xA1 && next <= 0xFE)) {
                                    this_char = (c << 8) | next;
                            } else if (next != 0x80 && next != 0xFF) {
                                    MB_FAILURE(pos, 1);
                            } else {
                                    MB_FAILURE(pos, 2);
                            }
                            pos += 2;
                    } else {
                            this_char = c;
                            pos += 1;
                    }
            }
            break;

    case cs_gb2312: /* EUC-CN */
            {
                    u_char c = str[pos];
                    if (c >= 0xA1 && c <= 0xFE) {
                            u_char next;
                            if (!CHECK_LEN(pos, 2))
                                    MB_FAILURE(pos, 1);

                            next = str[pos + 1];

                            if (gb2312_trail(next)) {
                                    this_char = (c << 8) | next;
                            } else if (gb2312_lead(next)) {
                                    MB_FAILURE(pos, 1);
                            } else {
                                    MB_FAILURE(pos, 2);
                            }
                            pos += 2;
                    } else if (gb2312_lead(c)) {
                            this_char = c;
                            pos += 1;
                    } else {
                            MB_FAILURE(pos, 1);
                    }
            }
            break;

    case cs_sjis:
            {
                    u_char c = str[pos];
                    if ((c >= 0x81 && c <= 0x9F) || (c >= 0xE0 && c <= 0xFC)) {
                            u_char next;
                            if (!CHECK_LEN(pos, 2))
                                    MB_FAILURE(pos, 1);

                            next = str[pos + 1];

                            if (sjis_trail(next)) {
                                    this_char = (c << 8) | next;
                            } else if (sjis_lead(next)) {
                                    MB_FAILURE(pos, 1);
                            } else {
                                    MB_FAILURE(pos, 2);
                            }
                            pos += 2;
                    } else if (c < 0x80 || (c >= 0xA1 && c <= 0xDF)) {
                            this_char = c;
                            pos += 1;
                    } else {
                            MB_FAILURE(pos, 1);
                    }
            }
            break;

    case cs_eucjp:
            {
                    u_char c = str[pos];

                    if (c >= 0xA1 && c <= 0xFE) {
                            unsigned next;
                            if (!CHECK_LEN(pos, 2))
                                    MB_FAILURE(pos, 1);
                            next = str[pos + 1];

                            if (next >= 0xA1 && next <= 0xFE) {
                                    /* this a jis kanji char */
                                    this_char = (c << 8) | next;
                            } else {
                                    MB_FAILURE(pos, (next != 0xA0 && next != 0xFF) ? 1 : 2);
                            }
                            pos += 2;
                    } else if (c == 0x8E) {
                            unsigned next;
                            if (!CHECK_LEN(pos, 2))
                                    MB_FAILURE(pos, 1);

                            next = str[pos + 1];
                            if (next >= 0xA1 && next <= 0xDF) {
                                    /* JIS X 0201 kana */
                                    this_char = (c << 8) | next;
                            } else {
                                    MB_FAILURE(pos, (next != 0xA0 && next != 0xFF) ? 1 : 2);
                            }
                            pos += 2;
                    } else if (c == 0x8F) {
                            size_t avail = str_len - pos;

                            if (avail < 3 || !(str[pos + 1] >= 0xA1 && str[pos + 1] <= 0xFE) ||
                                            !(str[pos + 2] >= 0xA1 && str[pos + 2] <= 0xFE)) {
                                    if (avail < 2 || (str[pos + 1] != 0xA0 && str[pos + 1] != 0xFF))
                                            MB_FAILURE(pos, 1);
                                    else if (avail < 3 || (str[pos + 2] != 0xA0 && str[pos + 2] != 0xFF))
                                            MB_FAILURE(pos, 2);
                                    else
                                            MB_FAILURE(pos, 3);
                            } else {
                                    /* JIS X 0212 hojo-kanji */
                                    this_char = (c << 16) | (str[pos + 1] << 8) | str[pos + 2];
                            }
                            pos += 3;
                    } else if (c != 0xA0 && c != 0xFF) {
                            /* character encoded in 1 code unit */
                            this_char = c;
                            pos += 1;
                    } else {
                            MB_FAILURE(pos, 1);
                    }
            }
            break;
    default:
            /* single-byte charsets */
            this_char = str[pos++];
            break;
    }

    *cursor = pos;
    return this_char;
}
/* }}} */


/* {{{ find_entity_for_char */
static inline void find_entity_for_char(unsigned int k,
                                        entity_charset charset,
                                        const entity_stage1_row *table,
                                        const u_char **entity,
                                        size_t *entity_len,
                                        u_char *old,
                                        size_t oldlen,
                                        size_t *cursor)
{
    unsigned stage1_idx = ENT_STAGE1_INDEX(k);
    const entity_stage3_row *c;

    if (stage1_idx > 0x1D) {
        *entity     = nullptr;
        *entity_len = 0;
        return;
    }

    c = &table[stage1_idx][ENT_STAGE2_INDEX(k)][ENT_STAGE3_INDEX(k)];

    if (!c->ambiguous) {
        *entity     = (const u_char *)c->data.ent.entity;
        *entity_len = c->data.ent.entity_len;
    } else {
        /* peek at next char */
        size_t  cursor_before = *cursor;
        bool status = true;
        unsigned next_char;

        if (!(*cursor < oldlen))
                goto no_suitable_2nd;

        next_char = get_next_char(charset, old, oldlen, cursor, &status);

        if (status == false)
                goto no_suitable_2nd;

        {
            const entity_multicodepoint_row *s, *e;

            s = &c->data.multicodepoint_table[1];
            e = s - 1 + c->data.multicodepoint_table[0].leading_entry.size;
            /* we could do a binary search but it's not worth it since we have
             * at most two entries... */
            for ( ; s <= e; s++) {
                if (s->normal_entry.second_cp == next_char) {
                    *entity     = (const u_char *)s->normal_entry.entity;
                    *entity_len = s->normal_entry.entity_len;
                    return;
                }
            }
        }
no_suitable_2nd:
        *cursor = cursor_before;
        *entity = (const unsigned char *)
                c->data.multicodepoint_table[0].leading_entry.default_entity;
        *entity_len = c->data.multicodepoint_table[0].leading_entry.default_entity_len;
    }
}
/* }}} */

/* {{{ find_entity_for_char_basic */
static inline void find_entity_for_char_basic(unsigned int k,
                                              const entity_stage3_row *table,
                                              const u_char **entity,
                                              size_t *entity_len)
{
    if (k >= 64U) {
        *entity     = NULL;
        *entity_len = 0;
        return;
    }

    *entity     = (const u_char *)table[k].data.ent.entity;
    *entity_len = table[k].data.ent.entity_len;
}
/* }}} */


/* {{{ unicode_cp_is_allowed */
static inline int unicode_cp_is_allowed(unsigned uni_cp,
                                        int document_type)
{
    /* XML 1.0              HTML 4.01           HTML 5
     * 0x09..0x0A           0x09..0x0A          0x09..0x0A
     * 0x0D                 0x0D                0x0C..0x0D
     * 0x0020..0xD7FF       0x20..0x7E          0x20..0x7E
     *                      0x00A0..0xD7FF      0x00A0..0xD7FF
     * 0xE000..0xFFFD       0xE000..0x10FFFF    0xE000..0xFDCF
     * 0x010000..0x10FFFF                       0xFDF0..0x10FFFF (*)
     *
     * (*) exclude code points where ((code & 0xFFFF) >= 0xFFFE)
     *
     * References:
     * XML 1.0:   <http://www.w3.org/TR/REC-xml/#charsets>
     * HTML 4.01: <http://www.w3.org/TR/1999/PR-html40-19990824/sgml/sgmldecl.html>
     * HTML 5:    <http://dev.w3.org/html5/spec/Overview.html#preprocessing-the-input-stream>
     *
     * Not sure this is the relevant part for HTML 5, though. I opted to
     * disallow the characters that would result in a parse error when
     * preprocessing of the input stream. See also section 8.1.3.
     *
     * It's unclear if XHTML 1.0 allows C1 characters. I'll opt to apply to
     * XHTML 1.0 the same rules as for XML 1.0.
     * See <http://cmsmcq.com/2007/C1.xml>.
     */

    switch (document_type) {
    case static_cast<int64_t>( EntBitmask::ENT_BM_HTML401 ):
        return (uni_cp >= 0x20 && uni_cp <= 0x7E) ||
                (uni_cp == 0x0A || uni_cp == 0x09 || uni_cp == 0x0D) ||
                (uni_cp >= 0xA0 && uni_cp <= 0xD7FF) ||
                (uni_cp >= 0xE000 && uni_cp <= 0x10FFFF);
    case static_cast<int64_t>( EntBitmask::ENT_BM_HTML5 ):
        return (uni_cp >= 0x20 && uni_cp <= 0x7E) ||
                /* form feed U+0C allowed */
                (uni_cp >= 0x09 && uni_cp <= 0x0D && uni_cp != 0x0B) ||
                (uni_cp >= 0xA0 && uni_cp <= 0xD7FF) ||
                (uni_cp >= 0xE000 && uni_cp <= 0x10FFFF &&
                 /* last two of each plane (nonchars) disallowed */
                 ((uni_cp & 0xFFFF) < 0xFFFE) &&
                 /* U+FDD0-U+FDEF (nonchars) disallowed */
                 (uni_cp < 0xFDD0 || uni_cp > 0xFDEF));
    case static_cast<int64_t>( EntBitmask::ENT_BM_XHTML ):
    case static_cast<int64_t>( EntBitmask::ENT_BM_XML1 ):
        return (uni_cp >= 0x20 && uni_cp <= 0xD7FF) ||
                (uni_cp == 0x0A || uni_cp == 0x09 || uni_cp == 0x0D) ||
                (uni_cp >= 0xE000 && uni_cp <= 0x10FFFF && uni_cp != 0xFFFE && uni_cp != 0xFFFF);
    default:
            return 1;
    }
}
/* }}} */

/* {{{ unicode_cp_is_allowed */
static inline int numeric_entity_is_allowed(unsigned uni_cp,
                                            int document_type)
{
    /* less restrictive than unicode_cp_is_allowed */
    switch (document_type) {
    case static_cast<int64_t>( EntBitmask::ENT_BM_HTML401 ):
        /* all non-SGML characters (those marked with UNUSED in DESCSET) should be
         * representable with numeric entities */
        return uni_cp <= 0x10FFFF;
    case static_cast<int64_t>( EntBitmask::ENT_BM_HTML5 ):
        /* 8.1.4. The numeric character reference forms described above are allowed to
         * reference any Unicode code point other than U+0000, U+000D, permanently
         * undefined Unicode characters (noncharacters), and control characters other
         * than space characters (U+0009, U+000A, U+000C and U+000D) */
        /* seems to allow surrogate characters, then */
        return (uni_cp >= 0x20 && uni_cp <= 0x7E) ||
                (uni_cp >= 0x09 && uni_cp <= 0x0C && uni_cp != 0x0B) || /* form feed U+0C allowed, but not U+0D */
                (uni_cp >= 0xA0 && uni_cp <= 0x10FFFF &&
                        ((uni_cp & 0xFFFF) < 0xFFFE) && /* last two of each plane (nonchars) disallowed */
                        (uni_cp < 0xFDD0 || uni_cp > 0xFDEF)); /* U+FDD0-U+FDEF (nonchars) disallowed */
    case static_cast<int64_t>( EntBitmask::ENT_BM_XHTML ):
    case static_cast<int64_t>( EntBitmask::ENT_BM_XML1 ):
            /* OTOH, XML 1.0 requires "character references to match the production for Char
             * See <http://www.w3.org/TR/REC-xml/#NT-CharRef> */
            return unicode_cp_is_allowed(uni_cp, document_type);
    default:
            return 1;
    }
}
/* }}} */


/* {{{ process_numeric_entity
 * Auxiliary function to traverse_for_entities.
 * On input, *buf should point to the first character after # and on output, it's the last
 * byte read, no matter if there was success or insuccess.
 */
static inline bool process_numeric_entity(const char **buf,
                                          unsigned *code_point)
{
    long code_l;
    int hexadecimal = (**buf == 'x' || **buf == 'X'); /* TODO: XML apparently disallows "X" */
    char *endptr;

    if (hexadecimal && (**buf != '\0'))
        (*buf)++;

    /* strtol allows whitespace and other stuff
     * in the beginning we're not interested
     */
    if ((hexadecimal && !isxdigit(**buf)) ||
        (!hexadecimal && !isdigit(**buf))) {
        return false;
    }

    code_l = strtol(*buf, &endptr, hexadecimal ? 16 : 10);
    /* we're guaranteed there were valid digits, so *endptr > buf */
    *buf = endptr;

    if (**buf != ';')
        return false;

    /* many more are invalid, but that depends on whether it's HTML
     * (and which version) or XML. */
    if (code_l > 0x10FFFFL)
        return false;

    if (code_point != nullptr)
        *code_point = (unsigned)code_l;

    return true;
}
/* }}} */



/* {{{ process_named_entity */
static inline bool process_named_entity_html(const char **buf,
                                             const char **start,
                                             size_t *length)
{
    *start = *buf;

    /* "&" is represented by a 0x26 in all supported encodings. That means
     * the byte after represents a character or is the leading byte of an
     * sequence of 8-bit code units. If in the ranges below, it represents
     * necessarily a alpha character because none of the supported encodings
     * has an overlap with ASCII in the leading byte (only on the second one) */
    while ((**buf >= 'a' && **buf <= 'z') ||
           (**buf >= 'A' && **buf <= 'Z') ||
           (**buf >= '0' && **buf <= '9')) {
        (*buf)++;
    }

    if (**buf != ';')
        return false;

    /* cast to size_t OK as the quantity is always non-negative */
    *length = *buf - *start;

    if (*length == 0)
        return false;

    return true;
}
/* }}} */

/* {{{ resolve_named_entity_html */
static inline bool resolve_named_entity_html(const char *start,
                                             size_t length,
                                             const entity_ht *ht,
                                             unsigned *uni_cp1,
                                             unsigned *uni_cp2)
{
    const entity_cp_map *s;
    ulong hash = zend_inline_hash_func(start, length);

    s = ht->buckets[hash % ht->num_elems];
    while (s->entity) {
        if (s->entity_len == length) {
            if (memcmp(start, s->entity, length) == 0) {
                *uni_cp1 = s->codepoint1;
                *uni_cp2 = s->codepoint2;
                return true;
            }
        }
        s++;
    }
    return false;
}
/* }}} */


char *string_html_encode(const char *old, size_t &oldlen,
                         const int64_t flags, entity_charset charset,
                         bool double_encode, bool all)
{
    size_t cursor, maxlen, len;
    char *replaced;
    int doctype = flags & static_cast<int64_t>( EntBitmask::ENT_BM_DOC_TYPE );
    entity_table_opt entity_table;
    const enc_to_uni *to_uni_table = nullptr;
    const entity_ht *inv_map = nullptr; /* used for !double_encode */
    /* only used if flags includes
     * ENT_HTML_IGNORE_ERRORS or
     * ENT_HTML_SUBSTITUTE_DISALLOWED_CHARS */
    const u_char *replacement = nullptr;
    size_t replacement_len = 0;

    if (all) { /* replace with all named entities */
        if (CHARSET_PARTIAL_SUPPORT(charset)) {
            /* Non basic entities substitution for
             * multi-byte encodings other than UTF-8 */
            return nullptr;
        }
        LIMIT_ALL(all, doctype, charset);
    }

    entity_table = determine_entity_table(all, doctype);

    if (all && !CHARSET_UNICODE_COMPAT(charset)) {
        to_uni_table = enc_to_uni_index[charset];
    }

    if (!double_encode) {
        /* first arg is 1 because we want to identify valid named entities
         * even if we are only encoding the basic ones */
        inv_map = unescape_inverse_map(1, flags);
    }

    if (flags & (static_cast<int64_t>( EntBitmask::ENT_BM_SUBSTITUTE ) |
                 static_cast<int64_t>( EntBitmask::ENT_BM_DISALLOWED )))
    {
        if (charset == cs_utf_8) {
            replacement = (const u_char*)"\xEF\xBF\xBD";
            replacement_len = sizeof("\xEF\xBF\xBD") - 1;
        } else {
            replacement = (const u_char*)"&#xFFFD;";
            replacement_len = sizeof("&#xFFFD;") - 1;
        }
    }

    /* initial estimate */
    if (oldlen < 64) {
        maxlen = 128;
    } else {
        maxlen = 2 * oldlen;
        if (maxlen < oldlen) {
            // zend_error_noreturn(E_ERROR, "Input string is too long");
            return nullptr;
        }
    }

    replaced = (char *)malloc(maxlen * 8uL + 1); /* adding 1 is safe: maxlen is even */
    if ( !replaced ) {
      return nullptr;
    }

    len = 0;
    cursor = 0;
    while (cursor < oldlen) {
        const u_char *mbsequence = nullptr;
        size_t mbseqlen = 0,
               cursor_before = cursor;
        bool status = true;
        unsigned int this_char = get_next_char(charset, (const u_char *)old, oldlen, &cursor, &status);

        /* guarantee we have at least 40 bytes to write.
         * In HTML5, entities may take up to 33 bytes */
        if (len > maxlen - 40) { /* maxlen can never be smaller than 128 */
            char* oldptr=replaced;
            maxlen += 128;
            replaced = (char*)realloc(replaced, maxlen);
            if( !replaced ) {
                free( oldptr );
                return nullptr;
            }
        }

        if (status == false) {
            /* invalid MB sequence */
            if (flags & static_cast<int64_t>( EntBitmask::ENT_BM_IGNORE )) {
                continue;
            } else if (flags & static_cast<int64_t>( EntBitmask::ENT_BM_SUBSTITUTE )) {
                memcpy(&replaced[len], replacement, replacement_len);
                len += replacement_len;
                continue;
            } else {
                free(replaced);
                oldlen = 0;
                return nullptr;
            }
        } else { /* SUCCESS */
            mbsequence = (const u_char *)&old[cursor_before];
            mbseqlen = cursor - cursor_before;
        }

        if (this_char != '&') { /* no entity on this position */
            const u_char *rep = nullptr;
            size_t rep_len = 0;

            if ((this_char == '\'' && !(flags & static_cast<int64_t>( EntBitmask::ENT_BM_SINGLE ))) ||
                (this_char == '"' && !(flags & static_cast<int64_t>( EntBitmask::ENT_BM_DOUBLE ))))
                goto pass_char_through;

            if (all) { /* false that CHARSET_PARTIAL_SUPPORT(charset) */
                if (to_uni_table != nullptr) {
                    /* !CHARSET_UNICODE_COMPAT therefore not UTF-8; since UTF-8
                     * is the only multibyte encoding with !CHARSET_PARTIAL_SUPPORT,
                     * we're using a single byte encoding */
                    map_to_unicode(this_char, to_uni_table, &this_char);
                    if (this_char == 0xFFFF) /* no mapping; pass through */
                        goto pass_char_through;
                }
                /* the cursor may advance */
                find_entity_for_char(
                            this_char,
                            charset,
                            entity_table.ms_table,
                            &rep,
                            &rep_len,
                            (u_char *)old,
                            oldlen,
                            &cursor
                        );
            } else {
                find_entity_for_char_basic(
                            this_char,
                            entity_table.table,
                            &rep,
                            &rep_len
                        );
            }

            if (rep != nullptr) {
                replaced[len++] = '&';
                memcpy(&replaced[len], rep, rep_len);
                len += rep_len;
                replaced[len++] = ';';
            } else {
                /* we did not find an entity for this char.
                 * check for its validity, if its valid pass it unchanged */
                if (flags & static_cast<int64_t>( EntBitmask::ENT_BM_DISALLOWED )) {
                    if (CHARSET_UNICODE_COMPAT(charset)) {
                        if (!unicode_cp_is_allowed(this_char, doctype)) {
                            mbsequence = replacement;
                            mbseqlen = replacement_len;
                        }
                    } else if (to_uni_table) {
                        if (!all) /* otherwise we already did this */
                            map_to_unicode(this_char, to_uni_table, &this_char);
                        if (!unicode_cp_is_allowed(this_char, doctype)) {
                            mbsequence = replacement;
                            mbseqlen = replacement_len;
                        }
                    } else {
                        /* not a unicode code point, unless, coincidentally, it's in
                         * the 0x20..0x7D range (except 0x5C in sjis). We know nothing
                         * about other code points, because we have no tables. Since
                         * Unicode code points in that range are not disallowed in any
                         * document type, we could do nothing. However, conversion
                         * tables frequently map 0x00-0x1F to the respective C0 code
                         * points. Let's play it safe and admit that's the case */
                        if (this_char <= 0x7D &&
                            !unicode_cp_is_allowed(this_char, doctype)) {
                            mbsequence = replacement;
                            mbseqlen = replacement_len;
                        }
                    }
                }
            pass_char_through:
                if (mbseqlen > 1) {
                    memcpy(replaced + len, mbsequence, mbseqlen);
                    len += mbseqlen;
                } else {
                    replaced[len++] = mbsequence[0];
                }
            }
        } else { /* this_char == '&' */
            if (double_encode) {
        encode_amp:
                memcpy(&replaced[len], "&amp;", sizeof("&amp;") - 1);
                len += sizeof("&amp;") - 1;
            } else { /* no double encode */
                     /* check if entity is valid */
                size_t ent_len; /* not counting & or ; */
                /* peek at next char */
                if (old[cursor] == '#') { /* numeric entity */
                    unsigned code_point;
                    int valid;
                    char *pos = (char*)&old[cursor+1];
                    valid = process_numeric_entity((const char **)&pos, &code_point);
                    if (valid == false)
                        goto encode_amp;
                    if (flags & static_cast<int64_t>( EntBitmask::ENT_BM_DISALLOWED )) {
                        if (!numeric_entity_is_allowed(code_point, doctype))
                            goto encode_amp;
                    }
                    ent_len = pos - (char*)&old[cursor];
                } else { /* named entity */
                        /* check for vality of named entity */
                        const char *start = &old[cursor],
                                   *next = start;
                        unsigned   dummy1, dummy2;

                        if (process_named_entity_html(&next, &start, &ent_len) == false)
                            goto encode_amp;
                        if (resolve_named_entity_html(start, ent_len, inv_map, &dummy1, &dummy2) == false) {
                                if (!(doctype == static_cast<int64_t>( EntBitmask::ENT_BM_XHTML )
                                      && ent_len == 4
                                      && start[0] == 'a'
                                      && start[1] == 'p'
                                      && start[2] == 'o'
                                      && start[3] == 's'))
                                {
                                    /* uses html4 inv_map, which doesn't include apos;. This is a
                                     * hack to support it */
                                    goto encode_amp;
                                }
                        }
                }
                /* checks passed; copy entity to result */
                /* entity size is unbounded, we may need more memory */
                /* at this point maxlen - len >= 40 */
                if (maxlen - len < ent_len + 2 /* & and ; */) {
                    /* ent_len < oldlen, which is certainly <= SIZE_MAX/2 */
                    char* oldptr=replaced;
                    maxlen += ent_len + 128;
                    replaced = (char*)realloc(replaced, maxlen);
                    if( !replaced ) {
                        free( oldptr );
                        return nullptr;
                    }
                }
                replaced[len++] = '&';
                memcpy(&replaced[len], &old[cursor], ent_len);
                len += ent_len;
                replaced[len++] = ';';
                cursor += ent_len + 1;
            }
        }
    }
    replaced[len] = '\0';
    oldlen = len;

    return replaced;
}


/*
 *  refactoring : end
 */


char *string_html_encode_extra(const char *input, int &len,
                               StringHtmlEncoding flags,
                               const AsciiMap *asciiMap) {
  assert(input);
  /**
   * Though seems to be wasting memory a lot, we have to realize most of the
   * time this function is called with small strings, or fragments of HTMLs.
   * Allocating/deallocating anything less than 1K is trivial these days, and
   * we want avoid string copying as much as possible. Of course, the return
   * char * is really sent back at large, occupying unnessary space for
   * potentially longer time than we need, we have to realize the two closest
   * solutions are not that much better, either:
   *
   * 1. pre-calculate size by iterating through the string once: too time
   *    consuming;
   * 2. take a guess and double buffer size when over: still wasting, and
   *    it may not save that much.
   */
  char *ret = (char *)malloc(len * 8uL + 1);
  if (!ret) {
    return nullptr;
  }
  char *q = ret;
  const char *rep = u8"\ufffd";
  int32_t srcPosBytes;
  for (srcPosBytes = 0; srcPosBytes < len; /* incremented in-loop */) {
    unsigned char c = input[srcPosBytes];
    if (c && c < 128) {
      srcPosBytes++; // Optimize US-ASCII case
      if ((asciiMap->map[c & 64 ? 1 : 0] >> (c & 63)) & 1) {
        switch (c) {
          case '"':
            *q++ = '&'; *q++ = 'q'; *q++ = 'u';
            *q++ = 'o'; *q++ = 't'; *q++ = ';';
            break;
          case '\'':
            *q++ = '&'; *q++ = '#'; *q++ = '0';
            *q++ = '3'; *q++ = '9'; *q++ = ';';
            break;
          case '<':
            *q++ = '&'; *q++ = 'l'; *q++ = 't'; *q++ = ';';
            break;
          case '>':
            *q++ = '&'; *q++ = 'g'; *q++ = 't'; *q++ = ';';
            break;
          case '&':
            *q++ = '&'; *q++ = 'a'; *q++ = 'm'; *q++ = 'p'; *q++ = ';';
            break;
          default:
            *q++ = '&'; *q++ = '#';
            *q++ = c >= 100 ? '1' : '0';
            *q++ = ((c / 10) % 10) + '0';
            *q++ = (c % 10) + '0';
            *q++ = ';';
            break;
        }
      } else {
        *q++ = c;
      }
    } else if (flags & STRING_HTML_ENCODE_UTF8) {
      UChar32 curCodePoint;
      U8_NEXT(input, srcPosBytes, len, curCodePoint);
      if ((flags & STRING_HTML_ENCODE_NBSP) && curCodePoint == 0xC2A0) {
        *q++ = '&'; *q++ = 'n'; *q++ = 'b'; *q++ = 's'; *q++ = 'p'; *q++ = ';';
      } else if (curCodePoint <= 0) {
        if (flags & STRING_HTML_ENCODE_UTF8IZE_REPLACE) {
          if (flags & STRING_HTML_ENCODE_HIGH) {
            *q++ = '&'; *q++ = '#'; *q++ = 'x';
            *q++ = 'f'; *q++ = 'f'; *q++ = 'f'; *q++ = 'd';
            *q++ = ';';
          } else {
            const char *r = rep;
            while (*r) *q++ = *r++;
          }
        }
      } else if (flags & STRING_HTML_ENCODE_HIGH) {
        q += sprintf(q, "&#x%x;", curCodePoint);
      } else {
        int32_t pos = 0;
        U8_APPEND_UNSAFE(q, pos, curCodePoint);
        q += pos;
      }
    } else {
      srcPosBytes++; // Optimize US-ASCII case
      if (c == 0xa0) {
        *q++ = '&'; *q++ = 'n'; *q++ = 'b'; *q++ = 's'; *q++ = 'p'; *q++ = ';';
      } else if (flags & STRING_HTML_ENCODE_HIGH) {
        *q++ = '&'; *q++ = '#';
        *q++ = c >= 200 ? '2' : '1';
        *q++ = ((c / 10) % 10) + '0';
        *q++ = (c % 10) + '0';
        *q++ = ';';
      } else {
        *q++ = c;
      }
    }
  }
  if (q - ret > INT_MAX) {
    free(ret);
    return nullptr;
  }
  *q = 0;
  len = q - ret;
  return ret;
}


/*
 *  refactoring : begin
 */

/* {{{ php_utf32_utf8 */
static inline size_t php_utf32_utf8(
        unsigned char *buf,
        unsigned k)
{
    size_t retval = 0;

    /* assert(0x0 <= k <= 0x10FFFF); */

    if (k < 0x80) {
        buf[0] = k;
        retval = 1;
    } else if (k < 0x800) {
        buf[0] = 0xc0 | (k >> 6);
        buf[1] = 0x80 | (k & 0x3f);
        retval = 2;
    } else if (k < 0x10000) {
        buf[0] = 0xe0 | (k >> 12);
        buf[1] = 0x80 | ((k >> 6) & 0x3f);
        buf[2] = 0x80 | (k & 0x3f);
        retval = 3;
    } else {
        buf[0] = 0xf0 | (k >> 18);
        buf[1] = 0x80 | ((k >> 12) & 0x3f);
        buf[2] = 0x80 | ((k >> 6) & 0x3f);
        buf[3] = 0x80 | (k & 0x3f);
        retval = 4;
    }
    /* UTF-8 has been restricted to max 4 bytes since RFC 3629 */

    return retval;
}
/* }}} */

/* {{{ unimap_bsearc_cmp
 * Binary search of unicode code points in unicode <--> charset mapping.
 * Returns the code point in the target charset (whose mapping table was given) or 0 if
 * the unicode code point is not in the table.
 */
static inline unsigned char unimap_bsearch(
        const uni_to_enc *table,
        unsigned code_key_a,
        size_t num)
{
    const uni_to_enc *l = table,
                     *h = &table[num-1],
                     *m;
    unsigned short code_key;

    /* we have no mappings outside the BMP */
    if (code_key_a > 0xFFFFU)
        return 0;

    code_key = (unsigned short) code_key_a;

    while (l <= h) {
        m = l + (h - l) / 2;
        if (code_key < m->un_code_point)
            h = m - 1;
        else if (code_key > m->un_code_point)
            l = m + 1;
        else
            return m->cs_code;
    }
    return 0;
}
/* }}} */

/* {{{ map_from_unicode */
static inline int map_from_unicode(
        unsigned code,
        entity_charset charset,
        unsigned *res)
{
    unsigned char found;
    const uni_to_enc *table;
    size_t table_size;

    switch (charset) {
    case cs_8859_1:
        /* identity mapping of code points to unicode */
        if (code > 0xFF) {
            return false;
        }
        *res = code;
        break;

    case cs_8859_5:
        if (code <= 0xA0 || code == 0xAD /* soft hyphen */) {
            *res = code;
        } else if (code == 0x2116) {
            *res = 0xF0; /* numero sign */
        } else if (code == 0xA7) {
            *res = 0xFD; /* section sign */
        } else if (code >= 0x0401 && code <= 0x044F) {
            if (code == 0x040D || code == 0x0450 || code == 0x045D)
                return false;
            *res = code - 0x360;
        } else {
            return false;
        }
        break;

    case cs_8859_15:
        if (code < 0xA4 || (code > 0xBE && code <= 0xFF)) {
            *res = code;
        } else { /* between A4 and 0xBE */
            found = unimap_bsearch(unimap_iso885915,
                    code, sizeof(unimap_iso885915) / sizeof(*unimap_iso885915));
            if (found)
                *res = found;
            else
                return false;
        }
        break;

    case cs_cp1252:
        if (code <= 0x7F || (code >= 0xA0 && code <= 0xFF)) {
            *res = code;
        } else {
            found = unimap_bsearch(unimap_win1252,
                    code, sizeof(unimap_win1252) / sizeof(*unimap_win1252));
            if (found)
                *res = found;
            else
                return false;
        }
        break;

    case cs_macroman:
        if (code == 0x7F)
            return false;
        table = unimap_macroman;
        table_size = sizeof(unimap_macroman) / sizeof(*unimap_macroman);
        goto table_over_7F;
    case cs_cp1251:
        table = unimap_win1251;
        table_size = sizeof(unimap_win1251) / sizeof(*unimap_win1251);
        goto table_over_7F;
    case cs_koi8r:
        table = unimap_koi8r;
        table_size = sizeof(unimap_koi8r) / sizeof(*unimap_koi8r);
        goto table_over_7F;
    case cs_cp866:
        table = unimap_cp866;
        table_size = sizeof(unimap_cp866) / sizeof(*unimap_cp866);

table_over_7F:
        if (code <= 0x7F) {
            *res = code;
        } else {
            found = unimap_bsearch(table, code, table_size);
            if (found)
                    *res = found;
            else
                    return false;
        }
        break;

    /* from here on, only map the possible characters in the ASCII range.
     * to improve support here, it's a matter of building the unicode mappings.
     * See <http://www.unicode.org/Public/6.0.0/ucd/Unihan.zip> */
    case cs_sjis:
    case cs_eucjp:
        /* we interpret 0x5C as the Yen symbol. This is not universal.
         * See <http://www.w3.org/Submission/japanese-xml/#ambiguity_of_yen> */
        if (code >= 0x20 && code <= 0x7D) {
            if (code == 0x5C)
                return false;
            *res = code;
        } else {
            return false;
        }
        break;

    case cs_big5:
    case cs_big5hkscs:
    case cs_gb2312:
        if (code >= 0x20 && code <= 0x7D) {
            *res = code;
        } else {
            return false;
        }
        break;

    default:
        return false;
    }

    return true;
}
/* }}} */


static inline size_t write_octet_sequence(
        char *buf,
        entity_charset charset,
        unsigned code)
{
    /* code is not necessarily a unicode code point */
    switch (charset) {
    case cs_utf_8:
        return php_utf32_utf8(reinterpret_cast<u_char*>( buf ), code);

    case cs_8859_1:
    case cs_cp1252:
    case cs_8859_15:
    case cs_koi8r:
    case cs_cp1251:
    case cs_8859_5:
    case cs_cp866:
    case cs_macroman:
        /* single byte stuff */
        *buf = code;
        return 1;

    case cs_big5:
    case cs_big5hkscs:
    case cs_sjis:
    case cs_gb2312:
        /* we don't have complete unicode mappings for these yet in entity_decode,
         * and we opt to pass through the octet sequences for these in htmlentities
         * instead of converting to an int and then converting back. */
#if 0
        return php_mb2_int_to_char(buf, code);
#else
#ifdef ZEND_DEBUG
        assert(code <= 0xFFU);
#endif
        *buf = code;
        return 1;
#endif

    case cs_eucjp:
#if 0 /* idem */
        return php_mb2_int_to_char(buf, code);
#else
#ifdef ZEND_DEBUG
        assert(code <= 0xFFU);
#endif
        *buf = code;
        return 1;
#endif

    default:
        assert(0);
        return 0;
    }
}


/* {{{ traverse_for_entities
 * Auxiliary function to php_unescape_html_entities().
 * - The argument "all" determines if all numeric entities are decode or only those
 *   that correspond to quotes (depending on quote_style).
 */
/* maximum expansion (factor 1.2) for HTML 5 with &nGt; and &nLt; */
/* +2 is 1 because of rest (probably unnecessary), 1 because of terminating 0 */
#define TRAVERSE_FOR_ENTITIES_EXPAND_SIZE(oldlen) ((oldlen) + (oldlen) / 5 + 2)
static void traverse_for_entities(
        const char *old,
        size_t oldlen,
        char *ret, /* should have allocated TRAVERSE_FOR_ENTITIES_EXPAND_SIZE(olden) */
        size_t *retlen,
        int all,
        int flags,
        const entity_ht *inv_map,
        enum entity_charset charset)
{
    const char *p,*lim;
    char *q;
    int doctype = flags & static_cast<int64_t>( EntBitmask::ENT_BM_DOC_TYPE );

    lim = old + oldlen; /* terminator address */
    assert(*lim == '\0');

    for (p = old, q = ret; p < lim;) {
        unsigned code, code2 = 0;
        const char *next = nullptr; /* when set, next > p, otherwise possible inf loop */

        /* Shift JIS, Big5 and HKSCS use multi-byte encodings where an
         * ASCII range byte can be part of a multi-byte sequence.
         * However, they start at 0x40, therefore if we find a 0x26 byte,
         * we're sure it represents the '&' character. */

        /* assumes there are no single-char entities */
        if (p[0] != '&' || (p + 3 >= lim)) {
            *(q++) = *(p++);
            continue;
        }

        /* now p[3] is surely valid and is no terminator */

        /* numerical entity */
        if (p[1] == '#') {
            next = &p[2];
            if (process_numeric_entity(&next, &code) == false)
                goto invalid_code;

            /* If we're in htmlspecialchars_decode, we're only decoding entities
             * that represent &, <, >, " and '. Is this one of them? */
            if (!all && (code > 63U ||
                stage3_table_be_apos_00000[code].data.ent.entity == nullptr))
                    goto invalid_code;

            /* are we allowed to decode this entity in this document type?
             * HTML 5 is the only that has a character that cannot be used in
             * a numeric entity but is allowed literally (U+000D). The
             * unoptimized version would be ... || !numeric_entity_is_allowed(code) */
            if (!unicode_cp_is_allowed(code, doctype) ||
                (doctype == static_cast<int64_t>( EntBitmask::ENT_BM_HTML5 ) &&
                 code == 0x0D))
                goto invalid_code;

        } else {
            const char *start;
            size_t ent_len;

            next = &p[1];
            start = next;

            if (process_named_entity_html(&next, &start, &ent_len) == false)
                goto invalid_code;

            if (resolve_named_entity_html(start, ent_len, inv_map, &code, &code2) == false) {
                if (doctype == static_cast<int64_t>( EntBitmask::ENT_BM_XHTML ) &&
                    ent_len == 4    &&
                    start[0] == 'a' &&
                    start[1] == 'p' &&
                    start[2] == 'o' &&
                    start[3] == 's')
                {
                    /* uses html4 inv_map, which doesn't include apos;.
                     * This is a hack to support it */
                    code = (unsigned) '\'';
                } else {
                    goto invalid_code;
                }
            }
        }

        assert(*next == ';');

        if (((code == '\'' &&
                !(flags & static_cast<int64_t>( EntBitmask::ENT_BM_SINGLE))) ||
             (code == '"' &&
                !(flags & static_cast<int64_t>( EntBitmask::ENT_BM_DOUBLE))))
            /* && code2 == '\0' always true for current maps */)
            goto invalid_code;

        /* UTF-8 doesn't need mapping (ISO-8859-1 doesn't either, but
         * the call is needed to ensure the codepoint <= U+00FF)  */
        if (charset != cs_utf_8) {
            /* replace unicode code point */
            if (map_from_unicode(code, charset, &code) == false || code2 != 0)
                goto invalid_code; /* not representable in target charset */
        }

        q += write_octet_sequence(q,charset,code);

        if (code2) {
            q += write_octet_sequence(q,charset,code2);
        }

        /* jump over the valid entity; may go beyond size of buffer; np */
        p = next + 1;
        continue;

    invalid_code:
        for (; p < next; p++) {
            *(q++) = *p;
        }
    }

    *q = '\0';
    *retlen = (size_t)(q - ret);
}
/* }}} */


char *string_html_decode(
                const char *old,
                int &oldlen,
                int64_t flags,
                const char *hint_charset,
                bool all)
{
    size_t retlen;
    char *ret = nullptr;
    enum entity_charset charset;
    const entity_ht *inverse_map = nullptr;
    int new_size = TRAVERSE_FOR_ENTITIES_EXPAND_SIZE(oldlen);

    if (all) {
        charset = determine_charset(hint_charset);
    } else {
        charset = cs_8859_1; /* charset shouldn't matter, use ISO-8859-1 for performance */
    }

    /* don't use LIMIT_ALL! */

    if (oldlen > new_size) {
        /* overflow, refuse to do anything */
        ret = strndup((char*)old, oldlen);
        retlen = oldlen;
        goto empty_source;
    }
    ret = static_cast<char*>( malloc(new_size) );
    *ret = '\0';
    retlen = oldlen;
    if (retlen == 0) {
        goto empty_source;
    }

    inverse_map = unescape_inverse_map(all, flags);

    /* replace numeric entities */
    traverse_for_entities(old, oldlen, ret, &retlen, all, flags, inverse_map, charset);

empty_source:
    oldlen = retlen;
    return ret;
}



/* {{{ write_s3row_data */
static inline void write_s3row_data(
        const entity_stage3_row *r,
        unsigned orig_cp,
        enum entity_charset charset,
        vector<pair<string,string>>& arr)
{
    char key[9] = ""; /* two unicode code points in UTF-8 */
    char entity[LONGEST_ENTITY_LENGTH + 2] = {'&'};
    size_t written_k1;

    written_k1 = write_octet_sequence(key, charset, orig_cp);

    if (!r->ambiguous) {
        size_t l = r->data.ent.entity_len;
        memcpy(&entity[1], r->data.ent.entity, l);
        entity[l + 1] = ';';
        arr.push_back( pair<string,string>(
                                string( key, written_k1 + 1 ),
                                string( entity, l + 2 )
                            )
                        );
        //add_assoc_stringl_ex(arr, key, written_k1 + 1, entity, l + 2, 1);
    } else {
        unsigned i, num_entries;
        const entity_multicodepoint_row *mcpr = r->data.multicodepoint_table;

        if (mcpr[0].leading_entry.default_entity != nullptr) {
            size_t l = mcpr[0].leading_entry.default_entity_len;
            memcpy(&entity[1], mcpr[0].leading_entry.default_entity, l);
            entity[l + 1] = ';';
            arr.push_back( pair<string,string>(
                                    string( key, written_k1 + 1 ),
                                    string( entity, l + 2 )
                                )
                            );
            //add_assoc_stringl_ex(arr, key, written_k1 + 1, entity, l + 2, 1);
        }
        num_entries = mcpr[0].leading_entry.size;
        for (i = 1; i <= num_entries; i++) {
            size_t   l, written_k2;
            unsigned uni_cp, spe_cp;

            uni_cp = mcpr[i].normal_entry.second_cp;
            l = mcpr[i].normal_entry.entity_len;

            if (!CHARSET_UNICODE_COMPAT(charset)) {
                if (map_from_unicode(uni_cp, charset, &spe_cp) == false)
                    continue; /* non representable in this charset */
            } else {
                spe_cp = uni_cp;
            }

            written_k2 = write_octet_sequence(&key[written_k1], charset, spe_cp);
            memcpy(&entity[1], mcpr[i].normal_entry.entity, l);
            entity[l + 1] = ';';
            entity[l + 1] = '\0';
            arr.push_back( pair<string,string>(
                                   string( key, written_k1 + written_k2 + 1 ),
                                   string( entity, l + 1 )
                               )
                           );
            //add_assoc_stringl_ex(arr, key, written_k1 + written_k2 + 1, entity, l + 1, 1);
        }
    }
}
/* }}} */


/* {{{ proto array get_html_translation_table([int table [, int flags [, string charset_hint]]])
   Returns the internal translation table used by htmlspecialchars and htmlentities */
vector<pair<string,string>>
get_html_translation_table(int64_t table /* = 0 */,
                           int64_t flags /* = k_ENT_HTML_QUOTE_DOUBLE */,
                           const char* charset_hint /* = "UTF-8" */)
{
    int64_t doctype;
    entity_table_opt entity_table;
    const enc_to_uni *to_uni_table = nullptr;
    entity_charset charset;

    /* in this function we have to jump through some loops because we're
     * getting the translated table from data structures that are optimized for
     * random access, not traversal */

    bool all = (table == k_HTML_ENTITIES);
    flags |= k_ENT_HTML_QUOTE_DOUBLE;

    charset = determine_charset(charset_hint);
    doctype = flags & k_ENT_HTML_DOC_TYPE_MASK;
    LIMIT_ALL(all, doctype, charset);

    vector<pair<string,string>> ret;

    entity_table = determine_entity_table(all, doctype);
    if (all && !CHARSET_UNICODE_COMPAT(charset)) {
        to_uni_table = enc_to_uni_index[charset];
    }

    if (all) { /* HTML_ENTITIES (actually, any non-zero value for 1st param) */
        const entity_stage1_row *ms_table = entity_table.ms_table;

        if (CHARSET_UNICODE_COMPAT(charset)) {
            unsigned i, j, k, max_i, max_j, max_k;
            /* no mapping to unicode required */
            if (CHARSET_SINGLE_BYTE(charset)) { /* ISO-8859-1 */
                max_i = 1; max_j = 4; max_k = 64;
            } else {
                max_i = 0x1E; max_j = 64; max_k = 64;
            }

            for (i = 0; i < max_i; i++) {
                if (ms_table[i] == empty_stage2_table)
                    continue;
                for (j = 0; j < max_j; j++) {
                    if (ms_table[i][j] == empty_stage3_table)
                        continue;
                    for (k = 0; k < max_k; k++) {
                        const entity_stage3_row *r = &ms_table[i][j][k];
                        unsigned code;

                        if (r->data.ent.entity == nullptr)
                            continue;

                        code = ENT_CODE_POINT_FROM_STAGES(i, j, k);
                        if (((code == '\'' && !(flags & k_ENT_HTML_QUOTE_SINGLE)) ||
                             (code == '"' && !(flags & k_ENT_HTML_QUOTE_DOUBLE))))
                            continue;

                        write_s3row_data(r, code, charset, ret);
                    }
                }
            }
        } else {
            /* we have to iterate through the set of code points for this
             * encoding and map them to unicode code points */
            unsigned i;
            for (i = 0; i <= 0xFF; i++) {
                const entity_stage3_row *r;
                unsigned uni_cp;

                /* can be done before mapping, they're invariant */
                if (((i == '\'' && !(flags & k_ENT_HTML_QUOTE_SINGLE)) ||
                     (i == '"' && !(flags & k_ENT_HTML_QUOTE_DOUBLE))))
                    continue;

                map_to_unicode(i, to_uni_table, &uni_cp);
                r = &ms_table[ENT_STAGE1_INDEX(uni_cp)][ENT_STAGE2_INDEX(uni_cp)][ENT_STAGE3_INDEX(uni_cp)];
                if (r->data.ent.entity == nullptr)
                    continue;

                write_s3row_data(r, i, charset, ret);
            }
        }
    } else {
        /* we could use sizeof(stage3_table_be_apos_00000) as well */
        unsigned    j, numelems = sizeof(stage3_table_be_noapos_00000) /
                                  sizeof(*stage3_table_be_noapos_00000);

        for (j = 0; j < numelems; j++) {
            const entity_stage3_row *r = &entity_table.table[j];
            if (r->data.ent.entity == nullptr)
                continue;

            if (((j == '\'' && !(flags & k_ENT_HTML_QUOTE_SINGLE)) ||
                 (j == '"' && !(flags & k_ENT_HTML_QUOTE_DOUBLE))))
                continue;

            /* charset is indifferent, used cs_8859_1 for efficiency */
            write_s3row_data(r, j, cs_8859_1, ret);
        }
    }

    return ret;
}
/* }}} */

/*
 *  refactoring : end
 */

///////////////////////////////////////////////////////////////////////////////
}
