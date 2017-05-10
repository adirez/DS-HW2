//
// Created by adire on 02-May-17.
//

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>


#include "visitor_room.h"

/**
 * initializes all the fields of a 'ChallengeActivity' data type.
 * @param activity - ptr to a data type 'challenge_activity' to initialize
 * @param challenge - ptr to a challenge to connect it to the activity
 *        start_time is initialized to 0, visitor is set to NULL
 * @return NULL_PARAMETER: if the ptr to activity or challenge are NULL
 *         OK: if everything went well
 */
Result init_challenge_activity(ChallengeActivity *activity,
                               Challenge *challenge) {
    assert(activity != NULL && challenge != NULL);
    if (activity == NULL || challenge == NULL) {
        return NULL_PARAMETER;
    }
    activity->challenge = challenge;
    activity->start_time = 0;
    activity->visitor = NULL;
    return OK;
}

/**
 * resets all the fields of an activity.
 * @param activity - ptr to a data type 'ChallengeActivity' for reset
 * @return NULL_PARAMETER: if the ptr to activity are NULL
 *         OK: if everything went well
 */
Result reset_challenge_activity(ChallengeActivity *activity) {
    assert(activity != NULL);
    if (activity == NULL) {
        return NULL_PARAMETER;
    }
    activity->challenge = NULL;
    activity->visitor = NULL;
    activity->start_time = 0;
    return OK;
}

/**
 * initializes all the fields of a 'Visitor' data type.
 * @param visitor - ptr to a data type 'Visitor' to initialize
 * @param name - allocates and duplicate the name to the visitor
 * @param id - value is inserted to visitor
 *        room_name & current_challenge are set to NULL
 * @return NULL_PARAMETER: if the ptr to visitor or name are NULL
 *         MEMORY_PROBLEM: if allocation problems for name have occurred
 *         OK: if everything went well
 */
Result init_visitor(Visitor *visitor, char *name, int id) {
    assert(visitor != NULL && name != NULL);
    if (visitor == NULL || name == NULL) {
        return NULL_PARAMETER;
    }
    visitor->visitor_name =  malloc(strlen(name) + 1);
    if (visitor->visitor_name == NULL) {
        return MEMORY_PROBLEM;
    }
    strcpy(visitor->visitor_name, name);

    visitor->visitor_id = id;
    visitor->room_name = NULL;
    visitor->current_challenge = NULL;
    return OK;
}

/**
 * resets all the fields of visitor, frees name previously allocated memory.
 * @param visitor - ptr to a data type 'Visitor' for reset
 * @return NULL_PARAMETER: if the ptr to visitor is NULL
 *         OK: if everything went well
 */
Result reset_visitor(Visitor *visitor) {
    assert(visitor != NULL);
    if (visitor == NULL) {
        return NULL_PARAMETER;
    }
    free(visitor->visitor_name);
    visitor->visitor_name = NULL;
    visitor->visitor_id = 0;
    *(visitor->room_name) = NULL;
    visitor->room_name = NULL;
    visitor->current_challenge = NULL;
    return OK;
}

/**
 * initializes all the fields of a 'ChallengeRoom' data type.
 * @param room - ptr to a data type 'ChallengeRoom' to initialize
 * @param name - allocates and duplicate the name to the room
 * @param num_challenges - value is inserted to room
 *        allocates memory for an array of the type 'ChallengeActivity'
 *        according to the num of challenges
 * @return NULL_PARAMETER: if the ptr to room or name is NULL
 *         ILLEGAL_PARAMETER: if num_challenges is less than 1
 *         OK: if everything went well
 */
Result init_room(ChallengeRoom *room, char *name, int num_challenges) {
    assert(room != NULL && name != NULL);
    if (room == NULL || name == NULL) {
        return NULL_PARAMETER;
    }
    if (num_challenges < 1) {
        return ILLEGAL_PARAMETER;
    }

    room->name = malloc(strlen(name) + 1);
    if (room->name == NULL) {
        return MEMORY_PROBLEM;
    }
    strcpy(room->name, name);

    room->challenges = malloc(num_challenges * sizeof(*(room->challenges)));
    if (room->challenges == NULL) {
        //free the already allocated memory name
        free(room->name);
        room->name = NULL;
        return MEMORY_PROBLEM;
    }

    room->num_of_challenges = num_challenges;
    return OK;
}

