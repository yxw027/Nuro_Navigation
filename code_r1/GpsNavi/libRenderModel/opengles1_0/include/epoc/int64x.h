//==============================================================================
//
// Symbian TInt64 extension
//
//------------------------------------------------------------------------------
//
// 11-05-2004   Iwan Junianto       initial version
//
//==============================================================================

#ifndef __INT64X_H__
#define __INT64X_H__

#include <e32std.h>

class TInt64X: public TInt64 {
public:
    IMPORT_C TInt64X();
    IMPORT_C TInt64X(const TInt64& aVal);
    IMPORT_C TInt64X(TInt aVal);
    IMPORT_C operator TInt() const;
    IMPORT_C TInt64X operator*(const TInt64X& aVal) const;
    IMPORT_C TInt64X operator*(TInt aVal) const;
    IMPORT_C TInt64X operator/(const TInt64X& aVal) const;
    IMPORT_C TInt64X operator/(TInt aVal) const;
    IMPORT_C TInt64X operator+(const TInt64X& aVal) const;
    IMPORT_C TInt64X operator-(const TInt64X& aVal) const;
    IMPORT_C TInt64X operator>>(TInt aShift) const;
    IMPORT_C TInt64X operator<<(TInt aShift) const;
    IMPORT_C TInt operator<(TInt aVal) const;
    IMPORT_C TInt operator>(TInt aVal) const;
};
#endif
