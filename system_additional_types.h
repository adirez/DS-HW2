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