/**
 * resets all the fields of room, frees name & challenge activity array
 * previously allocated memory.
 * @param room - ptr to a data type 'ChallengeRoom' for reset
 * @return NULL_PARAMETER: if the ptr to room is NULL
 *         OK: if everything went well
 */
Result reset_room(ChallengeRoom *room) {
    assert(room != NULL);
    if (room == NULL) {
        return NULL_PARAMETER;
    }
    free(room->name);
    room->name = NULL;
    //loops through all the challenge activities in the room and resets them
    for (int i = 0; i < room->num_of_challenges; ++i) {
        reset_challenge_activity(room->challenges + i);
    }
    free(room->challenges);
    room->challenges = NULL;
    room->num_of_challenges = 0;
    return OK;
}

/**
 * returns the num of available challenges in a specific room & a wanted level.
 * @param room - ptr to a data type 'ChallengeRoom'
 * @param level - wanted level of challenge
 * @param places - the ptr that needs to be updated
 * @return NULL_PARAMETER: if the ptr to room is NULL
 *         OK: if everything went well
 */
Result num_of_free_places_for_level(ChallengeRoom *room, Level level,
                                    int *places) {
    assert(room != NULL);
    if (room == NULL) {
        return NULL_PARAMETER;
    }
    //if the level is All_Levels returns the num of challenges in the room
    if (level == All_Levels) {
        *places = room->num_of_challenges;
        return OK;
    }

    int count = 0;
    for (int i = 0; i < room->num_of_challenges; ++i) {
        //checks the challenge's level & that the room is empty
        if (room->challenges[i].challenge->level == level
            && room->challenges[i].visitor == NULL) {
            count++;
        }
    }
    *places = count;
    return OK;
}

/**
 * changes the name field of a room.
 * @param room - ptr to a data type 'ChallengeRoom'
 * @param new_name - wanted name for the room
 * @return NULL_PARAMETER: if the ptr to room or new_name are NULL
 *         MEMORY_PROBLEM: if allocation problems for new_name have occurred
 *         OK: if everything went well
 */
Result change_room_name(ChallengeRoom *room, char *new_name) {
    assert(room != NULL && new_name != NULL);
    if (room == NULL || new_name == NULL) {
        return NULL_PARAMETER;
    }

    //to make sure that we won't lose data if realloc fails
    char *name_ptr = realloc(room->name, strlen(new_name) + 1);
    if (name_ptr == NULL) {
        return MEMORY_PROBLEM;
    }
    room->name = name_ptr;
    strcpy(room->name, new_name);
    return OK;
}

/**
 * returns the room of a visitor through ptr.
 * @param visitor - ptr to the visitor
 * @param room_name - the ptr that needs to be updated
 * @return NULL_PARAMETER: if the ptr to room_name is NULL
 *         NOT_IN_ROOM: if the ptr to visitor is NULL or the room_name field
 *                      of visitor is NULL
 *         MEMORY_PROBLEM: if allocation problems for room_name have occurred
 *         OK: if everything went well
 */
Result room_of_visitor(Visitor *visitor, char **room_name) {
    assert(room_name != NULL);
    if (room_name == NULL) {
        return NULL_PARAMETER;
    }
    if (visitor == NULL || *(visitor->room_name) == NULL) {
        return NOT_IN_ROOM;
    }
    *room_name = malloc(strlen(*(visitor->room_name)) + 1);
    if (*room_name == NULL) {
        return MEMORY_PROBLEM;
    }
    strcpy(*room_name, *(visitor->room_name));
    return OK;
}

/**
 * finds the smallest lexicographically challenge that matches the required
 * level and non taken.
 * @param room - ptr to the room
 * @param level - wanted level of challenge
 * @return the idx of the challenge
 */
