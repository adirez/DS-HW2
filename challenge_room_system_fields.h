//
// Created by adire on 06-May-17.
//

#include "visitor_room.h"

char *system_name;
Challenge **system_challenges;
ChallengeRoom **system_rooms;

/*
 * a linked list of visitors
 */

struct SVisitorsList
{
    Visitor *visitor;
    struct SVisitorsList *next;
} *VisitorsList;