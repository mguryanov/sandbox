#pragma once

#include <unordered_map>
#include <array>
#include <vector>
#include <map>

namespace HPHP {

enum class entity_doctype {
  html401,
  html5,
  xhtml,
  xml1,
};

namespace entity_charset_enum {
    enum entity_charset_impl {
      cs_terminator, cs_8859_1, cs_cp1252,
      cs_8859_15, cs_utf_8, cs_big5, cs_gb2312,
      cs_big5hkscs, cs_sjis, cs_eucjp, cs_koi8r,
      cs_cp1251, cs_8859_5, cs_cp866, cs_macroman,
      cs_unknown,
      cs_end
    };

    typedef entity_charset_enum::entity_charset_impl entity_charset;

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
      /* Missing support for these at the moment
      { "EUC-JP",         cs_eucjp },
      { "KOI8-R",         cs_koi8r },
      { "koi8-ru",        cs_koi8r },
      { "koi8r",          cs_koi8r },
      { "MacRoman",       cs_macroman },
      */
      { nullptr }
    };
}

typedef struct entity_multicodepoint_row entity_multicodepoint_row_t;
struct entity_multicodepoint_row {
    const unsigned int second_codepoint; /* second code point */
    const char* entity;
};

typedef std::vector<entity_multicodepoint_row_t> entity_multicodepoint_t;
struct entity_row {
    const char* entity;
    const entity_multicodepoint_t* multicodepoint_table;
};

typedef struct entity_row entity_row_t;
typedef std::map<const unsigned int,entity_row_t> entity_table_html5_t;
const entity_table_html5_t& get_entity_table_html5();
}
