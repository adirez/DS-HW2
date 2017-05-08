//
// Created by adire on 02-May-17.
//

#ifndef ESCAPY_SYSTEM_ADDITIONAL_TYPES_H
#define ESCAPY_SYSTEM_ADDITIONAL_TYPES_H

/*
 * a linked list of visitors
 */
typedef struct SVisitorsList {
    Visitor *visitor;
    struct SVisitorsList *next;
} *VisitorsList;

#endif //ESCAPY_SYSTEM_ADDITIONAL_TYPES_H


/* additional types that may be required to support a linled list of visitors */
/* Please notice that it's preferable not to use this file. */