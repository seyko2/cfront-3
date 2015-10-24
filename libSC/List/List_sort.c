/*ident	"@(#)List:List_sort.c	3.1" */
/******************************************************************************
*
* C++ Standard Components, Release 3.0.
*
* Copyright (c) 1991, 1992 AT&T and Unix System Laboratories, Inc.
* Copyright (c) 1988, 1989, 1990 AT&T.  All Rights Reserved.
*
* THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T and Unix System
* Laboratories, Inc.  The copyright notice above does not evidence
* any actual or intended publication of such source code.
*
******************************************************************************/

// So ./List.c does not get included when building with 4.0.
#define _LIST_C_

#include <List.h>

// binary counter - merge sort
// thanks to Alexander Stepanov
static const int logN = 32;    // max capacity will be 2^logN

void
voidP_List_sort_internal(List<voidP>& aList, int (*lessThan)(const voidP &, const voidP &))
{
    register lnnk_ATTLC<voidP>*    temp;
    register lnnk_ATTLC<voidP>*    newCh;
    register lnnk_ATTLC<voidP>*    oldCh;
    lnnk_ATTLC<voidP>*    bitPos[logN];
    lnnk_ATTLC<voidP>**    bitPtr;
    lnnk_ATTLC<voidP>**    bitPtrMax = &bitPos[0];
    for (bitPtr = &bitPos[0]; bitPtr < &bitPos[logN]; *bitPtr++ = 0) ;
    lnnk_ATTLC<voidP>* nextPtr = aList.t ? (lnnk_ATTLC<voidP>*) aList.t->nxt: 0;
    aList.t->nxt = 0;
    lnnk_ATTLC<voidP>*    ans;
    while (newCh = nextPtr) {
        nextPtr = (lnnk_ATTLC<voidP>*)nextPtr->nxt;
        newCh->nxt = 0;
        for (bitPtr = &bitPos[0];; bitPtr++) {
            if (bitPtr > bitPtrMax) bitPtrMax = bitPtr;
            if (*bitPtr == 0) {
                *bitPtr = newCh;
                break;
            }
            oldCh = *bitPtr;
            *bitPtr = 0;
            if (!(*lessThan)(newCh->val, oldCh->val)) {
                ans = oldCh;
                for(;;) {
                    while ((temp = (lnnk_ATTLC<voidP>*)oldCh->nxt) &&
                        !(*lessThan)(newCh->val, temp->val))
                        oldCh = temp;
                    oldCh->nxt = newCh;
                    if ((oldCh = temp) == 0) {
                        newCh = ans;
                        break;
                    }
bMerge:
                    while ((temp = (lnnk_ATTLC<voidP>*)newCh->nxt) &&
                        (*lessThan)(temp->val, oldCh->val))
                        newCh = temp;
                    newCh->nxt = oldCh;
                    if ((newCh = temp) == 0) {
                        newCh = ans;
                        break;
                    }
                }
            } else {
                ans = newCh;
                goto bMerge;
            }
        }
    }
    // final merge sweep
    lnnk_ATTLC<voidP>**    bPtr2;
    for (bitPtr = &bitPos[0];; bitPtr = bPtr2) {
        while (*bitPtr == 0) bitPtr++;
        if (bitPtr == bitPtrMax) break;
        for (bPtr2 = bitPtr + 1; *bPtr2 == 0; bPtr2++) ;
        oldCh = *bPtr2;
        newCh = *bitPtr;
        if (!(*lessThan)(newCh->val, oldCh->val)) {
            ans = oldCh;
            for(;;) {
                while ((temp = (lnnk_ATTLC<voidP>*)oldCh->nxt) &&
                    !(*lessThan)(newCh->val, temp->val))
                    oldCh = temp;
                oldCh->nxt = newCh;
                if ((oldCh = temp) == 0) {
                    newCh = ans;
                    break;
                }
eMerge:
                while ((temp = (lnnk_ATTLC<voidP>*)newCh->nxt) &&
                    (*lessThan)(temp->val, oldCh->val))
                    newCh = temp;
                newCh->nxt = oldCh;
                if ((newCh = temp) == 0) {
                    newCh = ans;
                    break;
                }
            }
        } else {
            ans = newCh;
            goto eMerge;
        }
        *bPtr2 = ans;
    }
    for (newCh = *bitPtr; newCh->nxt; newCh = (lnnk_ATTLC<voidP>*)newCh->nxt)
        newCh->nxt->prv = newCh;
    newCh->nxt = *bitPtr;
    (*bitPtr)->prv = newCh;
    aList.t = newCh;
}
