//
// Created by adire on 02-May-17.
//

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>

#include "challenge_system.h"

#define WORD_MAX_LEN 51

#define CREATE_RESULT_CHECK(result)\
    if (result != OK){\
        fclose(input);\
        free((*sys));\
        return result;\
    }
#define RESULT_STANDARD_CHECK(result)\
    if (result != OK){\
        return result;\
    }

/* deceleration for static functions */

static void free_system_name(ChallengeRoomSystem *sys);

static void free_system_challenges_and_previous(ChallengeRoomSystem *sys);

static void free_system_rooms_and_previous(ChallengeRoomSystem *sys);

static Result update_system_name(ChallengeRoomSystem *sys, FILE *input_file);

static Result create_system_challenges(ChallengeRoomSystem *sys,
                                       FILE *input_file);

static Result add_challenge_to_room(ChallengeRoomSystem *sys, int challenge_id,
                                    int activity_idx, int room_idx);

static Result rooms_add_challenge_activities(ChallengeRoomSystem *sys,
                                             FILE *input_file);

static Result create_system_rooms(ChallengeRoomSystem *sys, FILE *input_file);

static Result create_system_visitor_list_head(ChallengeRoomSystem *sys);

static Result system_lowest_best_time(ChallengeRoomSystem *sys,
                                      char **challenge_best_time);

static void destroy_visitor_node(ChallengeRoomSystem *sys, Visitor *visitor);

static Visitor *find_visitor_by_id(ChallengeRoomSystem *sys, int visitor_id);

static Result create_visitor_node(ChallengeRoomSystem *sys, char *visitor_name,
                                  int visitor_id);

static Result find_room_by_name(ChallengeRoomSystem *sys, char *room_name,
                                int *room_idx);

/**
 * creates the system according to the specifications from the file
 * @param init_file - the file with all the specifications
 * @param sys - ptr to a data type 'ChallengeRoomSystem' for creation
 * @return NULL_PARAMETER: if the ptr to sys or init_file are NULL
 *         MEMORY_PROBLEM: if allocation problems have occurred
 *         OK: if everything went well
 */
Result create_system(char *init_file, ChallengeRoomSystem **sys) {
    if (init_file == NULL || sys == NULL) {
        return NULL_PARAMETER;
    }
    (*sys) = malloc(sizeof(**sys));
    (*sys)->system_last_known_time = 0;
    (*sys)->system_num_rooms = 0;
    (*sys)->system_num_challenges = 0;
    FILE *input = fopen(init_file, "r");
    if (input == NULL) {
        return MEMORY_PROBLEM; //TODO not sure if that's the right Error
    }
    Result result = update_system_name(*sys, input);
    CREATE_RESULT_CHECK(result);
    result = create_system_challenges(*sys, input);
    CREATE_RESULT_CHECK(result);
    result = create_system_rooms(*sys, input);
    CREATE_RESULT_CHECK(result);
    result = create_system_visitor_list_head(*sys);
    CREATE_RESULT_CHECK(result);
    fclose(input);
    return OK;
}

/**
 * resets any memory and frees any allocated memory from the system.
 * also force quit for any visitor left in the system.
 * returns the most popular challenge through ptr, if there are some with the
 * same num of visits the smallest lexicographically will be returned, if
 * there were no visitors NULL will be returned.
 * also returns the name of the challenge with the lowest best_time param,
 * if there are some the smallest lexicographically will be returned, if
 * there were no visitors 0 will be returned.
 * @param sys - ptr to the system
 * @param destroy_time - the current time
 * @param most_popular_challenge_p - the ptr that needs to be updated
 * @param challenge_best_time - the ptr that needs to be updated
 * @return NULL_PARAMETER: if the ptr to sys or most_popular_challenge_p or
 *                         challenge_best_time are NULL
 *         ILLEGAL_TIME: if the destroy_time is not greater or equal than the
 *                       last time known to the system
 *         MEMORY_PROBLEM: if allocation problems have occurred
 *         OK: if everything went well
 */
