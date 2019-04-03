/***************************************************************************/
/*                                                                         */
/*  ftheader.h                                                             */
/*                                                                         */
/*    Build macros of the FreeType 2 library.                              */
/*                                                                         */
/*  Copyright 1996-2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008 by       */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

#ifndef __FT_HEADER_H__
#define __FT_HEADER_H__


  /*@***********************************************************************/
  /*                                                                       */
  /* <Macro>                                                               */
  /*    FT_BEGIN_HEADER                                                    */
  /*                                                                       */
  /* <Description>                                                         */
  /*    This macro is used in association with @FT_END_HEADER in header    */
  /*    files to ensure that the declarations within are properly          */
  /*    encapsulated in an `extern "C" { .. }' block when included from a  */
  /*    C++ compiler.                                                      */
  /*                                                                       */
#ifdef __cplusplus
#define FT_BEGIN_HEADER  extern "C" {
#else
#define FT_BEGIN_HEADER  /* nothing */
#endif


  /*@***********************************************************************/
  /*                                                                       */
  /* <Macro>                                                               */
  /*    FT_END_HEADER                                                      */
  /*                                                                       */
  /* <Description>                                                         */
  /*    This macro is used in association with @FT_BEGIN_HEADER in header  */
  /*    files to ensure that the declarations within are properly          */
  /*    encapsulated in an `extern "C" { .. }' block when included from a  */
  /*    C++ compiler.                                                      */
  /*                                                                       */
#ifdef __cplusplus
#define FT_END_HEADER  }
#else
#define FT_END_HEADER  /* nothing */
#endif


  /*************************************************************************/
  /*                                                                       */
  /* Aliases for the FreeType 2 public and configuration files.            */
  /*                                                                       */
  /*************************************************************************/

  /*************************************************************************/
  /*                                                                       */
  /* <Section>                                                             */
  /*    header_file_macros                                                 */
  /*                                                                       */
  /* <Title>                                                               */
  /*    Header File Macros                                                 */
  /*                                                                       */
  /* <Abstract>                                                            */
  /*    Macro definitions used to #include specific header files.          */
  /*                                                                       */
  /* <Description>                                                         */
  /*    The following macros are defined to the name of specific           */
  /*    FreeType~2 header files.  They can be used directly in #include    */
  /*    statements as in:                                                  */
  /*                                                                       */
  /*    {                                                                  */
  /*      #include <freetype/freetype.h>                                           */
  /*      #include <freetype/ftmm.h>                                   */
  /*      #include <freetype/ftglyph.h>                                              */
  /*    }                                                                  */
  /*                                                                       */
  /*    There are several reasons why we are now using macros to name      */
  /*    public header files.  The first one is that such macros are not    */
  /*    limited to the infamous 8.3~naming rule required by DOS (and       */
  /*    `<freetype/ftmm.h>' is a lot more meaningful than `ftmm.h').   */
  /*                                                                       */
  /*    The second reason is that it allows for more flexibility in the    */
  /*    way FreeType~2 is installed on a given system.                     */
  /*                                                                       */
  /*************************************************************************/


  /* configuration files */

  /*************************************************************************
   *
   * @macro:
   *   <freetype/config/ftconfig.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing
   *   FreeType~2 configuration data.
   *
   */
// #ifndef <freetype/config/ftconfig.h>
// #define <freetype/config/ftconfig.h>  <freetype/config/ftconfig.h>
// #endif


  /*************************************************************************
   *
   * @macro:
   *   <freetype/config/ftstdlib.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing
   *   FreeType~2 interface to the standard C library functions.
   *
   */
// #ifndef <freetype/config/ftstdlib.h>
// #define <freetype/config/ftstdlib.h>  <freetype/config/ftstdlib.h>
// #endif


  /*************************************************************************
   *
   * @macro:
   *   <freetype/config/ftoption.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing
   *   FreeType~2 project-specific configuration options.
   *
   */
// #ifndef <freetype/config/ftoption.h>
// #define <freetype/config/ftoption.h>  <freetype/config/ftoption.h>
// #endif


  /*************************************************************************
   *
   * @macro:
   *   <freetype/config/ftmodule.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   list of FreeType~2 modules that are statically linked to new library
   *   instances in @FT_Init_FreeType.
   *
   */
// #ifndef <freetype/config/ftmodule.h>
// #define <freetype/config/ftmodule.h>  <freetype/config/ftmodule.h>
// #endif

  /* */

  /* public headers */

  /*************************************************************************
   *
   * @macro:
   *   <freetype/freetype.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   base FreeType~2 API.
   *
   */
/*#define <freetype/freetype.h>  <freetype/freetype.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/fterrors.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   list of FreeType~2 error codes (and messages).
   *
   *   It is included by @<freetype/freetype.h>.
   *
   */
/*#define <freetype/fterrors.h>  <freetype/fterrors.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftmoderr.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   list of FreeType~2 module error offsets (and messages).
   *
   */
