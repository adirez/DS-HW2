//
// Created by adire on 02-May-17.
//

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>


#include "visitor_room.h"

/*
 * initializes a challenge activity. receives the challenge as an input and
 * update it in a received activity.
 * visitor and start_time are set to 0.
 */
Result init_challenge_activity(ChallengeActivity *activity,
                               Challenge *challenge) {
    if (activity == NULL || challenge == NULL) {
        return NULL_PARAMETER;
    }
    activity->start_time = 0;
    activity->challenge = challenge;
    activity->visitor = NULL;
    return OK;
}

/*
 * resets all the fields of a received activity.
 */
Result reset_challenge_activity(ChallengeActivity *activity) {
    if (activity == NULL) {
        return NULL_PARAMETER;
    }
    activity->challenge = NULL;
    activity->visitor = NULL;
    activity->start_time = 0;
    return OK;
}

/*
 * initializes a visitor. receives a name and an id and insert it into the
 * relevant fields of a received visitor.
 * room_name and current_challenge are by default set to NULL.
 */
Result init_visitor(Visitor *visitor, char *name, int id) {
    if (visitor == NULL || name == NULL) {
        return NULL_PARAMETER;
    }
    //allocates new memory for the name and copies it
    visitor->visitor_name = malloc(strlen(name) + 1);
    if (visitor->visitor_name == NULL) {
        return MEMORY_PROBLEM;
    }
    strcpy(visitor->visitor_name, name);

    visitor->visitor_id = id;
    visitor->room_name = NULL;
    visitor->current_challenge = NULL;
    return OK;
}

/*
 * resets all the fields of a given visitor and frees the allocated memory.
 */
Result reset_visitor(Visitor *visitor) {
    if (visitor == NULL) {
        return NULL_PARAMETER;
    }
    //visitor_name is allocated in initialization and needs to be freed.
    free(visitor->visitor_name);
    visitor->visitor_name = NULL;
    visitor->room_name = NULL;
    visitor->current_challenge = NULL;
    visitor->visitor_id = 0;
    return OK;
}

/*
 * initializes a room. receives a name and the number of challenges and update
 * in the relevant fields of a given room. allocates memory for the name.
 */
Result init_room(ChallengeRoom *room, char *name, int num_challenges) {
    if (room == NULL || name == NULL) {
        return NULL_PARAMETER;
    }
    if (num_challenges < 1) {
        return ILLEGAL_PARAMETER;
    }

    room->num_of_challenges = num_challenges;

    room->name = malloc(strlen(name) + 1);
    if (room->name == NULL) {
        return MEMORY_PROBLEM;
    }
    strcpy(room->name, name);

    //allocates memory for an array of the type ChallengeActivity
    room->challenges = malloc(num_challenges * sizeof(ChallengeActivity));
    if (room->challenges == NULL) {
        return NULL_PARAMETER;
    }
    return OK;
}

/*
 * resets all the fields of a given room and frees the allocated memory.
 */
Result reset_room(ChallengeRoom *room) {
    if (room == NULL) {
        return NULL_PARAMETER;
    }
    //room->name is allocated in initialization and needs to be freed.
    free (room->name);
    room->name = NULL;
    room->num_of_challenges = 0;
    room->challenges = NULL;
    return OK;
}

/*
 * receives a room and a level and updates 'places' to the number of free places for the asked level.
 */
Result num_of_free_places_for_level(ChallengeRoom *room, Level level, int *places) {

}
Result reset_room(ChallengeRoom *room) {

}

Result num_of_free_places_for_level(ChallengeRoom *room, Level level,
                                    int *places) {

}

Result change_room_name(ChallengeRoom *room, char *new_name) {

}

Result room_of_visitor(Visitor *visitor, char **room_name) {

}

Result visitor_enter_room(ChallengeRoom *room, Visitor *visitor, Level level,
                          int start_time) {

}

/* the challenge to be chosen is the lexicographically named smaller one that has
   the required level. assume all names are different. */

Result visitor_quit_room(Visitor *visitor, int quit_time) {

}
