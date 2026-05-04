#include "../include/nfa.h"


void nfa_free(NFA* nfa)
{
    bool* visited = calloc(nfa->size, sizeof(bool));
    Node_Queue* q = Node_Queue_init(nfa->size);
    
    Node_Queue_append(q, nfa->initial);
    int queue_ptr = 0;
    while (queue_ptr < q->size)
    {
        Node* curr = q->data[queue_ptr++];
        for (int i = 0; i < curr->edges->size; ++i)
        {
            int neigh_id = curr->edges->data[i]->neigh->id;
            if (!visited[neigh_id])
            {
                visited[neigh_id] = true;
                Node_Queue_append(q, curr->edges->data[i]->neigh);
            }

            // verbosity -- to help with knowing if the graph is correct
            printf("Edge from %d to %d with spelling %s\n", curr->id, neigh_id, curr->edges->data[i]->spelling);
            free(curr->edges->data[i]->spelling);
            free(curr->edges->data[i]);
        }

        Edges_free(curr->edges);
    }

    free(visited);

    // freeing the nodes
    for (int i = 0; i < q->size; ++i)
    {
        free(q->data[i]);
    }

    Node_Queue_free(q);

    free(nfa);
}