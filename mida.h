#ifndef MIDA_H
#define MIDA_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef MIDA_STATIC
#define MIDA_API static
#else
#define MIDA_API extern
#endif /* MIDA_STATIC */

struct mida {
    size_t size;
    size_t length;
    unsigned char data[0];
};

MIDA_API void mida_init(struct mida *ht);

MIDA_API void mida_cleanup(struct mida *ht);

#ifndef MIDA_HEADER

#include <string.h>

MIDA_API void
mida_init(struct mida *mida)
{
    memset(mida, 0, sizeof *mida);
}

MIDA_API void
mida_cleanup(struct mida *mida)
{
    if (!mida) return;
    memset(mida->data, 0, mida->size * mida->length);
    mida->size = 0;
    mida->length = 0;
}

#endif /* MIDA_HEADER */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MIDA_H */
