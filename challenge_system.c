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

/*
 * frees the allocated memory of the name of the system
 */
static void free_system_name(ChallengeRoomSystem *sys) {
    free(sys->system_name);
    return;
}

/*
 * frees allocated memory for all the challenges in a specific room
 */
static void free_system_challenges(ChallengeRoomSystem *sys,
                                   int num_challenges) {
    for (int i = 0; i < num_challenges; ++i) {
        free(sys->system_challenges[i]);
        //releasing the allocated memory of each challenge
    }
    free(sys->system_challenges);
    //releasing the allocated memory of the pointers-array
    return;
}

/*
 * frees allocated memory for all system rooms
 */
static void free_system_rooms(ChallengeRoomSystem *sys, int num_rooms) {
    for (int i = 0; i < num_rooms; ++i) {
        free(sys->system_rooms[i]->name);
        free(sys->system_rooms[i]->challenges);
        //releasing the allocated memory of each room
    }
    free(sys->system_rooms);
    //releasing the allocated memory of the pointers-array
    return;
}

/*
 * Reads the name from the file and updates it in the system
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

/*
 * takes the data of the challenges from the file and updates it to the system
 * returns the num of challenges through ptr
 */
static Result create_system_challenges(ChallengeRoomSystem *sys,
                                       FILE *input_file){
    char challenge_name[WORD_MAX_LEN] = "";
    //reads the number of challenges from the file
    fscanf(input_file, "%d\n", &sys->system_num_challenges);
    //allocates memory for the Challenge's pointer array
    sys->system_challenges = malloc(sys->system_num_challenges *
                                    sizeof(*sys->system_challenges));
    if (sys->system_challenges == NULL) {
        free_system_name(sys);
        return MEMORY_PROBLEM;
    }

    for (int i = 0; i < sys->system_num_challenges; ++i) {
        int level = 0, id = 0;
        //reads the challenge params and initialize every challenge to it's
        // pointer in the array
        fscanf(input_file, "%s %d %d\n", challenge_name, &id, &level);
        Result result = init_challenge(sys->system_challenges[i], id,
                                       challenge_name, (Level) level);
        if (result != OK) {
            free_system_name(sys);
            free_system_challenges(sys, sys->system_num_challenges);
            //releasing system name and challenges that have been allocated
            //previously
            return result;
        }
    }
    return OK;
}

/*
 * takes the data of the rooms from the file and updates it to the system
 */
static Result create_system_rooms(ChallengeRoomSystem *sys, FILE *input_file) {
    char room_name[WORD_MAX_LEN] = "";
    //reads the num of rooms from the file
    fscanf(input_file, "%d\n", &sys->system_num_rooms);
    //allocates memory for the room's ptr array
    sys->system_rooms = malloc(sys->system_num_rooms * sizeof
                                                       (*sys->system_rooms));
    if (sys->system_rooms == NULL) {
        free_system_name(sys);
        free_system_challenges(sys, sys->system_num_challenges);
        //releasing system name and challenges that have been allocated
        //previously
        return NULL_PARAMETER;
    }

    //goes through each room
    for (int i = 0; i < sys->system_num_rooms; ++i) {
        int num_challenges_in_room = 0;
        //reads the room's name and num of challenges within the room from file
        fscanf(input_file, "%s %d", room_name, &num_challenges_in_room);
        //initializes the room and the included challenge activities
        Result result = init_room(sys->system_rooms[i], room_name,
                                  num_challenges_in_room);
        if (result != OK) {
            free_system_name(sys);
            free_system_challenges(sys, sys->system_num_challenges);
            free_system_rooms(sys, sys->system_num_rooms);
            //releasing system name, challenges and rooms that have been
            //allocated previously
            return result;
        }

        //goes through all the challenge's ids for each room
        for (int j = 0; j < num_challenges_in_room; ++j) {
            int challenge_id = 0;
            //reads the id from file
            //if that's the last id move cursor to the start of next line
            if (j != num_challenges_in_room - 1) {
                fscanf(input_file, "%d", &challenge_id);
            } else {
                fscanf(input_file, "%d\n", &challenge_id);
            }
            //goes through the challenge array in the system to find the
            //right challenge to add to the room by id
            for (int k = 0; k < sys->system_num_challenges; ++k) {
                if (sys->system_challenges[k]->id == challenge_id) {
                    //initialize the challenge activity for each challenge id
                    result = init_challenge_activity(
                            &(sys->system_rooms[i]->challenges[j]),
                            sys->system_challenges[k]);
                    if (result != OK) {
                        free_system_name(sys);
                        free_system_challenges(sys, sys->system_num_challenges);
                        free_system_rooms(sys, sys->system_num_rooms);
                        //releasing system name, challenges and rooms that have
                        //been allocated previously
                        return result;
                    }
                }
            }
        }
    }
    return OK;
}

