/*--------------------------------------------------------------------*/
/* d_nodeDT.h                                                         */
/* Author: Judah Guggenheim, code borrowed from Christopher Moretti   */
/*--------------------------------------------------------------------*/

/* NOTE: This was originally set up to be the node implementation only
for nodes represanting directories. I (think I) am in the process of changing that now
to be one node that does both directories and files. */
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "d_nodeFT.h"
#include "dynarray.h"

/* A node representing a directory in a File Tree */
struct node
{
    /* the path to this directory */
    Path_T oPPath; /* or should it  be: Path_T *opPath;   ?? */

    /* pointer to the parent (directory node) of this node */
    Node_T oNParent;

    /* a pointer to the dynarray containing this node's children.
    NULL if the node is a file. */
    DynArray_T oDChildren;

    /* tells if the node is a directory or a file */
    boolean isDirectory;

    /* contains the node's contents if a file, NULL if a directory */
    void *contents;

    /* length of the contentss in the file, 0 if a directory */
    size_t lenContents;
};

/*
  Links new child oNChild into oNParent's children array at index
  ulIndex. Returns SUCCESS if the new child was added successfully,
  or  MEMORY_ERROR if allocation fails adding oNChild to the array.*/

static int Node_addChild(Node_T oNParent, Node_T oNChild,
                         size_t ulIndex)
{
    assert(oNParent != NULL);
    assert(oNChild != NULL);

    if (DynArray_addAt(oNParent->oDChildren, ulIndex, oNChild))
        return SUCCESS;
    else
        return MEMORY_ERROR;
}

/*
  Compares the string representation of oNfirst with a string
  pcSecond representing a node's path.
  Returns <0, 0, or >0 if oNFirst is "less than", "equal to", or
  "greater than" pcSecond, respectively.
*/
static int Node_compareString(const Node_T oNFirst,
                              const char *pcSecond)
{
    assert(oNFirst != NULL);
    assert(pcSecond != NULL);

    return Path_compareString(oNFirst->oPPath, pcSecond);
}

int Node_new(Path_T oPPath, Node_T oNParent, Node_T *poNResult, boolean dir,
             void *conts, size_t sizeConts)
{
    struct node *psNew;
    Path_T oPParentPath = NULL;
    Path_T oPNewPath = NULL;
    size_t ulParentDepth;
    size_t ulIndex;
    int iStatus;

    assert(oPPath != NULL);
    /* assert(oNParent == NULL || CheckerDT_Node_isValid(oNParent)); */

    /* allocate space for a new node */
    psNew = malloc(sizeof(struct node));
    if (psNew == NULL)
    {
        *poNResult = NULL;
        return MEMORY_ERROR;
    }

    /* set the new node's path */
    iStatus = Path_dup(oPPath, &oPNewPath);
    if (iStatus != SUCCESS)
    {
        free(psNew);
        *poNResult = NULL;
        return iStatus;
    }
    psNew->oPPath = oPNewPath;

    /* validate and set the new node's parent */
    if (oNParent != NULL)
    {
        size_t ulSharedDepth;

        oPParentPath = oNParent->oPPath;
        ulParentDepth = Path_getDepth(oPParentPath);
        ulSharedDepth = Path_getSharedPrefixDepth(psNew->oPPath,
                                                  oPParentPath);
        /* parent must be an ancestor of child and cannot be a file */
        if (ulSharedDepth < ulParentDepth ||
            oNParent->isDirectory == FALSE)
        {
            Path_free(psNew->oPPath);
            free(psNew);
            *poNResult = NULL;
            return CONFLICTING_PATH;
        }

        /* parent must be exactly one level up from child */
        if (Path_getDepth(psNew->oPPath) != ulParentDepth + 1)
        {
            Path_free(psNew->oPPath);
            free(psNew);
            *poNResult = NULL;
            return NO_SUCH_PATH;
        }

        /* parent must not already have child with this path */
        if (Node_hasChild(oNParent, oPPath, &ulIndex))
        {
            Path_free(psNew->oPPath);
            free(psNew);
            *poNResult = NULL;
            return ALREADY_IN_TREE;
        }
    }
    else
    {
        /* new node must be root */
        /* can only create one "level" at a time */
        if (Path_getDepth(psNew->oPPath) != 1)
        {
            Path_free(psNew->oPPath);
            free(psNew);
            *poNResult = NULL;
            return NO_SUCH_PATH;
        }
    }
    psNew->oNParent = oNParent;

    /* initialize the new node */
    if (dir == TRUE)
    {
        psNew->oDChildren = DynArray_new(0);
        if (psNew->oDChildren == NULL)
        {
            Path_free(psNew->oPPath);
            free(psNew);
            *poNResult = NULL;
            return MEMORY_ERROR;
        }
    }
    else
    {
        psNew->oDChildren = NULL;
    }
    /* add contents if it is a file.
    if it is a directory, dir will be FALSE, conts will
    be NULL, and sizeConts will be 0. */
    psNew->isDirectory = dir;
    psNew->contents = conts;
    psNew->lenContents = sizeConts;

    /* Link into parent's children list */
    if (oNParent != NULL)
    {
        iStatus = Node_addChild(oNParent, psNew, ulIndex);
        if (iStatus != SUCCESS)
        {
            Path_free(psNew->oPPath);
            free(psNew);
            *poNResult = NULL;
            return iStatus;
        }
    }

    *poNResult = psNew;

    /* assert(oNParent == NULL || CheckerDT_Node_isValid(oNParent)); */
    /* assert(CheckerDT_Node_isValid(*poNResult)); */

    return SUCCESS;
}
/* think i'm done */

