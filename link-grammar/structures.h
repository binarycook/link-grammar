/*************************************************************************/
/* Copyright (c) 2004                                                    */
/* Daniel Sleator, David Temperley, and John Lafferty                    */
/* All rights reserved                                                   */
/*                                                                       */
/* Use of the link grammar parsing system is subject to the terms of the */
/* license set forth in the LICENSE file included with this software,    */
/* and also available at http://www.link.cs.cmu.edu/link/license.html    */
/* This license allows free redistribution and use in source and binary  */
/* forms, with or without modification, subject to certain conditions.   */
/*                                                                       */
/*************************************************************************/

#ifndef _STRUCTURES_H_
#define _STRUCTURES_H_

#include "api-types.h"
#include "api-structures.h"
#include "dict-structures.h"  /* For Exp, Exp_list */
#include "utilities.h"  /* Needed for inline defn in Windows */

/*
 Global variable descriptions
  -- Most of these global variables have been eliminated.
     I've left this comment here for historical purposes --DS 4/98

 N_words:
    The number of words in the current sentence.  Computed by
    separate_sentence().

 N_links:
    The number of links in the current linkage.  Computed by
    extract_linkage().

 sentence[].string:
    Contains a slightly modified form of the words typed by the user.
    Computed by separate_sentence().

 sentence[].x:
    Contains, for each word, a pointer to a list of expressions from the
    dictionary that match the word in sentence[].string.
    Computed by build_sentence_expressions().

 sentence[].d
    Contains for each word, a pointer to a list of disjuncts for this word.
    Computed by: parepare_to_parse(), but modified by pruning and power
    pruning.

 link_array[]
    This is an array of links.  These links define the current linkage.
    It is computed by extract_links().  It is used by analyze_linkage() to
    compute pp_linkage[].  It may contain fat links.

 pp_link_array[]   ** eliminated (ALB)
    Another array of links.  Here all fat links have been expunged.
    It is computed by analyze_linkage(), and used by post_process() and by
    print_links();

 chosen_disjuncts[]
    This is an array pointers to disjuncts, one for each word, that is
    computed by extract_links().  It represents the chosen disjuncts for the
    current linkage.  It is used to compute the cost of the linkage, and
    also by compute_chosen_words() to compute the chosen_words[].

 chosen_words[]
    An array of pointers to strings.  These are the words to be displayed
    when printing the solution, the links, etc.  Computed as a function of
    chosen_disjuncts[] by compute_chosen_words().  This differs from
    sentence[].string because it contains the suffixes.  It differs from
    chosen_disjunct[].string in that the idiom symbols have been removed.

#ifdef USE_FAT_LINKAGES
 has_fat_down[]
    An array of chars, one for each word.  TRUE if there is a fat link
    down from this word, FALSE otherwise.  (Only set if there is at least
    one fat link.)  Set by set_has_fat_down_array() and used by
    analyze_linkage() and is_canonical().

 is_conjunction[]
    An array of chars, one for each word.  TRUE if the word is a conjunction
    ("and", "or", "nor", or "but" at the moment).  False otherwise. 
#endif USE_FAT_LINKAGES 
*/


#define NEGATIVECOST -1000000
/* This is a hack that allows one to discard disjuncts containing
   connectors whose cost is greater than given a bound. This number plus
   the cost of any connectors on a disjunct must remain negative, and
   this number multiplied times the number of costly connectors on any
   disjunct must fit into an integer. */

/* Upper bound on the cost of any connector. */
#define MAX_CONNECTOR_COST 1000.0f

/* The following define the names of the special strings in the dictionary. */
#define LEFT_WALL_WORD   ("LEFT-WALL")
#define RIGHT_WALL_WORD  ("RIGHT-WALL")

/* Word subscripts come after the subscript mark (ASCII ETX) */
/* #define SUBSCRIPT_MARK '.' */
#define SUBSCRIPT_MARK 0x3
/* #define EMPTY_WORD  "=.zzz" */
#define EMPTY_WORD  "=zzz"

#ifdef USE_FAT_LINKAGES
  #define ANDABLE_CONNECTORS_WORD ("ANDABLE-CONNECTORS")

  /* conditional compiling flags */
  #define PLURALIZATION
      /* If defined, Turns on the pluralization operation in        */
      /* "and", "or" and "nor" */

  #define NORMAL_LABEL  (-1) /* used for normal connectors            */
                             /* the labels >= 0 are used by fat links */
#endif /* USE_FAT_LINKAGES */

#define UNLIMITED_CONNECTORS_WORD ("UNLIMITED-CONNECTORS")

