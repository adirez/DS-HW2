//
// Created by adire on 02-May-17.
//
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>


#include "challenge.h"

Result init_challenge(Challenge *challenge, int id, char *name, Level level) {
    if (challenge == NULL || name == NULL) {
        return NULL_PARAMETER;
    }
    challenge->name = malloc(strlen(name) * sizeof(char));
    if (challenge->name == NULL) {
        return MEMORY_PROBLEM;
    }
    for (int i = 0; i < strlen(name); ++i) {
        challenge->name[i] = name[i];
    }
    challenge->id = id;
    challenge->level = level;
    challenge->best_time = 0;
    challenge->num_visits = 0;
    return OK;
}

Result reset_challenge(Challenge *challenge) {
    if (challenge == NULL) {
        return NULL_PARAMETER;
    }
    free(challenge->name);
    challenge->id = 0;
    challenge->name = NULL;
    challenge->level = 0;
    challenge->best_time = 0;
    challenge->num_visits = 0;
    return OK;
}

Result change_name(Challenge *challenge, char *name) {
    if (challenge == NULL || name == NULL) {
        return NULL_PARAMETER;
    }
    char *newName = realloc(challenge->name, strlen(name) * sizeof(char));
    if (newName == NULL) {
        return MEMORY_PROBLEM;
    }
    challenge->name = newName;
    for (int i = 0; i < strlen(name); ++i) {
        challenge->name[i] = name[i];
    }
    return OK;
}

Result set_best_time_of_challenge(Challenge *challenge, int time) {
    if (challenge == NULL) {
        return NULL_PARAMETER;
    }
    if (time < 0 || time > challenge->best_time) {
        return ILLEGAL_PARAMETER;
    }
    challenge->best_time = time;
    return OK;
}

Result best_time_of_challenge(Challenge *challenge, int *time) {
    if (challenge == NULL) {
        return NULL_PARAMETER;
    }
    *time = challenge->best_time;
    return OK;
}

Result inc_num_visits(Challenge *challenge) {
    if (challenge == NULL) {
        return NULL_PARAMETER;
    }
    challenge->num_visits = challenge->num_visits + 1;
    return OK;
}

Result num_visits(Challenge *challenge, int *visits) {
    if (challenge == NULL) {
        return NULL_PARAMETER;
    }
    *visits = challenge->num_visits;
    return OK;
}