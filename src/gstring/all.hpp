// ================================================================================================
//  GString  ·  all.hpp
//  Include this one.
//
//  A speculative reconstruction. See src/gstring/README.md.
// ================================================================================================

#ifndef GSTRING_ALL_HPP
#define GSTRING_ALL_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "gconfig.hpp"              // the platform, the basic types, the foreign classes
#include "gencoding.hpp"            // what the bytes mean
#include "gfwd.hpp"                 // forward declarations
#include "gpayload.hpp"             // the payload, the body, and who wins
#include "gformat.hpp"              // arguments to Format
#include "gstring.hpp"              // GString
#include "gspot.hpp"                // GSpot
#include "gsubstring.hpp"           // GSubString
#include "gstringlist.hpp"          // GStringList, which lives inside a GString
#include "gtokeniser.hpp"           // GTokeniser
#include "gregularexpression.hpp"   // GRegularExpression and GMatch

#endif // GSTRING_ALL_HPP