static int find_lex_smallest(ChallengeRoom *room, Level level) {
    assert(room != NULL);
    int challenge_idx = 0;
    int num_challenges = room->num_of_challenges;
    for (int i = 0; i < num_challenges; ++i) {
        if ((level == All_Levels ||
             room->challenges[i].challenge->level == level) &&
            room->challenges[i].visitor == NULL) {

            if (challenge_idx == 0 ||
                strcmp(room->challenges[i].challenge->name,
                       room->challenges[challenge_idx].challenge->name) < 0) {
                challenge_idx = i;
            }
        }
    }
    return challenge_idx;
}

/**
 * updates the fields of the room and visitor when a visitor entered a challenge
 * @param room - ptr to the room
 * @param visitor - ptr to the visitor
 * @param challenge_idx - the idx of the challenge that the visitor is entering
 * @param start_time - the time that the visitor entered the room
 * @return MEMORY_PROBLEM: if allocation problems for room_name have occurred
 *         OK: if everything went well
 */
static Result visitor_update_fields(ChallengeRoom *room, Visitor *visitor,
                                    int challenge_idx, int start_time) {
    assert(room != NULL && visitor != NULL);
    //updates the room_name field in the visitor
    *(visitor->room_name) = malloc(strlen(room->name) + 1);
    if (*(visitor->room_name) == NULL) {
        return MEMORY_PROBLEM;
    }
    strcpy(*(visitor->room_name), room->name);
    //updates the chosen ChallengeActivity in the room
    room->challenges[challenge_idx].visitor = visitor;
    room->challenges[challenge_idx].start_time = start_time;
    //connecting the ChallengeActivity ptr to the Visitor
    visitor->current_challenge = &(room->challenges[challenge_idx]);
    //increase the num of visits for the Challenge
    return inc_num_visits(visitor->current_challenge->challenge);
}

/**
 * answers the request of a visitor to enter a room, the chosen challenge
 * will be the lexicographically named smaller, available and matching the
 * wanted level one.
 * @param room - ptr to the room
 * @param visitor - ptr to the visitor
 * @param level - wanted level of challenge
 * @param start_time - the time that the visitor arrive
 * @return NULL_PARAMETER: if the ptr to room or visitor are NULL
 *         ALREADY_IN_ROOM: if the visitor is already in a room
 *         NO_AVAILABLE_CHALLENGES: if there are no available matching
 *                                  challenges in the room
 *         OK: if everything went well
 */
Result visitor_enter_room(ChallengeRoom *room, Visitor *visitor, Level level,
                          int start_time) {
    assert(room != NULL && visitor != NULL);
    if (room == NULL || visitor == NULL) {
        return NULL_PARAMETER;
    }
    if (visitor->room_name != NULL) {
        return ALREADY_IN_ROOM;
    }
    int matching_challenges = 0;
    num_of_free_places_for_level(room, level, &matching_challenges);
    if (matching_challenges == 0) {
        return NO_AVAILABLE_CHALLENGES;
    }
    int challenge_idx = find_lex_smallest(room, level);
    assert(challenge_idx != 0);

    return visitor_update_fields(room, visitor, challenge_idx, start_time);
}

/**
 * updates all the fields when a visitor is quiting a room
 * @param visitor - ptr to the visitor
 * @param quit_time - the time in which the visitor has left
 * @return NULL_PARAMETER: if the ptr to visitor is NULL
 *         NOT_IN_ROOM: if the visitor is currently not in a room
 *         OK: if everything went well
 */
Result visitor_quit_room(Visitor *visitor, int quit_time) {
    assert(visitor != NULL);
    if (visitor == NULL) {
        return NULL_PARAMETER;
    }
    if (*(visitor->room_name) == NULL) {
        return NOT_IN_ROOM;
    }
    //calculates the total time that took the visitor to finish the challenge
    int visitor_total_time = quit_time -
                             (visitor->current_challenge->start_time);
    //update the best time in the Challenge
    if (set_best_time_of_challenge(visitor->current_challenge->challenge,
                                   visitor_total_time) == NULL_PARAMETER) {
        return NULL_PARAMETER;
    }
    return OK;
}