Result destroy_system(ChallengeRoomSystem *sys, int destroy_time,
                      char **most_popular_challenge_p,
                      char **challenge_best_time) {
    if (sys == NULL || most_popular_challenge_p == NULL ||
        challenge_best_time == NULL) {
        return NULL_PARAMETER;
    }
    if (destroy_time < sys->system_last_known_time) {
        return ILLEGAL_TIME;
    }
    Result result = most_popular_challenge(sys, most_popular_challenge_p);
    RESULT_STANDARD_CHECK(result);
    result = all_visitors_quit(sys, destroy_time);
    if (result != OK) {
        return result;
    }
    free(sys->visitors_list_head);
    result = system_lowest_best_time(sys, challenge_best_time);
    RESULT_STANDARD_CHECK(result);

    free_system_rooms_and_previous(sys);
    sys->system_num_rooms = 0;
    sys->system_num_challenges = 0;
    sys->system_last_known_time = 0;
    free(sys);
    return OK;
}

/**
 * receives a request of a visitor to enter a room in a requested level
 * if the visitor is not in the system yet it adds the visitor to the system
 * updates the system accordingly
 * @param sys - ptr to the system
 * @param room_name - the room the visitor wants to enter
 * @param visitor_name - the name of the visitor
 * @param visitor_id - the id of the visitor
 * @param level - the wanted challenge level
 * @param start_time - the current time
 * @return NULL_PARAMETER: if the ptr to sys is NULL
 *         ILLEGAL_TIME: if the start_time is not greater or equal than the
 *                       last time known to the system
 *         ILLEGAL_PARAMETER: if room or visitor are NULL
 *         MEMORY_PROBLEM: if allocation problems have occurred
 *         ALREADY_IN_ROOM: if the visitor is already in a room
 *         NO_AVAILABLE_CHALLENGES: if there are no available matching
 *                                  challenges in the room
 *         OK: if everything went well
 */
Result visitor_arrive(ChallengeRoomSystem *sys, char *room_name,
                      char *visitor_name, int visitor_id, Level level,
                      int start_time) {
    if (sys == NULL) {
        return NULL_PARAMETER;
    }
    if (start_time < sys->system_last_known_time) {
        return ILLEGAL_TIME;
    }
    if (visitor_name == NULL || room_name == NULL) {
        return ILLEGAL_PARAMETER;
    }
    Visitor *visitor = find_visitor_by_id(sys, visitor_id);
    if (visitor == NULL) {
        Result result = create_visitor_node(sys, visitor_name, visitor_id);
        RESULT_STANDARD_CHECK(result);
    } else {
        if (visitor->room_name != NULL) {
            return ALREADY_IN_ROOM;
        }
    }
    int room_idx = 0;
    Result result = find_room_by_name(sys, room_name, &room_idx);
    if (result != OK) {
        destroy_visitor_node(sys, sys->visitors_list_head->next->visitor);
        return result;
    }
    result = visitor_enter_room(sys->system_rooms + room_idx,
                                sys->visitors_list_head->next->visitor,
                                level, start_time);
    if (result != OK) {
        destroy_visitor_node(sys, sys->visitors_list_head->next->visitor);
        return result;
    }
    sys->system_last_known_time = start_time;
    return OK;
}

/**
 * updates the system when a visitor is getting out of a room
 * @param sys - ptr to the system
 * @param visitor_id - the id of the visitor
 * @param quit_time - the current time
 * @return NULL_PARAMETER: if the ptr to sys is NULL
 *         ILLEGAL_TIME: if the quit_time is not greater or equal than the
 *                       last time known to the system
 *         NOT_IN_ROOM: if the visitor is not in a room or visitor_id is not
 *                      found in the system
 *         OK: if everything went well
 */
Result visitor_quit(ChallengeRoomSystem *sys, int visitor_id, int quit_time) {
    if (sys == NULL) {
        return NULL_PARAMETER;
    }
    if (quit_time < sys->system_last_known_time) {
        return ILLEGAL_TIME;
    }
    Visitor *visitor = find_visitor_by_id(sys, visitor_id);
    if (visitor == NULL) {
        return NOT_IN_ROOM;
    }
    sys->system_last_known_time = quit_time;
    Result result = visitor_quit_room(visitor, quit_time);
    if (result != OK) {
        return result;
    }
    destroy_visitor_node(sys, visitor);
    return OK;
}

/**
 * updates the system when all visitors are getting out of their rooms
 * @param sys - ptr to the system
 * @param quit_time - the current time
 * @return NULL_PARAMETER: if the ptr to sys is NULL
 *         ILLEGAL_TIME: if the quit_time is not greater or equal than the
 *                       last time known to the system
 *         MEMORY_PROBLEM: if allocation problems have occurred
 *         NOT_IN_ROOM: if the visitor is not in a room
 *         OK: if everything went well
 */