/*#define <freetype/ftmoderr.h>  <freetype/ftmoderr.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftsystem.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   FreeType~2 interface to low-level operations (i.e., memory management
   *   and stream i/o).
   *
   *   It is included by @<freetype/freetype.h>.
   *
   */
/*#define <freetype/ftsystem.h>  <freetype/ftsystem.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftimage.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing type
   *   definitions related to glyph images (i.e., bitmaps, outlines,
   *   scan-converter parameters).
   *
   *   It is included by @<freetype/freetype.h>.
   *
   */
/*#define <freetype/ftimage.h>  <freetype/ftimage.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/fttypes.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   basic data types defined by FreeType~2.
   *
   *   It is included by @<freetype/freetype.h>.
   *
   */
/*#define <freetype/fttypes.h>  <freetype/fttypes.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftlist.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   list management API of FreeType~2.
   *
   *   (Most applications will never need to include this file.)
   *
   */
/*#define <freetype/ftlist.h>  <freetype/ftlist.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftoutln.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   scalable outline management API of FreeType~2.
   *
   */
/*#define <freetype/ftoutln.h>  <freetype/ftoutln.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftsizes.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   API which manages multiple @FT_Size objects per face.
   *
   */
/*#define <freetype/ftsizes.h>  <freetype/ftsizes.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftmodapi.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   module management API of FreeType~2.
   *
   */
/*#define <freetype/ftmodapi.h>  <freetype/ftmodapi.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftrender.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   renderer module management API of FreeType~2.
   *
   */
/*#define <freetype/ftrender.h>  <freetype/ftrender.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/t1tables.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   types and API specific to the Type~1 format.
   *
   */
/*#define <freetype/t1tables.h>  <freetype/t1tables.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ttnameid.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   enumeration values which identify name strings, languages, encodings,
   *   etc.  This file really contains a _large_ set of constant macro
   *   definitions, taken from the TrueType and OpenType specifications.
   *
   */
/*#define <freetype/ttnameid.h>  <freetype/ttnameid.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/tttables.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   types and API specific to the TrueType (as well as OpenType) format.
   *
   */
/*#define <freetype/tttables.h>  <freetype/tttables.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/tttags.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   definitions of TrueType four-byte `tags' which identify blocks in
   *   SFNT-based font formats (i.e., TrueType and OpenType).
   *
   */
/*#define <freetype/tttags.h>  <freetype/tttags.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftbdf.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   definitions of an API which accesses BDF-specific strings from a
   *   face.
   *
   */
/*#define <freetype/ftbdf.h>  <freetype/ftbdf.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftcid.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   definitions of an API which access CID font information from a
   *   face.
   *
   */
/*#define <freetype/ftcid.h>  <freetype/ftcid.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftgzip.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   definitions of an API which supports gzip-compressed files.
   *
   */
/*#define <freetype/ftgzip.h>  <freetype/ftgzip.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftlzw.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   definitions of an API which supports LZW-compressed files.
   *
   */
/*#define <freetype/ftlzw.h>  <freetype/ftlzw.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftwinfnt.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   definitions of an API which supports Windows FNT files.
   *
   */
/*#define <freetype/ftwinfnt.h>   <freetype/ftwinfnt.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftglyph.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   API of the optional glyph management component.
   *
   */
/*#define <freetype/ftglyph.h>  <freetype/ftglyph.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftbitmap.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   API of the optional bitmap conversion component.
   *
   */
/*#define <freetype/ftbitmap.h>  <freetype/ftbitmap.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftbbox.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   API of the optional exact bounding box computation routines.
   *
   */
/*#define <freetype/ftbbox.h>  <freetype/ftbbox.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftcache.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   API of the optional FreeType~2 cache sub-system.
   *
   */
/*#define <freetype/ftcache.h>  <freetype/ftcache.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftcache.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   `glyph image' API of the FreeType~2 cache sub-system.
   *
   *   It is used to define a cache for @FT_Glyph elements.  You can also
   *   use the API defined in @<freetype/ftcache.h> if you only need to
   *   store small glyph bitmaps, as it will use less memory.
   *
   *   This macro is deprecated.  Simply include @<freetype/ftcache.h> to have all
   *   glyph image-related cache declarations.
   *
   */
/*#define <freetype/ftcache.h>  <freetype/ftcache.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftcache.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   `small bitmaps' API of the FreeType~2 cache sub-system.
   *
   *   It is used to define a cache for small glyph bitmaps in a relatively
   *   memory-efficient way.  You can also use the API defined in
   *   @<freetype/ftcache.h> if you want to cache arbitrary glyph images,
   *   including scalable outlines.
   *
   *   This macro is deprecated.  Simply include @<freetype/ftcache.h> to have all
   *   small bitmaps-related cache declarations.
   *
   */
