#include <stdio.h>
#include <stdlib.h>

#define RED 'R'
#define BLACK 'B'

typedef struct process {
    unsigned int left_time; // how much time it needs to run more
    unsigned int start_time; // when it appeared in the system
    unsigned int waiting_time; // how much time it spent in the waiting queue
    unsigned int response_time; // how much time it needed to be runned for the first time
    bool runned; 
    int preempted; //how many times it was preempted
    int priority; 
} process;

/* Global clock */
typedef struct gclock{
    unsigned int ticks;
} gclock;

/* Red-black tree properties:
    1) Every node is either red or black.

    2) Every leaf (NIL) is black.

    3) If a node is red, then both its children are black.

    4) Every simple path from a node to a descendant leaf contains the same number of black nodes.
*/    

typedef struct RBNode{
    unsigned int vruntime;
    char color;
    struct RBNode *parent;
    struct RBNode *left;
    struct RBNode *right;
    process *process;
} RBNode;   

RBNode nil_leaf;

/* create a new node and return it */
RBNode* newRBNode(process* p, unsigned int vruntime){
    RBNode *temp   = (RBNode*) malloc(sizeof(RBNode));
    temp->vruntime = vruntime;
    temp->process = p;

    /* the colour of the new node must always be red */
    temp->color  = RED;
    temp->left   = NULL;
    temp->right  = NULL;
    temp->parent = NULL;
    return temp;
}

void rotate_left( RBNode** T, RBNode* x) {
    RBNode *y  = x->right;    
    x->right = y->left;     
    if (y->left != &nil_leaf)
        y->left->parent = x;
    y->parent = x->parent; 
    if (x->parent == &nil_leaf)
        *T = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left   = x;            
    x->parent = y;
}

void rotate_right(RBNode** T, RBNode* y) {
    RBNode *x  = y->left;     
    y->left  = x->right;    
    if (x->right != &nil_leaf)
        x->right->parent = y;
    x->parent = y->parent;  
    if (y->parent == &nil_leaf)
        *T = x;
    else if (y == y->parent->right)
        y->parent->right = x;
    else
        y->parent->left  = x;
    x->right  = y;        
    y->parent = x;
}

void fix_insert(RBNode** root, RBNode* New) {
    RBNode* temp;
    while (New->parent->color == RED)
    {
        if (New->parent == New->parent->parent->left)
        {
            temp = New->parent->parent->right;
            if (temp->color == RED)
            {
                New->parent->color = BLACK;
                temp->color = BLACK;
                New->parent->parent->color = RED;
                New = New->parent->parent;
            }
            else {
                if (New == New->parent->right)
                {
                    New = New->parent;
                    rotate_left(root, New);
                }
                New->parent->color = BLACK;
                New->parent->parent->color = RED;
                rotate_right(root, New->parent->parent);
            }
        }
        else
        {
            temp = New->parent->parent->left;
    if (temp->color == RED)
    {
        New->parent->color = BLACK;
        temp->color = BLACK;
        New->parent->parent->color = RED;
        New = New->parent->parent;
    }
            else {
                if (New == New->parent->left)
                {
                    New = New->parent;
                    rotate_right(root, New);
                }
                New->parent->color = BLACK;
                New->parent->parent->color = RED;
                rotate_left(root, New->parent->parent);
            }
        }
    }
    root[0]->color = BLACK;
}

/* get the leftmost node */
RBNode* left(RBNode* T) {
    RBNode* temp = T;

    while(temp->left != &nil_leaf){
        temp = temp->left;
    } 

    return temp;
} 

void rbTransplant(RBNode **root, RBNode* u, RBNode* v){
    if (u->parent == &nil_leaf  ) {
        *root = v;
    } else if (u == u->parent->left){
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    v->parent = u->parent;
}
RBNode* RB_insert(RBNode** T, process* p, unsigned int vruntime) {
    RBNode* z =  newRBNode(p, vruntime);
    RBNode* y =  &nil_leaf;
    RBNode* x = *T;
    // printf("%d\n", p->start_time );
    // Find where to Insert new RBNode Z into the binary search tree
    while (x != &nil_leaf) {
        y = x;
        if (z->vruntime < x->vruntime)
            x = x->left;
        else
            x = x->right;
    }

    z->parent = y;
    if (y == &nil_leaf)
        *T = z;
    else if (z->vruntime < y->vruntime)
        y->left  = z;
    else
        y->right = z;

    // Init z as a red leaf
    z->left  = &nil_leaf;
    z->right = &nil_leaf;
    z->color = RED;

    if(z->vruntime == 0 && z->parent != &nil_leaf && z->parent != NULL) {
        z->vruntime = (z->parent->vruntime)/2;
    }

    // Ensure the Red-Black property is maintained
    fix_insert(T, z);

    return z;
}



void fix_delete(RBNode* x, RBNode** root) {
    RBNode* s;
    while (x != *root && x->color == BLACK) {
        if (x == x->parent->left) {
            s = x->parent->right;
            if (s->color == RED) {
                // case 3.1
                s->color = BLACK;
                x->parent->color = RED;
                rotate_left(root, x->parent);
                s = x->parent->right;
            }

            if (s->left->color == BLACK && s->right->color == BLACK) {
                // case 3.2
                s->color = RED;
                x = x->parent;
            } else {
                if (s->right->color == BLACK) {
                    // case 3.3
                    s->left->color = BLACK;
                    s->color = RED;
                    rotate_right(root, s);
                    s = x->parent->right;
                } 

                // case 3.4
                s->color = x->parent->color;
                x->parent->color = BLACK;
                s->right->color = BLACK;
                rotate_left(root, x->parent);
                x = *root;
            }
        } else {
            s = x->parent->left;
            if (s->color == RED) {
                // case 3.1
                s->color = BLACK;
                x->parent->color = RED;
                rotate_right(root, x->parent);
                s = x->parent->left;
            }

            if (s->right->color == BLACK && s->right->color == BLACK) {
                // case 3.2
                s->color = RED;
                x = x->parent;
            } else {
                if (s->left->color == BLACK) {
                    // case 3.3
                    s->right->color = BLACK;
                    s->color = RED;
                    rotate_left(root, s);
                    s = x->parent->left;
                } 

                // case 3.4
                s->color = x->parent->color;
                x->parent->color = BLACK;
                s->left->color = BLACK;
                rotate_right(root, x->parent);
                x = *root;
            }
        } 
    }
    x->color = BLACK;
}


RBNode* delete(RBNode** node) {
    // find the node containing key
    RBNode* z = left(*node);
    RBNode* x, *y;
    RBNode* ans = z;
    y = z;
    char y_original_color = y->color;
    if (z->left == &nil_leaf) {
        // if(z->right == &nil_leaf || z->right == NULL){
        //     z = &nil_leaf;
        //     return ans;
        // }
        x = z->right;
        rbTransplant(node, z, z->right);
    }  
    // free(z);
    if (y_original_color == BLACK){
        fix_delete(x, node);
    }
    return ans;
}


int compare(const void *s1, const void *s2){
    process *p1 = (process *)s1;
    process *p2 = (process *)s2;
    if (p1->start_time == p2->start_time)
        return p1->priority < p2->priority;
    return p1->start_time > p2->start_time;
}