Result all_visitors_quit(ChallengeRoomSystem *sys, int quit_time) {
    if (sys == NULL) {
        return NULL_PARAMETER;
    }
    if (quit_time < sys->system_last_known_time) {
        return ILLEGAL_TIME;
    }

    VisitorsList ptr = sys->visitors_list_head->next;
    while (ptr != NULL) {
        Result result = visitor_quit_room(ptr->visitor, quit_time);
        RESULT_STANDARD_CHECK(result);
        VisitorsList tmp_ptr = ptr->next;
        destroy_visitor_node(sys, ptr->visitor);
        ptr = tmp_ptr;
    }
    sys->system_last_known_time = quit_time;
    return OK;
}

/**
 * returns the room name in which the visitor is in
 * @param sys - ptr to the system
 * @param visitor_name - the name of the visitor
 * @param room_name - the ptr that needs to be updated
 * @return NULL_PARAMETER: if the ptr to sys is NULL
 *         ILLEGAL_PARAMETER: if a visitor_name or room_name are NULL
 *         NOT_IN_ROOM: if the visitor is not in a room
 *         OK: if everything went well
 */
Result system_room_of_visitor(ChallengeRoomSystem *sys, char *visitor_name,
                              char **room_name) {
    if (sys == NULL) {
        return NULL_PARAMETER;
    }
    if (visitor_name == NULL || room_name == NULL) {
        return ILLEGAL_PARAMETER;
    }
    VisitorsList ptr = sys->visitors_list_head->next;
    while (ptr != NULL) {
        if (strcmp(ptr->visitor->visitor_name, visitor_name) == 0) {
            Result result = room_of_visitor(ptr->visitor, room_name);
            return result;
        }
        ptr = ptr->next;
    }
    //if we got here it means that the visitor is not in the system
    return NOT_IN_ROOM;
}

/**
 * changes the name of a challenge in the system
 * @param sys - ptr to the system
 * @param challenge_id - the id of the challenge
 * @param new_name - the wanted name for the challenge
 * @return NULL_PARAMETER: if the ptr to sys or new_name are NULL
 *         ILLEGAL_PARAMETER: if a challenge with the name given is not found
 *         OK: if everything went well
 */
Result change_challenge_name(ChallengeRoomSystem *sys, int challenge_id,
                             char *new_name) {
    if (sys == NULL || new_name == NULL) {
        return NULL_PARAMETER;
    }
    for (int i = 0; i < sys->system_num_challenges; ++i) {
        if ((sys->system_challenges + i)->id == challenge_id) {
            Result result = change_name(sys->system_challenges + i, new_name);
            RESULT_STANDARD_CHECK(result);
            return OK;
        }
    }
    //did'nt find a challenge with the id given
    return ILLEGAL_PARAMETER;
}

/**
 * changes the name of a room in the system
 * @param sys - ptr to the system
 * @param current_name - the name of the room
 * @param new_name - the wanted name of the room
 * @return NULL_PARAMETER: if the ptr to sys, current_name or new_name are NULL
 *         ILLEGAL_PARAMETER: if a room with the name given is not found
 *         OK: if everything went well
 */
Result change_system_room_name(ChallengeRoomSystem *sys, char *current_name,
                               char *new_name) {
    if (sys == NULL || current_name == NULL || new_name == NULL) {
        return NULL_PARAMETER;
    }
    int room_idx = 0;
    Result result = find_room_by_name(sys, current_name, &room_idx);
    RESULT_STANDARD_CHECK(result);
    result = change_room_name(sys->system_rooms + room_idx, new_name);
    return result;
}

/**
 * returns the best time for a specific challenge
 * @param sys - ptr to the system
 * @param challenge_name - the name of the challenge
 * @param time - the ptr that need to be updated
 * @return NULL_PARAMETER: if the ptr to sys, challenge_name or time are NULL
 *         ILLEGAL_PARAMETER: if a challenge with the name given is not found
 *         OK: if everything went well
 */