#define UNKNOWN_WORD     ("UNKNOWN-WORD")

#define MAX_PATH_NAME 200     /* file names (including paths)
                                 should not be longer than this */

/*      Some size definitions.  Reduce these for small machines */
/* MAX_WORD is large, because unicode entries can use a lot of space */
#define MAX_WORD 180          /* maximum number of bytes in a word */
#define MAX_LINE 2500         /* maximum number of chars in a sentence */

/* conditional compiling flags */
#define INFIX_NOTATION
      /* If defined, then we're using infix notation for the dictionary */
      /* otherwise we're using prefix notation */

#define UNLIMITED_LEN 255
#define SHORT_LEN 6
#define NO_WORD 255

#ifndef _MSC_VER
typedef long long s64; /* signed 64-bit integer, even on 32-bit cpus */
#define PARSE_NUM_OVERFLOW (1LL<<24)  
#else
/* Microsoft Visual C Version 6 doesn't support long long. */
typedef signed __int64 s64; /* signed 64-bit integer, even on 32-bit cpus */
#define PARSE_NUM_OVERFLOW (((s64)1)<<24)  
#endif

#ifdef USE_FAT_LINKAGES
typedef enum
{
	THIN_priority,
	UP_priority,
	DOWN_priority
} Priority;
#endif /* USE_FAT_LINKAGES */

struct Connector_struct
{
    int hash;
    unsigned char word;
                   /* The nearest word to my left (or right) that
                      this could connect to.  Computed by power pruning */
    unsigned char length_limit;
                  /* If this is a length limited connector, this
                     gives the limit of the length of the link
                     that can be used on this connector.  Since
                     this is strictly a funcion of the connector
                     name, efficiency is the only reason to store
                     this.  If no limit, the value is set to 255. */
    char multi;   /* TRUE if this is a multi-connector */
#ifdef USE_FAT_LINKAGES
    Priority priority;/* one of the three priorities above */
    short label;
#endif /* USE_FAT_LINKAGES */
    Connector * next;
    const char * string;

    /* Hash table next pointer, used only during pruning. */
    Connector * tableNext;
};

static inline void connector_set_string(Connector *c, const char *s)
{
	c->string = s;
	c->hash = -1;
}
static inline const char * connector_get_string(Connector *c)
{
	return c->string;
}

struct Disjunct_struct
{
    Disjunct *next;
    const char * string;
    Connector *left, *right;
    float cost;
    char marked;
};

typedef struct Match_node_struct Match_node;
struct Match_node_struct
{
    Match_node * next;
    Disjunct * d;
};

typedef struct X_node_struct X_node;
struct X_node_struct
{
    const char * string;  /* the word itself */
    Exp * exp;
    X_node *next;
};

struct Word_struct
{
    const char *unsplit_word;
    const char **alternatives;
    X_node * x;      /* sentence starts out with these */
    Disjunct * d;    /* eventually these get generated */
    Boolean firstupper;
};


/* The structure below stores a list of dictionary word files. */
struct Word_file_struct
{
    char file[MAX_PATH_NAME+1];   /* the file name */
    int changed;             /* TRUE if this file has been changed */
    Word_file * next;
};

/* The regexs are stored as a linked list of the following nodes. */
struct Regex_node_s
{
    char *name;      /* The identifying name of the regex */
    char *pattern;   /* The regular expression pattern */
    void *re;        /* The compiled regex. void * to avoid
                      * having re library details invading the
                      * rest of the LG system; regex-morph.c
                      * takes care of all matching.
                      */
    Regex_node *next;
};


/* The following three structs comprise what is returned by post_process(). */
typedef struct D_type_list_struct D_type_list;
struct D_type_list_struct
{
    D_type_list * next;
    int type;
};

struct PP_node_struct
{
    D_type_list *d_type_array[MAX_LINKS];
    const char *violation;
};

/* Davy added these */

#ifdef USE_FAT_LINKAGES
typedef struct Andlist_struct Andlist;
struct Andlist_struct
{
    Andlist * next;
    int conjunction;
    int num_elements;
    int element[MAX_SENTENCE];
    int num_outside_words;
    int outside_word[MAX_SENTENCE];
    int cost;
};
#endif /* USE_FAT_LINKAGES */

/** 
 * This is for building the graphs of links in post-processing and 
 * fat link extraction.
 */
