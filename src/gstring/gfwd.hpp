// ================================================================================================
//  GString  ·  gfwd.hpp
//  Forward declarations, for headers that only need to name the classes.
//
//  A speculative reconstruction. See src/gstring/README.md.
// ================================================================================================

#ifndef GSTRING_GFWD_HPP
#define GSTRING_GFWD_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <vector>

class GString;            // the string                               gstring.hpp
class GSubString;         // a piece that borrows its storage         gsubstring.hpp
class GSpot;              // a position, by code point                gspot.hpp
class GStringList;        // the pieces of a + b, not yet joined      gstringlist.hpp
class GTokeniser;         // lazy splitting, without strtok's state   gtokeniser.hpp
class GRegularExpression; // every dialect in the building            gregularexpression.hpp
class GMatch;             // one match of a GRegularExpression        gregularexpression.hpp
class GFormatArg;         // one argument to GString::Format          gformat.hpp

struct GPayload;          // a native string, and who frees it        gpayload.hpp
struct GBody;             // what GStrings share                      gpayload.hpp

typedef std::vector<GString>    GStrings;
typedef std::vector<GSubString> GSubStrings;

#endif // GSTRING_GFWD_HPP