static Result create_system_visitor_list_head(ChallengeRoomSystem *sys){
    sys->visitorsListHead = malloc(sizeof((sys->visitorsListHead)));
    if (sys->visitorsListHead == NULL) {
        return MEMORY_PROBLEM;
    }
    sys->visitorsListHead->visitor = NULL;
    sys->visitorsListHead->next = NULL;
    return OK;
}

Result create_system(char *init_file, ChallengeRoomSystem **sys) {
    assert(init_file != NULL || (*sys) != NULL);
    if (init_file == NULL || (*sys) == NULL) {
        return NULL_PARAMETER;
    }
    (*sys)->system_curr_time = 0;
    (*sys)->system_num_rooms = 0;
    (*sys)->system_num_challenges = 0;
    FILE *input = fopen(init_file, "r");
    if (input == NULL) {
        return MEMORY_PROBLEM; //TODO not sure if that's the right Error
    }

    Result result = update_system_name(*sys, input);
    if (result != OK) {
        fclose(input);
        return result;
    }

    result = create_system_challenges(*sys, input);
    if (result != OK) {
        fclose(input);
        return result;
    }

    result = create_system_rooms(*sys, input);
    if (result != OK) {
        fclose(input);
        return result;
    }
    result = create_system_visitor_list_head(*sys);
    if (result != OK) {
        fclose(input);
        return result;
    }

    fclose(input);
    return OK;
}

/*
 *iterates over all the challenges and returns the challenge with the best
 * time of all
 */
static Result find_challenge_best_time (ChallengeRoomSystem *sys,
                                        char **challenge_best_time,
                                        int *time) {
    int best_time = 0;
    int max_i = 0;
    for (int i = 0; i < sys->system_num_challenges; ++i) {
        int tmp_time = 0;
        Result result;
        result = best_time_of_system_challenge(sys,
                                               sys->system_challenges[i]->name,
                                               &tmp_time);
        if (result != OK) {
            return result;
        }
        if (tmp_time > 0 && tmp_time < best_time) {
            best_time = tmp_time;
            max_i = i;
        } else if (tmp_time > 0 && tmp_time == best_time) {
            if (strcmp(sys->system_challenges[i]->name,
                       sys->system_challenges[max_i]->name) < 0) {
                max_i = i;
            }
        }
    }
    *time = best_time;
    *challenge_best_time = realloc(*challenge_best_time,
                                   strlen(sys->system_challenges[max_i]->name));
    if (*challenge_best_time == NULL) {
        return MEMORY_PROBLEM;
    }
    return OK;
}

Result destroy_system(ChallengeRoomSystem *sys, int destroy_time, char
**most_popular_challenge_p, char **challenge_best_time) {
    assert(sys != NULL && *most_popular_challenge_p != NULL &&
           *challenge_best_time != NULL);
    if (sys == NULL || *most_popular_challenge_p == NULL ||
            *challenge_best_time == NULL) {
        return NULL_PARAMETER;
    }
    if (destroy_time < sys->system_curr_time) {
        return ILLEGAL_TIME;
    }
    Result result = all_visitors_quit(sys, destroy_time);
    if (result != OK) {
        return result;
    }
    free(*most_popular_challenge_p);
    result = most_popular_challenge(sys, most_popular_challenge_p);
    if (result != OK) {
        return result;
    }

    //no need to free because we used realloc in the func
    int best_time = 0;
    result = find_challenge_best_time(sys, challenge_best_time, &best_time);
    if (result != OK) {
        return result;
    }

    //if the best time is 0, it means no visitors have done any challenges
    if (best_time == 0) {
        free(*challenge_best_time);
        *challenge_best_time = NULL;
    }
    return OK;
}

/*
 * creates a visitor and initialize it
 * then creates a new node to the visitors list in the system and adds the
 * new visitor to the list
 */