struct Linkage_info_struct
{
    int index;
#ifdef USE_FAT_LINKAGES
    Boolean canonical;
    Boolean fat;
    Boolean improper_fat_linkage;
    Boolean inconsistent_domains;
    short and_cost;
#endif /* USE_FAT_LINKAGES */
    short N_violations;
    short null_cost;
    short unused_word_cost;
    short link_cost;
    float disjunct_cost;
    double corpus_cost;
    int island[MAX_SENTENCE];
    size_t nwords;
#ifdef USE_FAT_LINKAGES
    Andlist * andlist;
#endif /* USE_FAT_LINKAGES */
    char **disjunct_list_str;
#ifdef USE_CORPUS
    Sense **sense_list;
#endif
};

struct List_o_links_struct
{
    int link;       /* the link number */
    short word;       /* the word at the other end of this link */
    short dir;        /* 0: undirected, 1: away from me, -1: toward me */
    List_o_links * next;
};

#ifdef USE_FAT_LINKAGES
/* These parameters tell power_pruning, to tell whether this is before
 * or after generating and disjuncts.  GENTLE is before RUTHLESS is 
 * after.
 */
#define GENTLE 1
#endif /* USE_FAT_LINKAGES */
#define RUTHLESS 0

typedef struct Parse_choice_struct Parse_choice;

struct Parse_choice_struct
{
    Parse_choice * next;
    Parse_set * set[2];
    Link        link[2];   /* the lc fields of these is NULL if there is no link used */
    Disjunct *ld, *md, *rd;  /* the chosen disjuncts for the relevant three words */
};

struct Parse_set_struct
{
    s64 count;  /* the number of ways */
    Parse_choice * first;
    Parse_choice * current;  /* used to enumerate linkages */
};

struct X_table_connector_struct
{
    short             lw, rw;
    unsigned short    cost;
    Parse_set         *set;
    Connector         *le, *re;
    X_table_connector *next;
};

/* from string-set.c */
struct String_set_s
{
    unsigned int size;       /* the current size of the table */
    unsigned int count;      /* number of things currently in the table */
    char ** table;  /* the table itself */
};


/* from pp_linkset.c */
typedef struct pp_linkset_node_s
{
    const char *str;
    struct pp_linkset_node_s *next;
} pp_linkset_node;

struct pp_linkset_s
{
    unsigned int hash_table_size;
    unsigned int population;
    pp_linkset_node **hash_table;    /* data actually lives here */
};


/* from pp_lexer.c */
#define PP_LEXER_MAX_LABELS 512

typedef struct pp_label_node_s
{
    /* linked list of strings associated with a label in the table */
    const char *str;
    struct pp_label_node_s *next;
} pp_label_node;                 /* next=NULL: end of list */


/* from pp_knowledge.c */
typedef struct StartingLinkAndDomain_s
{
    const char *starting_link;
    int   domain;       /* domain which the link belongs to (-1: terminator)*/
} StartingLinkAndDomain;

typedef struct pp_rule_s
{
    /* Holds a single post-processing rule. Since rules come in many
       flavors, not all fields of the following are always relevant  */
    const char *selector; /* name of link to which rule applies      */
    pp_linkset *link_set; /* handle to set of links relevant to rule */
    int   link_set_size;  /* size of this set                        */
    int   domain;         /* type of domain to which rule applies    */
    const char  **link_array; /* array holding the spelled-out names */
    const char  *msg;     /* explanation (NULL=end sentinel in array)*/
} pp_rule;

typedef struct PPLexTable_s PPLexTable;
struct pp_knowledge_s
{
    PPLexTable *lt; /* Internal rep'n of sets of strings from knowledge file */
    const char *path; /* Name of file we loaded from */

    /* handles to sets of links specified in knowledge file. These constitute
       auxiliary data, necessary to implement the rules, below. See comments
       in post-process.c for a description of these. */
    pp_linkset *domain_starter_links;
    pp_linkset *urfl_domain_starter_links;
    pp_linkset *urfl_only_domain_starter_links;
    pp_linkset *domain_contains_links;
    pp_linkset *must_form_a_cycle_links;
    pp_linkset *restricted_links;
    pp_linkset *ignore_these_links;
    pp_linkset *left_domain_starter_links;

    /* arrays of rules specified in knowledge file */
    pp_rule *connected_rules, *form_a_cycle_rules;
    pp_rule *contains_one_rules, *contains_none_rules;
    pp_rule *bounded_rules;

    int n_connected_rules, n_form_a_cycle_rules;
    int n_contains_one_rules, n_contains_none_rules;
    int n_bounded_rules;

    int nStartingLinks;
    pp_linkset *set_of_links_starting_bounded_domain;
    StartingLinkAndDomain *starting_link_lookup_table;
    String_set *string_set;
};


#endif