/*
  Destroys and frees all memory allocated for the subtree rooted at
  oNNode, i.e., deletes this node and all its descendents. Returns the
  number of nodes deleted.
*/
size_t Node_free(Node_T oNNode)
{
    size_t ulIndex;
    size_t ulCount = 0;

    assert(oNNode != NULL);
    /* assert(CheckerDT_Node_isValid(oNNode)); */

    /* if onNode is a file, there are no descendents. */
    if (Node_isDirectory(oNNode) == FALSE)
    {
        free(oNNode);
        return 1;
    }

    /* remove from parent's list */
    if (oNNode->oNParent != NULL)
    {
        if (DynArray_bsearch(
                oNNode->oNParent->oDChildren,
                oNNode, &ulIndex,
                (int (*)(const void *, const void *))Node_compare))
            (void)DynArray_removeAt(oNNode->oNParent->oDChildren,
                                    ulIndex);
    }

    /* recursively remove children from directories */
    if (oNNode->isDirectory == TRUE)
    {
        while (DynArray_getLength(oNNode->oDChildren) != 0)
        {
            ulCount += Node_free(DynArray_get(oNNode->oDChildren, 0));
        }
    }
    DynArray_free(oNNode->oDChildren);

    /* remove path */
    Path_free(oNNode->oPPath);

    /* finally, free the struct node */
    free(oNNode);
    ulCount++;
    return ulCount;
}
/* think i'm done */

/* Returns the path object representing oNNode's absolute path. */
Path_T Node_getPath(Node_T oNNode)
{
    assert(oNNode != NULL);
    return oNNode->oPPath;
}
/* think i'm done */

boolean Node_hasChild(Node_T oNParent, Path_T oPPath,
                      size_t *pulChildID)
{
    assert(oNParent != NULL);
    assert(oPPath != NULL);
    assert(pulChildID != NULL);

    if (oNParent->isDirectory == FALSE)
    {
        return FALSE;
    }
    /* *pulChildID is the index into oNParent->oDChildren */
    return DynArray_bsearch(oNParent->oDChildren,
                            (char *)Path_getPathname(oPPath), pulChildID,
                            (int (*)(const void *, const void *))Node_compareString);
}
/* think i'm done  */

/* Returns the number of children that oNParent has. */
size_t Node_getNumChildren(Node_T oNParent)
{
    assert(oNParent != NULL);
    if (oNParent->isDirectory == FALSE)
    {
        return 0;
    }
    return (DynArray_getLength(oNParent->oDChildren));
}
/* think i'm done */

/*
  Returns an int SUCCESS status and sets *poNResult to be the child
  node of oNParent with identifier ulChildID, if one exists.
  Otherwise, sets *poNResult to NULL and returns status:
  * NO_SUCH_PATH if ulChildID is not a valid child for oNParent
*/
int Node_getChild(Node_T oNParent, size_t ulChildID,
                  Node_T *poNResult)
{

    assert(oNParent != NULL);
    assert(poNResult != NULL);

    if (oNParent->isDirectory == FALSE)
    {
        *poNResult = NULL;
        return NO_SUCH_PATH;
    }

    /* ulChildID is the index into oNParent->oDChildren */
    if (ulChildID >= Node_getNumChildren(oNParent))
    {
        *poNResult = NULL;
        return NO_SUCH_PATH;
    }
    else
    {
        *poNResult = DynArray_get(oNParent->oDChildren, ulChildID);
        return SUCCESS;
    }
}
/* think i'm done */

/*
  Returns the parent node of oNNode.
  Returns NULL if oNNode is the root and thus has no parent.
*/

Node_T Node_getParent(Node_T oNNode)
{
    assert(oNNode != NULL);
    return oNNode->oNParent;
}
/* think i'm done */

/*
  Compares oNFirst and oNSecond lexicographically based on their paths.
  Returns <0, 0, or >0 if onFirst is "less than", "equal to", or
  "greater than" oNSecond, respectively.
*/
int Node_compare(Node_T oNFirst, Node_T oNSecond)
{
    assert(oNFirst != NULL);
    assert(oNSecond != NULL);
    return (Path_comparePath(oNFirst->oPPath, oNSecond->oPPath));
}
/* think i'm done */

/*
  Returns a string representation for oNNode, or NULL if
  there is an allocation error.

  Allocates memory for the returned string, which is then owned by
  the caller!
*/

boolean Node_isDirectory(Node_T oNNode)
{
    assert(oNNode != NULL);
    return oNNode->isDirectory;
}

void *Node_getContents(Node_T oNNode)
{
    assert(oNNode != NULL);
    return oNNode->contents;
}

size_t Node_getSizeContents(Node_T oNNode)
{
    assert(oNNode != NULL);
    return oNNode->lenContents;
}

int Node_setContents(Node_T oNNode, void *pvNewContents, size_t newLenContents)
{
    assert(oNNode != NULL);

    if (Node_isDirectory(oNNode) == TRUE)
    {
        return NOT_A_FILE;
    }
    oNNode->contents = pvNewContents;
    oNNode->lenContents = newLenContents;
    return SUCCESS;
}

char *Node_toString(Node_T oNNode)
{
    char *copyPath;

    assert(oNNode != NULL);
    copyPath = malloc(Path_getStrLength(Node_getPath(oNNode)) + 1);
    if (copyPath == NULL)
        return NULL;
    else
        return strcpy(copyPath, Path_getPathname(Node_getPath(oNNode)));
}