Result best_time_of_system_challenge(ChallengeRoomSystem *sys,
                                     char *challenge_name, int *time) {
    if (sys == NULL || challenge_name == NULL || time == NULL) {
        return NULL_PARAMETER;
    }
    for (int i = 0; i < sys->system_num_challenges; ++i) {
        if (strcmp((sys->system_challenges + i)->name, challenge_name) == 0) {
            Result result = best_time_of_challenge(sys->system_challenges + i,
                                                   time);
            RESULT_STANDARD_CHECK(result);
            return OK;
        }
    }
    //did'nt find a challenge with the name given
    return ILLEGAL_PARAMETER;
}

/**
 * returns the challenge with the highest num of visits in the room, in case
 * there are more than one, the lexicographically smallest one will be returned
 * @param sys - ptr to the system
 * @param challenge_name - the ptr that needs to be updated
 * @return NULL_PARAMETER: if the ptr to sys or challenge_name are NULL
 *         MEMORY_PROBLEM: if allocation problems have occurred
 *         OK: if everything went well
 */
Result most_popular_challenge(ChallengeRoomSystem *sys, char **challenge_name) {
    if (sys == NULL || challenge_name == NULL) {
        return NULL_PARAMETER;
    }
    int max_idx = 0, max = 0, curr = 0;
    Result result = num_visits(sys->system_challenges, &max);
    RESULT_STANDARD_CHECK(result);
    for (int i = 1; i < sys->system_num_challenges; ++i) {
        result = num_visits(sys->system_challenges + i, &curr);
        RESULT_STANDARD_CHECK(result);
        if (curr > max) {
            max_idx = i;
            max = curr;
        } else if (curr == max && strcmp((sys->system_challenges + i)->name,
                                         (sys->system_challenges +
                                          max_idx)->name) < 0) {
            max_idx = i;
        }
    }
    if (max == 0) {
        //no visits in any of the rooms
        *challenge_name = NULL;
    } else {
        *challenge_name = malloc(
                strlen((sys->system_challenges + max_idx)->name) + 1);
        if (*challenge_name == NULL) {
            return MEMORY_PROBLEM;
        }
        strcpy(*challenge_name, (sys->system_challenges + max_idx)->name);
    }
    return OK;
}

/**
 * free the allocated memory of the system
 * @param sys - ptr to the system
 */
static void free_system_name(ChallengeRoomSystem *sys) {
    free(sys->system_name);
    return;
}

/**
 * frees all the memory that was allocated previously and the memory of
 * each challenge and the challenge array itself.
 * @param sys - ptr to the system
 * @param num_challenges - num of the challenges in the system
 */
static void free_system_challenges_and_previous(ChallengeRoomSystem *sys) {
    free_system_name(sys);
    for (int i = 0; i < sys->system_num_challenges; ++i) {
        reset_challenge(sys->system_challenges + i);
    }
    free(sys->system_challenges);
    sys->system_num_challenges = 0;
    return;
}

/**
 * frees all the memory that was allocated previously and the memory of
 * each room and the room array itself.
 * @param sys - ptr to system
 */
static void free_system_rooms_and_previous(ChallengeRoomSystem *sys) {
    free_system_challenges_and_previous(sys);
    for (int i = 0; i < sys->system_num_rooms; ++i) {
        reset_room(sys->system_rooms + i);
    }
    free(sys->system_rooms);
    return;
}

/**
 * updates the name field in the system
 * @param sys - ptr to the system
 * @param input_file - the file with the wanted name
 * @return MEMORY_PROBLEM: if allocation problems have occurred
 *         OK: if everything went well
 */
static Result update_system_name(ChallengeRoomSystem *sys, FILE *input_file) {
    char name[WORD_MAX_LEN] = "";
    fscanf(input_file, "%s\n", name);
    sys->system_name = malloc(strlen(name) + 1);
    if (sys->system_name == NULL) {
        return MEMORY_PROBLEM;
    }
    strcpy(sys->system_name, name);
    return OK;
}

/**
 * creates the challenges array in the system
 * @param sys - ptr to the system
 * @param input_file - the file with the specifications for the challenges
 * @return MEMORY_PROBLEM: if allocation problems have occurred
 *         OK: if everything went well
 */
static Result create_system_challenges(ChallengeRoomSystem *sys,
                                       FILE *input_file) {
    char challenge_name[WORD_MAX_LEN] = "";
    fscanf(input_file, "%d\n", &sys->system_num_challenges);
    sys->system_challenges = malloc(sys->system_num_challenges *
                                    sizeof(*sys->system_challenges));
    if (sys->system_challenges == NULL) {
        free_system_name(sys);
        return MEMORY_PROBLEM;
    }

    for (int i = 0; i < sys->system_num_challenges; ++i) {
        int level = 0, id = 0;
        fscanf(input_file, "%s %d %d\n", challenge_name, &id, &level);
        Result result = init_challenge((sys->system_challenges + i), id,
                                       challenge_name, (Level) level - 1);
        if (result != OK) {
            free_system_challenges_and_previous(sys);
            return result;
        }
    }
    return OK;
}

