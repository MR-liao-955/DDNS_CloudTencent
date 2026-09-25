/*
  Copyright (c) 2009 Dave Gamble
 
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
 
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
 
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#ifndef tretap_CJson__h
#define tretap_CJson__h

#ifdef __cplusplus
extern "C"
{
#endif

/* tretap_CJson Types: */
#define tretap_CJson_False 0
#define tretap_CJson_True 1
#define tretap_CJson_NULL 2
#define tretap_CJson_Number 3
#define tretap_CJson_String 4
#define tretap_CJson_Array 5
#define tretap_CJson_Object 6
	
#define tretap_CJson_IsReference 256
#define tretap_CJson_StringIsConst 512

/* The tretap_CJson structure: */
typedef struct tretap_CJson {
	struct tretap_CJson *next,*prev;	/* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
	struct tretap_CJson *child;		/* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */

	int type;					/* The type of the item, as above. */

	char *valuestring;			/* The item's string, if type==tretap_CJson_String */
	int valueint;				/* The item's number, if type==tretap_CJson_Number */
	double valuedouble;			/* The item's number, if type==tretap_CJson_Number */

	char *string;				/* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
} tretap_CJson;

typedef struct tretap_CJson_Hooks {
      void *(*malloc_fn)(unsigned int sz);
      void (*free_fn)(void *ptr);
} tretap_CJson_Hooks;

/* Supply malloc, realloc and free functions to tretap_CJson */
extern void tretap_CJson_InitHooks(tretap_CJson_Hooks* hooks);


/* Supply a block of JSON, and this returns a tretap_CJson object you can interrogate. Call tretap_CJson_Delete when finished. */
extern tretap_CJson *tretap_CJson_Parse(const char *value);
/* Render a tretap_CJson entity to text for transfer/storage. Free the char* when finished. */
extern char  *tretap_CJson_Print(tretap_CJson *item);
/* Render a tretap_CJson entity to text for transfer/storage without any formatting. Free the char* when finished. */
extern char  *tretap_CJson_PrintUnformatted(tretap_CJson *item);
/* Render a tretap_CJson entity to text using a buffered strategy. prebuffer is a guess at the final size. guessing well reduces reallocation. fmt=0 gives unformatted, =1 gives formatted */
extern char *tretap_CJson_PrintBuffered(tretap_CJson *item,int prebuffer,int fmt);
/* Delete a tretap_CJson entity and all subentities. */
extern void   tretap_CJson_Delete(tretap_CJson *c);

/* Returns the number of items in an array (or object). */
extern int	  tretap_CJson_GetArraySize(tretap_CJson *array);
/* Retrieve item number "item" from array "array". Returns NULL if unsuccessful. */
extern tretap_CJson *tretap_CJson_GetArrayItem(tretap_CJson *array,int item);
/* Get item "string" from object. Case insensitive. */
extern tretap_CJson *tretap_CJson_GetObjectItem(tretap_CJson *object,const char *string);

/* For analysing failed parses. This returns a pointer to the parse error. You'll probably need to look a few chars back to make sense of it. Defined when tretap_CJson_Parse() returns 0. 0 when tretap_CJson_Parse() succeeds. */
extern const char *tretap_CJson_GetErrorPtr(void);
	
/* These calls create a tretap_CJson item of the appropriate type. */
extern tretap_CJson *tretap_CJson_CreateNull(void);
extern tretap_CJson *tretap_CJson_CreateTrue(void);
extern tretap_CJson *tretap_CJson_CreateFalse(void);
extern tretap_CJson *tretap_CJson_CreateBool(int b);
extern tretap_CJson *tretap_CJson_CreateNumber(double num);
extern tretap_CJson *tretap_CJson_CreateString(const char *string);
extern tretap_CJson *tretap_CJson_CreateArray(void);
extern tretap_CJson *tretap_CJson_CreateObject(void);

/* These utilities create an Array of count items. */
extern tretap_CJson *tretap_CJson_CreateIntArray(const int *numbers,int count);
extern tretap_CJson *tretap_CJson_CreateFloatArray(const float *numbers,int count);
extern tretap_CJson *tretap_CJson_CreateDoubleArray(const double *numbers,int count);
extern tretap_CJson *tretap_CJson_CreateStringArray(const char **strings,int count);

/* Append item to the specified array/object. */
extern void tretap_CJson_AddItemToArray(tretap_CJson *array, tretap_CJson *item);
extern void tretap_CJson_AddItemToObject(tretap_CJson *object,const char *string,tretap_CJson *item);
extern void tretap_CJson_AddItemToObjectCS(tretap_CJson *object,const char *string,tretap_CJson *item);	/* Use this when string is definitely const (i.e. a literal, or as good as), and will definitely survive the tretap_CJson object */
/* Append reference to item to the specified array/object. Use this when you want to add an existing tretap_CJson to a new tretap_CJson, but don't want to corrupt your existing tretap_CJson. */
extern void tretap_CJson_AddItemReferenceToArray(tretap_CJson *array, tretap_CJson *item);
extern void tretap_CJson_AddItemReferenceToObject(tretap_CJson *object,const char *string,tretap_CJson *item);

/* Remove/Detatch items from Arrays/Objects. */
extern tretap_CJson *tretap_CJson_DetachItemFromArray(tretap_CJson *array,int which);
extern void tretap_CJson_DeleteItemFromArray(tretap_CJson *array,int which);
extern tretap_CJson *tretap_CJson_DetachItemFromObject(tretap_CJson *object,const char *string);
extern void tretap_CJson_DeleteItemFromObject(tretap_CJson *object,const char *string);
	
/* Update array items. */
extern void tretap_CJson_InsertItemInArray(tretap_CJson *array,int which,tretap_CJson *newitem);	/* Shifts pre-existing items to the right. */
extern void tretap_CJson_ReplaceItemInArray(tretap_CJson *array,int which,tretap_CJson *newitem);
extern void tretap_CJson_ReplaceItemInObject(tretap_CJson *object,const char *string,tretap_CJson *newitem);

/* Duplicate a tretap_CJson item */
extern tretap_CJson *tretap_CJson_Duplicate(tretap_CJson *item,int recurse);
/* Duplicate will create a new, identical tretap_CJson item to the one you pass, in new memory that will
need to be released. With recurse!=0, it will duplicate any children connected to the item.
The item->next and ->prev pointers are always zero on return from Duplicate. */

/* ParseWithOpts allows you to require (and check) that the JSON is null terminated, and to retrieve the pointer to the final byte parsed. */
extern tretap_CJson *tretap_CJson_ParseWithOpts(const char *value,const char **return_parse_end,int require_null_terminated);

extern void tretap_CJson_Minify(char *json);

/* Macros for creating things quickly. */
#define tretap_CJson_AddNullToObject(object,name)		tretap_CJson_AddItemToObject(object, name, tretap_CJson_CreateNull())
#define tretap_CJson_AddTrueToObject(object,name)		tretap_CJson_AddItemToObject(object, name, tretap_CJson_CreateTrue())
#define tretap_CJson_AddFalseToObject(object,name)	tretap_CJson_AddItemToObject(object, name, tretap_CJson_CreateFalse())
#define tretap_CJson_AddBoolToObject(object,name,b)	tretap_CJson_AddItemToObject(object, name, tretap_CJson_CreateBool(b))
#define tretap_CJson_AddNumberToObject(object,name,n)	tretap_CJson_AddItemToObject(object, name, tretap_CJson_CreateNumber(n))
#define tretap_CJson_AddStringToObject(object,name,s)	tretap_CJson_AddItemToObject(object, name, tretap_CJson_CreateString(s))

/* When assigning an integer value, it needs to be propagated to valuedouble too. */
#define tretap_CJson_SetIntValue(object,val)			((object)?(object)->valueint=(object)->valuedouble=(val):(val))
#define tretap_CJson_SetNumberValue(object,val)		((object)?(object)->valueint=(object)->valuedouble=(val):(val))

#ifdef __cplusplus
}
#endif

#endif
