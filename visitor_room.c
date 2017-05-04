//
// Created by adire on 02-May-17.
//

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>


#include "visitor_room.h"

/* IMPLEMENT HERE ALL WHAT IS NEEDED */
Result init_challenge_activity(ChallengeActivity *activity, Challenge *challenge) {
    if (activity == NULL || challenge == NULL) {
        return NULL_PARAMETER;
    }
    activity->challenge = challenge;
    activity->visitor = NULL;
    activity->start_time = 0;
    return OK;
}

Result reset_challenge_activity(ChallengeActivity *activity) {
    if (activity == NULL) {
        return NULL_PARAMETER;
    }
    activity->challenge = NULL;
    activity->visitor = NULL;
    activity->start_time = 0;
    return OK;
}

Result init_visitor(Visitor *visitor, char *name, int id) {
    if (visitor == NULL || name == NULL) {
        return NULL_PARAMETER;
    }
    visitor->visitor_name = malloc(strlen(name) * 1 + 1);
    if (visitor->visitor_name == NULL) {
        return MEMORY_PROBLEM;
    }
    strcpy(visitor->visitor_name, name);
    visitor->room_name = NULL;
    visitor->current_challenge = NULL;
    visitor->visitor_id = id;
    return OK;
}

Result reset_visitor(Visitor *visitor) {
    if (visitor == NULL) {
        return NULL_PARAMETER;
    }
    free(visitor->visitor_name);
    visitor->visitor_name = NULL;
    visitor->room_name = NULL;
    visitor->current_challenge = NULL;
    visitor->visitor_id = 0;
    return OK;
}

Result init_room(ChallengeRoom *room, char *name, int num_challenges) {
    if (room == NULL || name == NULL) {
        return NULL_PARAMETER;
    }
    if (num_challenges < 1) {
        return ILLEGAL_PARAMETER;
    }
    room->name = malloc(strlen(name) * 1 + 1);
    if (room == NULL) {
        return MEMORY_PROBLEM;
    }
    strcpy(room->name, name);
    room->num_of_challenges = num_challenges;
}
Result reset_room(ChallengeRoom *room);

Result num_of_free_places_for_level(ChallengeRoom *room, Level level, int *places);

Result change_room_name(ChallengeRoom *room, char *new_name);

Result room_of_visitor(Visitor *visitor, char **room_name);

Result visitor_enter_room(ChallengeRoom *room, Visitor *visitor, Level level, int start_time);
/* the challenge to be chosen is the lexicographically named smaller one that has
   the required level. assume all names are different. */

Result visitor_quit_room(Visitor *visitor, int quit_time);
