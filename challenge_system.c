//
// Created by adire on 02-May-17.
//

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>


#include "challenge_system.h"

/*
 * a linked list of visitors
 */
struct SVisitorsList
{
    Visitor *visitor;
    struct SVisitorsList *next;
} *VisitorsList;

#define BUFFER_SIZE 51


Result create_system(char *init_file, ChallengeRoomSystem **sys) {
    if (init_file == NULL || (*sys) == NULL) {
        return NULL_PARAMETER;
    }
    FILE *input = fopen(init_file, "r");
    if (input == NULL) {
        return MEMORY_PROBLEM; //TODO not sure if that's the right Error
    }

    int num_challenges = 0;
    char buffer[BUFFER_SIZE] = "";
    fscanf(input, "%s %d", buffer, &num_challenges);
    (*sys)->system_name = malloc(strlen(buffer) + 1);
    if ((*sys)->system_name == NULL) {
        return NULL_PARAMETER;
    }
    strcpy((*sys)->system_name, buffer);

    (*sys)->system_challenges = malloc(num_challenges * sizeof(void *));
    for (int i = 0; i < num_challenges; ++i) {
        int level = 0, id = 0;
        fscanf(input, "%s %d %d", buffer, &id, &level);
        init_challenge(&(*sys)->system_challenges[i], id, buffer, level);
    }

    int num_rooms = 0;
    fscanf(input, "%d", &num_rooms);
    (*sys)->system_rooms = malloc(num_rooms * sizeof(void *));
    if ((*sys)->system_rooms == NULL) {
        return NULL_PARAMETER;
    }
    for (int i = 0; i < num_rooms; ++i) {
        int num_challenges_in_room = 0;
        fscanf(input, "%s %d", buffer, &num_challenges_in_room);
        init_room(&(*sys)->system_rooms[i], buffer, num_challenges_in_room);
        ChallengeActivity **challengeActivity = malloc(num_challenges_in_room
                                                       * sizeof(void*));
        for (int j = 0; j < num_challenges_in_room; ++j) {
            int challenge_id = 0;
            fscanf(input, "%d", &challenge_id);
            for (int k = 0; k < num_challenges; ++k) {
                if((*sys)->system_challenges[k].id == challenge_id){
                    init_challenge_activity(challengeActivity[j], &(*sys)
                            ->system_challenges[k]);
                }
            }
        }
        free(challengeActivity);
    }


    fclose(input);
    return OK;
}

Result destroy_system(ChallengeRoomSystem *sys, int destroy_time, char
**most_popular_challenge_p, char **challenge_best_time);


Result visitor_arrive(ChallengeRoomSystem *sys, char *room_name, char
*visitor_name, int visitor_id, Level level, int start_time);

Result visitor_quit(ChallengeRoomSystem *sys, int visitor_id, int quit_time);


Result all_visitors_quit(ChallengeRoomSystem *sys, int quit_time);


Result system_room_of_visitor(ChallengeRoomSystem *sys, char *visitor_name,
                              char **room_name);


Result change_challenge_name(ChallengeRoomSystem *sys, int challenge_id,
                             char *new_name);


Result change_system_room_name(ChallengeRoomSystem *sys, char *current_name,
                               char *new_name);


Result best_time_of_system_challenge(ChallengeRoomSystem *sys,
                                     char *challenge_name, int *time);


Result most_popular_challenge(ChallengeRoomSystem *sys, char **challenge_name);