/**
 * finds the right challenge by id and initialize the activity accordingly
 * @param sys - ptr to the system
 * @param challenge_id - the id of the wanted challenge
 * @param activity_idx - the idx of the current activity
 * @return MEMORY_PROBLEM: if allocation problems have occurred
 *         OK: if everything went well
 */
static Result add_challenge_to_room(ChallengeRoomSystem *sys, int challenge_id,
                                    int activity_idx, int room_idx) {
    for (int i = 0; i < sys->system_num_challenges; ++i) {
        if ((sys->system_challenges + i)->id == challenge_id) {
            Result result = init_challenge_activity(
                    ((sys->system_rooms + room_idx)->challenges + activity_idx),
                    sys->system_challenges + i);
            if (result != OK) {
                free_system_rooms_and_previous(sys);
                return result;
            }
            return OK;
        }
    }
    return OK;
}

/**
 * adds the challenge activities to each room
 * @param sys - ptr to the system
 * @param input_file - the file with the specifications for the activities
 * @return MEMORY_PROBLEM: if allocation problems have occurred
 *         OK: if everything went well
 */
static Result rooms_add_challenge_activities(ChallengeRoomSystem *sys,
                                             FILE *input_file) {
    char room_name[WORD_MAX_LEN] = "";
    for (int i = 0; i < sys->system_num_rooms; ++i) {
        int num_challenges_in_room = 0;
        fscanf(input_file, "%s %d", room_name, &num_challenges_in_room);
        Result result = init_room((sys->system_rooms + i), room_name,
                                  num_challenges_in_room);
        if (result != OK) {
            free_system_rooms_and_previous(sys);
            return result;
        }
        for (int j = 0; j < num_challenges_in_room; ++j) {
            int challenge_id = 0;
            if (j != num_challenges_in_room - 1) {
                fscanf(input_file, "%d", &challenge_id);
            } else {
                //if that's the last id move cursor to the start of next line
                fscanf(input_file, "%d\n", &challenge_id);
            }
            result = add_challenge_to_room(sys, challenge_id, j, i);
            if (result != OK) {
                free_system_rooms_and_previous(sys);
                return result;
            }
        }
    }
    return OK;
}

/**
 * creates the rooms array in the system
 * @param sys - ptr to the system
 * @param input_file - the file with the specifications for the rooms
 * @return MEMORY_PROBLEM: if allocation problems have occurred
 *         OK: if everything went well
 */
static Result create_system_rooms(ChallengeRoomSystem *sys, FILE *input_file) {
    fscanf(input_file, "%d\n", &sys->system_num_rooms);
    sys->system_rooms = malloc(sys->system_num_rooms * sizeof
    (*sys->system_rooms));
    if (sys->system_rooms == NULL) {
        free_system_challenges_and_previous(sys);
        return NULL_PARAMETER;
    }
    return rooms_add_challenge_activities(sys, input_file);
}

/**
 * creates the head of the head of the linked list of visitors
 * @param sys - ptr to the system
 * @return MEMORY_PROBLEM: if allocation problems have occurred
 *         OK: if everything went well
 */
static Result create_system_visitor_list_head(ChallengeRoomSystem *sys) {
    sys->visitors_list_head = malloc(sizeof(*(sys->visitors_list_head)));
    if (sys->visitors_list_head == NULL) {
        return MEMORY_PROBLEM;
    }
    sys->visitors_list_head->visitor = NULL;
    sys->visitors_list_head->next = NULL;
    return OK;
}

/**
 * returns the challenge with the lowest best_time in the system
 * @param sys - ptr to the system
 * @param challenge_best_time - the ptr that needs to be updated
 * @return MEMORY_PROBLEM: if allocation problems have occurred
 *         OK: if everything went well
 */