/*#define <freetype/ftcache.h>  <freetype/ftcache.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftcache.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   `charmap' API of the FreeType~2 cache sub-system.
   *
   *   This macro is deprecated.  Simply include @<freetype/ftcache.h> to have all
   *   charmap-based cache declarations.
   *
   */
/*#define <freetype/ftcache.h>  <freetype/ftcache.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftmac.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   Macintosh-specific FreeType~2 API.  The latter is used to access
   *   fonts embedded in resource forks.
   *
   *   This header file must be explicitly included by client applications
   *   compiled on the Mac (note that the base API still works though).
   *
   */
/*#define <freetype/ftmac.h>  <freetype/ftmac.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftmm.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   optional multiple-masters management API of FreeType~2.
   *
   */
/*#define <freetype/ftmm.h>  <freetype/ftmm.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftsnames.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   optional FreeType~2 API which accesses embedded `name' strings in
   *   SFNT-based font formats (i.e., TrueType and OpenType).
   *
   */
/*#define <freetype/ftsnames.h>  <freetype/ftsnames.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftotval.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   optional FreeType~2 API which validates OpenType tables (BASE, GDEF,
   *   GPOS, GSUB, JSTF).
   *
   */
/*#define <freetype/ftotval.h>  <freetype/ftotval.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftgxval.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   optional FreeType~2 API which validates TrueTypeGX/AAT tables (feat,
   *   mort, morx, bsln, just, kern, opbd, trak, prop).
   *
   */
/*#define <freetype/ftgxval.h>  <freetype/ftgxval.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftpfr.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   FreeType~2 API which accesses PFR-specific data.
   *
   */
/*#define <freetype/ftpfr.h>  <freetype/ftpfr.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftstroke.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   FreeType~2 API which provides functions to stroke outline paths.
   */
/*#define <freetype/ftstroke.h>  <freetype/ftstroke.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftsynth.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   FreeType~2 API which performs artificial obliquing and emboldening.
   */
/*#define <freetype/ftsynth.h>  <freetype/ftsynth.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftxf86.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   FreeType~2 API which provides functions specific to the XFree86 and
   *   X.Org X11 servers.
   */
/*#define <freetype/ftxf86.h>  <freetype/ftxf86.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/fttrigon.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   FreeType~2 API which performs trigonometric computations (e.g.,
   *   cosines and arc tangents).
   */
/*#define <freetype/fttrigon.h>  <freetype/fttrigon.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftlcdfil.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   FreeType~2 API which performs color filtering for subpixel rendering.
   */
/*#define <freetype/ftlcdfil.h>  <freetype/ftlcdfil.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ttunpat.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   FreeType~2 API which performs color filtering for subpixel rendering.
   */
/*#define <freetype/ttunpat.h>  <freetype/ttunpat.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftincrem.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   FreeType~2 API which performs color filtering for subpixel rendering.
   */
/*#define <freetype/ftincrem.h>  <freetype/ftincrem.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftgasp.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   FreeType~2 API which returns entries from the TrueType GASP table.
   */
/*#define <freetype/ftgasp.h>  <freetype/ftgasp.h>*/


  /*************************************************************************
   *
   * @macro:
   *   <freetype/ftadvanc.h>
   *
   * @description:
   *   A macro used in #include statements to name the file containing the
   *   FreeType~2 API which returns individual and ranged glyph advances.
   */
/*#define <freetype/ftadvanc.h>  <freetype/ftadvanc.h>*/


  /* */

/*#define <freetype/fterrdef.h>  <freetype/fterrdef.h>*/


  /* The internals of the cache sub-system are no longer exposed.  We */
  /* default to <freetype/ftcache.h> at the moment just in case, but we know of */
  /* no rogue client that uses them.                                  */
  /*                                                                  */
// #define <freetype/ftcache.h>           <freetype/ftcache.h>
// #define <freetype/ftcache.h>      <freetype/ftcache.h>
// #define <freetype/ftcache.h>  <freetype/ftcache.h>
// #define FT_CACHE_INTERNAL_CACHE_H    <freetype/ftcache.h>
// #define FT_CACHE_INTERNAL_GLYPH_H    <freetype/ftcache.h>
// #define FT_CACHE_INTERNAL_IMAGE_H    <freetype/ftcache.h>
// #define FT_CACHE_INTERNAL_SBITS_H    <freetype/ftcache.h>


/*#define <freetype/ftincrem.h>          <freetype/ftincrem.h>*/

/*#define <freetype/ttunpat.h>  <freetype/ttunpat.h>*/


  /*
   * Include internal headers definitions from <freetype/internal/...>
   * only when building the library.
   */
#define FT2_BUILD_LIBRARY
#ifdef FT2_BUILD_LIBRARY
/*#define  FT_INTERNAL_INTERNAL_H  <freetype/internal/internal.h>*/
#include <freetype/internal/internal.h>
#endif /* FT2_BUILD_LIBRARY */


#endif /* __FT2_BUILD_H__ */


/* END */