static Result add_visitor_node(ChallengeRoomSystem *sys, char *visitor_name,
                               int visitor_id) {
    assert(sys != NULL && visitor != NULL);
    //create and initialize a new visitor
    Visitor *new_visitor = malloc(sizeof(*new_visitor));
    if (new_visitor == NULL) {
        return MEMORY_PROBLEM;
    }
    Result result = init_visitor(new_visitor, visitor_name, visitor_id);
    if (result != OK) {
        free(new_visitor);
        return result;
    }

    //create a new node to the list
    VisitorsList new_node = malloc(sizeof(new_node));
    if (new_node == NULL) {
        free(new_visitor);
        return MEMORY_PROBLEM;
    }
    //create a temp node to hold the current newest visitor in the list
    VisitorsList tmp_node = sys->visitorsListHead->next;
    sys->visitorsListHead->next = new_node;
    new_node->visitor = new_visitor;
    new_node->next = tmp_node;
    return OK;
}

/*
 * finds a room by it's name
 * returns a ptr to the room
 */
static ChallengeRoom *find_room_by_name(ChallengeRoomSystem *sys,
                                        char *room_name) {
    assert(sys != NULL && room_name != NULL);
    for (int i = 0; i < sys->system_num_rooms; ++i) {
        if (strcmp(sys->system_rooms[i]->name, room_name)) {
            return sys->system_rooms[i];
        }
    }
    //there is no matching room
    return NULL;
}

/*
 * receives details of a visitor, the room he chose, the start time and level
 * he's interested in. checks if there's a suitable challenge in the asked room
 * and updates the system accordingly
 */
Result visitor_arrive(ChallengeRoomSystem *sys, char *room_name, char
*visitor_name, int visitor_id, Level level, int start_time) {
    assert(sys != NULL);
    if (sys == NULL) {
        return NULL_PARAMETER;
    }
    if (start_time < sys->system_curr_time) {
        return ILLEGAL_TIME;
    }
    if (visitor_name == NULL || room_name == NULL) {
        return ILLEGAL_PARAMETER;
    }

    Result result = add_visitor_node(sys, visitor_name, visitor_id);
    if (result != OK) {
        return result;
    }

    ChallengeRoom *room = find_room_by_name(sys, room_name);
    if (room == NULL) {
        return ILLEGAL_PARAMETER;
    }

    result = visitor_enter_room(room, sys->visitorsListHead->next->visitor,
                                level, start_time);
    if (result != OK) {
        return result;
    }
    if (start_time > sys->system_curr_time) {
        sys->system_curr_time = start_time;
    }
    return OK;

}

/*
 * frees the allocated memory of a specific node in the list
 */
static void destroy_visitor_node(VisitorsList ptr, VisitorsList previous_ptr) {
    assert(ptr != NULL);
    VisitorsList tmp_ptr = ptr->next;
    free(ptr->visitor);
    free(ptr);
    previous_ptr->next = tmp_ptr;
}

/*
 * gets a visitor out of the system and frees any allocated memory of the
 * visitor
 */
Result visitor_quit(ChallengeRoomSystem *sys, int visitor_id, int quit_time) {
    assert(sys != NULL);
    if (sys == NULL) {
        return NULL_PARAMETER;
    }
    if (quit_time < sys->system_curr_time) {
        return ILLEGAL_TIME;
    }

    VisitorsList ptr = sys->visitorsListHead;
    VisitorsList previous_ptr = NULL;

    while (ptr->visitor->visitor_id != visitor_id) {
        previous_ptr = ptr;
        ptr = ptr->next;
    }

    Result result = visitor_quit_room(ptr->visitor, quit_time);
    if (result != OK) {
        return result;
    }

    destroy_visitor_node(ptr, previous_ptr);
    if (quit_time > sys->system_curr_time) {
        sys->system_curr_time = quit_time;
    }
    return OK;
}

/*
 * goes through all the visitor list and does quit to each one of the
 * visitors and frees any allocated memory of the list
 */
Result all_visitors_quit(ChallengeRoomSystem *sys, int quit_time) {
    assert(sys != NULL);
    if (sys == NULL) {
        return NULL_PARAMETER;
    }
    if (quit_time < sys->system_curr_time) {
        return ILLEGAL_TIME;
    }

    //frees any previously allocated memory of the visitors list from the system
    VisitorsList ptr = sys->visitorsListHead;
    while (ptr != NULL) {
        VisitorsList to_delete = ptr;
        ptr = ptr->next;
        Result result = visitor_quit(sys, to_delete->visitor->visitor_id,
                                     quit_time);
        if (result != OK) {
            return result;
        }
        free(to_delete);
    }
    if (quit_time > sys->system_curr_time) {
        sys->system_curr_time = quit_time;
    }
    return OK;
}


