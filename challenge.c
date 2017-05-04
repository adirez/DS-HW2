//
// Created by adire on 02-May-17.
//
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>


#include "challenge.h"

/*
 * initializes the challenge type given by the user
 */
Result init_challenge(Challenge *challenge, int id, char *name, Level level) {
    if (challenge == NULL || name == NULL) {
        return NULL_PARAMETER;
    }
    //allocates new memory for the name and copies it
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
/*
 * resets all the params of challenge
 */
Result reset_challenge(Challenge *challenge) {
    if (challenge == NULL) {
        return NULL_PARAMETER;
    }
    free(challenge->name);
    challenge->id = 0;
    challenge->name = NULL;
    challenge->level = Easy;
    challenge->best_time = 0;
    challenge->num_visits = 0;
    return OK;
}
/*
 * changes the name of the challenge
 */
Result change_name(Challenge *challenge, char *name) {
    if (challenge == NULL || name == NULL) {
        return NULL_PARAMETER;
    }
    //to make sure that we won't lose data if realloc fails
    char *newName = realloc(challenge->name, strlen(name) + 1);
    if (newName == NULL) {
        return MEMORY_PROBLEM;
    }
    challenge->name = newName;
    free(newName); //TODO: what do you think? is it needed?
    strcpy(challenge->name, name);
    return OK;
}
/*
 * sets the best time to be the time given by the user
 */
Result set_best_time_of_challenge(Challenge *challenge, int time) {
    if (challenge == NULL) {
        return NULL_PARAMETER;
    }
    //when best time is set to 0 it means that there were no visitors yet
    //if the time is legal (meaning not negative) set it as the best time
    if (time >= 0 && challenge->best_time == 0) {
        challenge->best_time = time;
        return OK;
    }
    //if time is negative or it's not better than the current best time
    if (time < 0 || time > challenge->best_time) {
        return ILLEGAL_PARAMETER;
    }
    challenge->best_time = time;
    return OK;
}
/*
 * returns the best time through ptr
 */
Result best_time_of_challenge(Challenge *challenge, int *time) {
    if (challenge == NULL) {
        return NULL_PARAMETER;
    }
    *time = challenge->best_time;
    return OK;
}
/*
 * increases the num of visits by 1
 */
Result inc_num_visits(Challenge *challenge) {
    if (challenge == NULL) {
        return NULL_PARAMETER;
    }
    challenge->num_visits = challenge->num_visits + 1;
    return OK;
}
/*
 * returns the num of visits through ptr
 */
Result num_visits(Challenge *challenge, int *visits) {
    if (challenge == NULL) {
        return NULL_PARAMETER;
    }
    *visits = challenge->num_visits;
    return OK;
}