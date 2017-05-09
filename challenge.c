//
// Created by adire on 02-May-17.
//
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>


#include "challenge.h"

/**
 * initializes all the fields of a 'Challenge' data type.
 * @param challenge - ptr to a data type 'challenge' to initialize
 * @param id - value is inserted to challenge
 * @param name - allocates and duplicate the name to the challenge
 * @param level - value is inserted to challenge
 *        best_time & num_visits are initialized to 0
 * @return NULL_PARAMETER: if the ptr to challenge or name are NULL
 *         MEMORY_PROBLEM: if allocation problems for name have occurred
 *         OK: if everything went well
 */
Result init_challenge(Challenge *challenge, int id, char *name, Level level) {
    if (challenge == NULL || name == NULL) {
        return NULL_PARAMETER;
    }
    challenge->name = malloc(strlen(name) + 1);
    if (challenge->name == NULL) {
        return MEMORY_PROBLEM;
    }
    strcpy(challenge->name, name);

    challenge->id = id;
    challenge->level = level;
    challenge->best_time = 0;
    challenge->num_visits = 0;
    return OK;
}

/**
 * resets all the fields of challenge, frees name previously allocated memory.
 * @param challenge - ptr to a data type 'challenge' for reset
 * @return NULL_PARAMETER: if the ptr to challenge is NULL
 *         OK: if everything went well
 */
Result reset_challenge(Challenge *challenge) {
    if (challenge == NULL) {
        return NULL_PARAMETER;
    }
    challenge->id = 0;
    free(challenge->name);
    challenge->name = NULL;
    challenge->level = Easy;
    challenge->best_time = 0;
    challenge->num_visits = 0;
    return OK;
}

/**
 * changes the name field of a challenge.
 * @param challenge - ptr to the data type 'challenge'
 * @param name - wanted name for the challenge
 * @return NULL_PARAMETER: if the ptr to challenge or name are NULL
 *         MEMORY_PROBLEM: if allocation problems for name have occurred
 *         OK: if everything went well
 */
Result change_name(Challenge *challenge, char *name) {
    if (challenge == NULL || name == NULL) {
        return NULL_PARAMETER;
    }
    //to make sure that we won't lose data if realloc fails
    char *name_ptr = realloc(challenge->name, strlen(name) + 1);
    if (name_ptr == NULL) {
        return MEMORY_PROBLEM;
    }
    challenge->name = name_ptr;
    strcpy(challenge->name, name);
    return OK;
}

/**
 * set the best time of a challenge.
 * @param challenge - ptr to the data type 'challenge'
 * @param time - the wanted value for best time
 * @return NULL_PARAMETER: if the ptr to challenge is NULL
 *         ILLEGAL_PARAMETER: if time is negative or worse than current best
 *         OK: if everything went well
 */
Result set_best_time_of_challenge(Challenge *challenge, int time) {
    if (challenge == NULL) {
        return NULL_PARAMETER;
    }
    //when best time is set to 0 it means that there were no visitors yet
    if (time >= 0 && time <= challenge->best_time) {
        challenge->best_time = time;
        return OK;
    }
    if (time < 0 || time > challenge->best_time) {
        return ILLEGAL_PARAMETER;
    }
    challenge->best_time = time;
    return OK;
}

/**
 * returns the best time through ptr.
 * @param challenge - ptr to the data type 'challenge'
 * @param time - the ptr that needs to be updated
 * @return NULL_PARAMETER: if the ptr to challenge is NULL
 *         OK: if everything went well
 */
Result best_time_of_challenge(Challenge *challenge, int *time) {
    if (challenge == NULL) {
        return NULL_PARAMETER;
    }
    *time = challenge->best_time;
    return OK;
}

/**
 * increases the num of visits by 1.
 * @param challenge - ptr to the data type 'challenge'
 * @return NULL_PARAMETER: if the ptr to challenge is NULL
 *         OK: if everything went well
 */
Result inc_num_visits(Challenge *challenge) {
    if (challenge == NULL) {
        return NULL_PARAMETER;
    }
    challenge->num_visits = challenge->num_visits + 1;
    return OK;
}

/**
 * returns the num of visits through ptr.
 * @param challenge  - ptr to the data type 'challenge'
 * @param visits - the ptr that needs to be updated
 * @return NULL_PARAMETER: if the ptr to challenge is NULL
 *         OK: if everything went well
 */
Result num_visits(Challenge *challenge, int *visits) {
    if (challenge == NULL) {
        return NULL_PARAMETER;
    }
    *visits = challenge->num_visits;
    return OK;
}