static Result system_lowest_best_time(ChallengeRoomSystem *sys,
                                      char **challenge_best_time) {
    assert(sys != NULL);
    int min_idx = 0, min = 0;
    for (int i = 0; i < sys->system_num_challenges; ++i) {
        if ((sys->system_challenges + i)->num_visits > 0) {
            min = (sys->system_challenges + i)->best_time;
            break;
        }
    }
    if (min == 0) {
        *challenge_best_time = NULL;
        return OK;
    }
    for (int i = 0; i < sys->system_num_challenges; ++i) {
        if ((sys->system_challenges + i)->best_time != 0) {
            if (((sys->system_challenges + i)->best_time < min)) {
                min = (sys->system_challenges + i)->best_time;
                min_idx = i;
            } else if ((sys->system_challenges + i)->best_time == min &&
                       strcmp((sys->system_challenges + i)->name,
                              (sys->system_challenges + min_idx)->name) < 0) {
                min_idx = i;
            }
        }
    }
    *challenge_best_time = malloc(strlen((sys->system_challenges + min_idx)
                                                 ->name) + 1);
    if (*challenge_best_time == NULL) {
        return MEMORY_PROBLEM;
    }
    strcpy(*challenge_best_time, (sys->system_challenges + min_idx)->name);
    return OK;
}
/**
 * creates a new node to the visitor list
 * initialize the new visitor
 * @param sys - ptr to the system
 * @param visitor_name - the name of the visitor
 * @param visitor_id - the id of the visitor
 * @return MEMORY_PROBLEM: if allocation problems have occurred
 *         OK: if everything went well
 */
static Result create_visitor_node(ChallengeRoomSystem *sys, char *visitor_name,
                                  int visitor_id) {
    assert(sys != NULL && visitor_name != NULL);
    VisitorsList new_node = malloc(sizeof(*new_node));
    if (new_node == NULL) {
        return MEMORY_PROBLEM;
    }
    new_node->visitor = malloc(sizeof(*new_node->visitor));
    if (new_node->visitor == NULL) {
        free(new_node);
        return MEMORY_PROBLEM;
    }
    Result result = init_visitor(new_node->visitor, visitor_name, visitor_id);
    if (result != OK) {
        free(new_node);
        return result;
    }
    VisitorsList tmp_node = sys->visitors_list_head->next;
    sys->visitors_list_head->next = new_node;
    new_node->next = tmp_node;
    return OK;
}

/**
 * finds a room by its name
 * @param sys - ptr to the system
 * @param room_name - the name of the wanted room
 * @param room_idx - ptr to the room's idx
 * @return ILLEGAL_PARAMETER: if a challenge with the name given is not found
 *         OK: if everything went well
 */
static Result find_room_by_name(ChallengeRoomSystem *sys, char *room_name,
                                int *room_idx) {
    assert(sys != NULL && room_name != NULL && room_idx != NULL);
    for (int i = 0; i < sys->system_num_rooms; ++i) {
        if (strcmp((sys->system_rooms + i)->name, room_name) == 0) {
            *room_idx = i;
            return OK;
        }
    }
    return ILLEGAL_PARAMETER;
}

/**
 * resets and frees the allocated memory of a visitor and its node in the list
 * @param sys - ptr to the system
 * @param visitor - the wanted visitor to be destroyed
 */
static void destroy_visitor_node(ChallengeRoomSystem *sys, Visitor *visitor) {
    assert(sys != NULL && visitor != NULL);
    VisitorsList ptr = sys->visitors_list_head;
    while (ptr->next != NULL && ptr->next->visitor != visitor) {
        ptr = ptr->next;
    }
    VisitorsList tmp_node = ptr->next->next;
    reset_visitor(ptr->next->visitor);
    free(ptr->next->visitor);
    free(ptr->next);
    ptr->next = tmp_node;
}

/**
 * finds a visitor by its id and returns a ptr to it
 * @param sys - ptr to the system
 * @param visitor - the ptr that needs to be updated
 * @param visitor_id - the id of the wanted visitor
 * @return NOT_IN_ROOM: if the visitor is not in a room or visitor_id is not
 *         found in the system
 *         OK: if everything went well
 */
static Visitor *find_visitor_by_id(ChallengeRoomSystem *sys, int visitor_id) {
    assert(sys != NULL);
    VisitorsList ptr = sys->visitors_list_head->next;
    while (ptr != NULL && ptr->visitor->visitor_id != visitor_id) {
        ptr = ptr->next;
    }
    if (ptr == NULL) {
        return NULL;
    } else {
        return ptr->visitor;
    }
}