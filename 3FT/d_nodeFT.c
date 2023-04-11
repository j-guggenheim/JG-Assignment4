/*--------------------------------------------------------------------*/
/* d_nodeDT.h                                                         */
/* Author: Judah Guggenheim, code borrowed from Christopher Moretti   */
/*--------------------------------------------------------------------*/

/* NOTE: This was originally set up to be the node implementation only 
for nodes represanting directories. I (think I) am in the process of changing that now
to be one node that does both directories and files. */
#include "d_nodeFT.h"
#include "dynarray.h"

/* A node representing a directory in a File Tree */
struct d_node{
    /* the path to this directory */
    Path_T oPPath; /* or should it  be: Path_T *opPath;   ?? */

    /* pointer to the parent (directory node) of this node */
    dNode_T oNParent;

    /* a pointer to the dynarray containing this node's directory children */
    DynArray_T oDChildren;

    /* a pointer to the dynarray containing this node's file children */
    DynArray_T oFChildren;
}

/* maybe need an error here if trying to make a node without parents but there's already a root in the tree */
int dNode_new(Path_T oPPath, dNode_T oNParent, dNode_T *poNResult){
    size_t depth = Path_getDepth(oPPath);
    Path_T parentPath;
    size_t *uIndex;

    if(depth == 0) {
        *poNResult = NULL;
        fprintf(stderr, "can't put a node at an empty path");
        return NO_SUCH_PATH;
    }
    if(oNParent == NULL && depth != 1){
        *poNResult = NULL;
        fprintf(stderr, "can't put a (non-root) node at a NULL parent");
        return NO_SUCH_PATH;
    }
    

    /*Not sure about this one*/
    if(oNParent!=NULL){
    Path_prefix(oPPath, (depth-1), parentPath);
    if(Path_comparePath(oNParent->oPPath, parentPath)!=0){
        *poNResult = NULL;
        fprintf(stderr, "oNParent's path is not oPPath's direct parent");
        return NO_SUCH_PATH;
    }
    }

    (*poNResult) = (dNode_T) malloc(sizeof(struct d_node));
    if(*poNResult == NULL){
        fprintf(stderr, "not enough memory to add this node");
        return MEMORY_ERROR;
    }

    if(oNParent!=NULL){
    if(DynArray_bsearch(oNParent->oDChildren, (*poNResult), uIndex, dNode_compare)==1){
        fprintf(stderr, "this node already exists");
        return ALREADY_IN_TREE;
    }

    if(DynArray_addAt(oNParent->oDChildren, uIndex, (*poNResult)) == 0){
        fprintf(stderr, "not enough memory to add this node");
        *poNResult = NULL;
        return MEMORY_ERROR;
    }
    }
    (*poNResult)->oPPath = oPPath;
    (*poNResult)->oNParent = oNParent;
    (*poNResult)->oDChildren = DynArray_new(0);
    return SUCCESS;
}
/* think i'm done */

/*
  Destroys and frees all memory allocated for the subtree rooted at
  oNNode, i.e., deletes this node and all its descendents. Returns the
  number of nodes deleted.
*/
size_t dNode_free(dNode_T oNNode){
    size_t deleteCount = 0;
    while(DynArray_getLength(oNNode->oDChildren)!=0){

    }
    free(oNNode->oDChildren);
    free(oNNode->oNParent);
    free(oNNode->oPPath);
    free(oNNode);
}

/* Returns the path object representing oNNode's absolute path. */
Path_T dNode_getPath(dNode_T oNNode){
    return oNNode->oPPath;
}
/* think i'm done */


/*
  Returns TRUE if oNParent has a child with path oPPath. Returns
  FALSE if it does not.

  If oNParent has such a child, stores in *pulChildID the child's
  identifier (as used in Node_getChild). If oNParent does not have
  such a child, stores in *pulChildID the identifier that such a
  child _would_ have if inserted.

  [Judah's notes] I don't think this is necessary. Later: ok wait yeah it is. 
*/
boolean dNode_hasChild(dNode_T oNParent, Path_T oPPath,
                         size_t *pulChildID){
                            /* This is an issue bc it takes memory */
                            dNode_T temp;
                            boolean bool;
                            dNode_new(oPPath, oNParent, temp);
                            if(DynArray_bsearch(oNParent->oDChildren, temp, pulChildID, dNode_compare())==0){bool = FALSE;}
                            else {bool = TRUE;}
                            dNode_free(temp);
                            return bool;
                         };
/* think i'm done  - needs much improvement */

/* Returns the number of children that oNParent has. */
size_t dNode_getNumChildren(dNode_T oNParent){
    return(DynArray_getLength(oNParent->oDChildren));
}
/* think i'm done */


/*
  Returns an int SUCCESS status and sets *poNResult to be the child
  node of oNParent with identifier ulChildID, if one exists.
  Otherwise, sets *poNResult to NULL and returns status:
  * NO_SUCH_PATH if ulChildID is not a valid child for oNParent
*/
int dNode_getChild(dNode_T oNParent, size_t ulChildID,
                  dNode_T *poNResult){
                    if(ulChildID>= DynArray_getLength(oNParent->oDChildren)){
                        *poNResult = NULL;
                        return NO_SUCH_PATH;
                    }
                    poNResult = DynArray_get(oNParent->oDChildren, ulChildID);
                    return SUCCESS;
            }
/* think i'm done */


/*
  Returns the parent node of oNNode.
  Returns NULL if oNNode is the root and thus has no parent.
*/
dNode_T dNode_getParent(dNode_T oNNode){
    return(*(oNNode->oNParent));
}
/* think i'm done */

/*
  Compares oNFirst and oNSecond lexicographically based on their paths.
  Returns <0, 0, or >0 if onFirst is "less than", "equal to", or
  "greater than" oNSecond, respectively.
*/
int dNode_compare(dNode_T oNFirst, dNode_T oNSecond){
    return (Path_comparePath(oNFirst->oPPath, oNSecond->oPPath));
}
/* think i'm done */

/*
  Returns a string representation for oNNode, or NULL if
  there is an allocation error.

  Allocates memory for the returned string, which is then owned by
  the caller!
*/
char *dNode_toString(dNode_T oNNode);
