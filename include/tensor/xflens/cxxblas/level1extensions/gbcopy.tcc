/*
 *   Copyright (c) 2012, Michael Lehn, Klaus Pototzky
 *
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1) Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2) Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *   3) Neither the name of the FLENS development group nor the names of
 *      its contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CXXBLAS_LEVEL1EXTENSIONS_GBCOPY_TCC
#define CXXBLAS_LEVEL1EXTENSIONS_GBCOPY_TCC 1

#include <cassert>
#include "xflens/cxxblas/cxxblas.h"

namespace cxxblas {

//
//  B = A  or B = A^T
//
template <typename IndexType, typename MA, typename MB>
void
gbcopy(StorageOrder order,
       Transpose trans, IndexType m, IndexType n,
       IndexType kl, IndexType ku,
       const MA *A, IndexType ldA,
       MB *B, IndexType ldB)
{
    CXXBLAS_DEBUG_OUT("gbcopy_generic");

    using std::max;
    using std::min;

    if (order==ColMajor) {
        gbcopy(RowMajor, trans, n, m, ku, kl, A, ldA,
               B, ldB);
        return;
    }

    if (trans == NoTrans) {
        for (IndexType j=0, i=-kl; i<=ku; ++j, ++i) {
            IndexType length = (i < 0)
                             ? min(m+i,min(m,n))
                             : min(n-i,min(m,n));
            copy(length, A+j+max(-i,0)*ldA, ldA, B+j+max(-i,0)*ldB, ldB);
        }
        return;
    }
    if (trans == Conj) {
        for (IndexType j=0, i=-kl; i<=ku; ++j, ++i) {
            IndexType length = (i < 0)
                             ? min(m+i,min(m,n))
                             : min(n-i,min(m,n));
            ccopy(length, A+j+max(-i,0)*ldA, ldA, B+j+max(-i,0)*ldB, ldB);
        }
        return;
    }
    if (trans == Trans) {
        for (IndexType j=0, J=kl+ku, i=-ku, I=ku; i<=kl; ++j, --J, ++i, --I) {
            IndexType length = (i < 0)
                             ? min(m+i,min(m,n))
                             : min(n-i,min(m,n));
            copy(length, A+j+max(-i,0)*ldA, ldA, B+J+max(-I,0)*ldB, ldB);
        }
        return;
    }

    for (IndexType j=0, J=kl+ku, i=-ku, I=ku; i<=kl; ++j, --J, ++i, --I) {
        IndexType length = (i < 0)
                         ? min(m+i,min(m,n))
                         : min(n-i,min(m,n));
        ccopy(length, A+j+max(-i,0)*ldA, ldA, B+J+max(-I,0)*ldB, ldB);
    }
    return;
}

} // namespace cxxblas

#endif // CXXBLAS_LEVEL1EXTENSIONS_GBCOPY_TCC