Result system_room_of_visitor(ChallengeRoomSystem *sys, char *visitor_name,
                              char **room_name) {
    assert(sys != NULL);
    if (sys == NULL) {
        return NULL_PARAMETER;
    }
    if (visitor_name == NULL) {
        return ILLEGAL_PARAMETER;
    }

    VisitorsList ptr = sys->visitorsListHead->next;
    while (ptr != NULL) {
        if (strcmp(ptr->visitor->visitor_name, visitor_name) == 0) {
            if (ptr->visitor->room_name != NULL){
                *room_name = malloc(strlen(*(ptr->visitor->room_name)) + 1);
                if (*room_name == NULL) {
                    return MEMORY_PROBLEM;
                }
                strcpy(*room_name, *(ptr->visitor->room_name));
                return OK;
            }
            return NOT_IN_ROOM;
        }
        ptr = ptr->next;
    }
    //if we got here it means that the visitor is not in the system
    return ILLEGAL_PARAMETER;
}

/*
 * gets a challenge id, and changes the name of the suitable challenge to
 * a new name received as an input
 */
Result change_challenge_name(ChallengeRoomSystem *sys, int challenge_id,
                             char *new_name) {
    if (sys == NULL || new_name == NULL) {
        return NULL_PARAMETER;
    }
    //iterating over the challenges in the system
    for (int i = 0; i < sys->system_num_challenges; ++i) {
        if (sys->system_challenges[i]->id == challenge_id) {
            Result result = change_name(sys->system_challenges[i], new_name);
            if (result != OK) {
                return result;
            }
            return OK;
        }
    }

    //if we got here, it means that challenge_id wasn't found in the system
    return ILLEGAL_PARAMETER;
}

/*
 * receives a system and changes it's name to a new name receives as an input
 */
Result change_system_room_name(ChallengeRoomSystem *sys, char *current_name,
                               char *new_name) {
    if (sys == NULL || current_name == NULL || new_name == NULL) {
        return NULL_PARAMETER;
    }
    //iterating over all the rooms
    for (int i = 0; i < sys->system_num_rooms; ++i) {

        //checking if we got to the suitable room
        if (strcmp(sys->system_rooms[i]->name, current_name) == 0) {
            Result result = change_room_name(sys->system_rooms[i], new_name);
            if (result != OK) {
                return result;
            }
            return OK;
        }
    }

    //if we got here, it means that current_name wasn't found in the system
    return ILLEGAL_PARAMETER;
}

/*
 * receives a challenge name, searches the challenge and returns the best time
 * of the challenge through ptr
 */
Result best_time_of_system_challenge(ChallengeRoomSystem *sys,
                                     char *challenge_name, int *time) {
    if (sys == NULL || challenge_name == NULL || time == NULL) {
        return NULL_PARAMETER;
    }
    for (int i = 0; i < sys->system_num_challenges; ++i) {
        if (strcmp(sys->system_challenges[i]->name, challenge_name) == 0) {
            Result result = best_time_of_challenge(sys->system_challenges[i],
                                                   time);
            if (result != OK) {
                return result;
            }
            return OK;
        }
    }

    //if we got here, it means that challenge_name wasn't found in the system
    return ILLEGAL_PARAMETER;
}

/*
 * iterates over all the challenges in the system and returns
 * the most popular challenge in case there are a few most popular, the
 * function will return the lexicographically smallest one
 */
Result most_popular_challenge(ChallengeRoomSystem *sys, char **challenge_name) {
    if (sys == NULL ||
        *challenge_name == NULL) { //TODO challenge_name or *challenge_name?
        return NULL_PARAMETER;
    }
    int max_idx = 0;

    //iterating over all the challenges in the system and finds the one
    //with highest num of visits starting from 1 because we set by default
    // the initial minimum to the first challenge
    for (int i = 1; i < sys->system_num_challenges; ++i) {

        //if the num of visits of the current challenge is higher,
        //it will be the current maximum
        if (sys->system_challenges[i]->num_visits >
            sys->system_challenges[max_idx]->num_visits) {
            max_idx = i;

            //if the num of visits of the current challenge is equal,
            //we will take the lexicographically smaller
        } else if (sys->system_challenges[i]->num_visits ==
                   sys->system_challenges[max_idx]->num_visits) {
            if (strcmp(sys->system_challenges[i]->name,
                       sys->system_challenges[max_idx]->name) < 0) {
                max_idx = i;
            }
        }
    }

    //creating a separate copy of the challenge name
    *challenge_name = malloc(strlen(sys->system_challenges[max_idx]->name) + 1);
    if (*challenge_name == NULL) {
        return MEMORY_PROBLEM;
    }
    strcpy(*challenge_name, sys->system_challenges[max_idx]->name);
    return OK;
}