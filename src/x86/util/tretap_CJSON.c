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

/* JSON parser in C. */

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>
#include "tretap_cJSON.h"

extern void *ddns_mem_alloc(int size);
extern void ddns_mem_free(void **ptr);

#define tretap_CJson_malloc ddns_mem_alloc
#define tretap_CJson_free ddns_mem_free

static const char *ep;

const char *tretap_CJson_GetErrorPtr(void) {return ep;}

static int tretap_CJson_strcasecmp(const char *s1,const char *s2)
{
	if (!s1) return (s1==s2)?0:1;if (!s2) return 1;
	for(; tolower(*s1) == tolower(*s2); ++s1, ++s2)	if(*s1 == 0)	return 0;
	return tolower(*(const unsigned char *)s1) - tolower(*(const unsigned char *)s2);
}

static char* tretap_CJson_strdup(const char* str)
{
      size_t len;
      char* copy;

      len = strlen(str) + 1;
      if (!(copy = (char*)tretap_CJson_malloc(len))) return 0;
      memcpy(copy,str,len);
      return copy;
}

void tretap_CJson_InitHooks(tretap_CJson_Hooks* hooks)
{
    if (!hooks) { /* Reset hooks */
        //tretap_CJson_malloc = malloc;
        //tretap_CJson_free = free;
        return;
    }

	//tretap_CJson_malloc = (hooks->malloc_fn)?hooks->malloc_fn:malloc;
	//tretap_CJson_free	 = (hooks->free_fn)?hooks->free_fn:free;
}

/* Internal constructor. */
static tretap_CJson *tretap_CJson_New_Item(void)
{
	tretap_CJson* node = (tretap_CJson*)tretap_CJson_malloc(sizeof(tretap_CJson));
	if (node) memset(node,0,sizeof(tretap_CJson));
	return node;
}

/* Delete a tretap_CJson structure. */
void tretap_CJson_Delete(tretap_CJson *c)
{
	tretap_CJson *next;

	while (c)
	{
		next=c->next;
		if (!(c->type&tretap_CJson_IsReference) && c->child) tretap_CJson_Delete(c->child);
		if (!(c->type&tretap_CJson_IsReference) && c->valuestring) tretap_CJson_free((void**)&c->valuestring);
		if (!(c->type&tretap_CJson_StringIsConst) && c->string) tretap_CJson_free((void**)&c->string);
		tretap_CJson_free((void**)&c);
		c=next;
	}
}

/* Parse the input text to generate a number, and populate the result into item. */
static const char *parse_number(tretap_CJson *item,const char *num)
{
	double n=0,sign=1,scale=0;int subscale=0,signsubscale=1;

	if (*num=='-') sign=-1,num++;	/* Has sign? */
	if (*num=='0') num++;			/* is zero */
	if (*num>='1' && *num<='9')	do	n=(n*10.0)+(*num++ -'0');	while (*num>='0' && *num<='9');	/* Number? */
	if (*num=='.' && num[1]>='0' && num[1]<='9') {num++;		do	n=(n*10.0)+(*num++ -'0'),scale--; while (*num>='0' && *num<='9');}	/* Fractional part? */
	if (*num=='e' || *num=='E')		/* Exponent? */
	{	num++;if (*num=='+') num++;	else if (*num=='-') signsubscale=-1,num++;		/* With sign? */
		while (*num>='0' && *num<='9') subscale=(subscale*10)+(*num++ - '0');	/* Number? */
	}

	n=sign*n*pow(10.0,(scale+subscale*signsubscale));	/* number = +/- number.fraction * 10^+/- exponent */

	item->valuedouble=n;
	item->valueint=(int)n;
	item->type=tretap_CJson_Number;
	return num;
}

static int pow2gt (int x)	{	--x;	x|=x>>1;	x|=x>>2;	x|=x>>4;	x|=x>>8;	x|=x>>16;	return x+1;	}

typedef struct {char *buffer; int length; int offset; } printbuffer;

static char* ensure(printbuffer *p,int needed)
{
	char *newbuffer;int newsize;
	if (!p || !p->buffer) return 0;
	needed+=p->offset;
	if (needed<=p->length) return p->buffer+p->offset;

	newsize=pow2gt(needed);
	newbuffer=(char*)tretap_CJson_malloc(newsize);
	if (!newbuffer) {tretap_CJson_free((void**)&p->buffer);p->length=0,p->buffer=0;return 0;}
	if (newbuffer) memcpy(newbuffer,p->buffer,p->length);
	tretap_CJson_free((void**)&p->buffer);
	p->length=newsize;
	p->buffer=newbuffer;
	return newbuffer+p->offset;
}

static int update(printbuffer *p)
{
	char *str;
	if (!p || !p->buffer) return 0;
	str=p->buffer+p->offset;
	return p->offset+strlen(str);
}

/* Render the number nicely from the given item into a string. */
static char *print_number(tretap_CJson *item,printbuffer *p)
{
	char *str=0;
	double d=item->valuedouble;
	if (d==0)
	{
		if (p)	str=ensure(p,2);
		else	str=(char*)tretap_CJson_malloc(2);	/* special case for 0. */
		if (str) strcpy(str,"0");
	}
	else if (fabs(((double)item->valueint)-d)<=DBL_EPSILON && d<=INT_MAX && d>=INT_MIN)
	{
		if (p)	str=ensure(p,21);
		else	str=(char*)tretap_CJson_malloc(21);	/* 2^64+1 can be represented in 21 chars. */
		if (str)	sprintf(str,"%d",item->valueint);
	}
	else
	{
		if (p)	str=ensure(p,64);
		else	str=(char*)tretap_CJson_malloc(64);	/* This is a nice tradeoff. */
		if (str)
		{
			if (fabs(floor(d)-d)<=DBL_EPSILON && fabs(d)<1.0e60)sprintf(str,"%.0f",d);
			else if (fabs(d)<1.0e-6 || fabs(d)>1.0e9)			sprintf(str,"%e",d);
			else												sprintf(str,"%f",d);
		}
	}
	return str;
}

static unsigned parse_hex4(const char *str)
{
	unsigned h=0;
	if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
	h=h<<4;str++;
	if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
	h=h<<4;str++;
	if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
	h=h<<4;str++;
	if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
	return h;
}

/* Parse the input text into an unescaped cstring, and populate item. */
static const unsigned char firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
static const char *parse_string(tretap_CJson *item,const char *str)
{
	const char *ptr=str+1;char *ptr2;char *out;int len=0;unsigned uc,uc2;
	if (*str!='\"') {ep=str;return 0;}	/* not a string! */

	while (*ptr!='\"' && *ptr && ++len) if (*ptr++ == '\\') ptr++;	/* Skip escaped quotes. */

	out=(char*)tretap_CJson_malloc(len+1);	/* This is how long we need for the string, roughly. */
	if (!out) return 0;

	ptr=str+1;ptr2=out;
	while (*ptr!='\"' && *ptr)
	{
		if (*ptr!='\\') *ptr2++=*ptr++;
		else
		{
			ptr++;
			switch (*ptr)
			{
				case 'b': *ptr2++='\b';	break;
				case 'f': *ptr2++='\f';	break;
				case 'n': *ptr2++='\n';	break;
				case 'r': *ptr2++='\r';	break;
				case 't': *ptr2++='\t';	break;
				case 'u':	 /* transcode utf16 to utf8. */
					uc=parse_hex4(ptr+1);ptr+=4;	/* get the unicode char. */

					if ((uc>=0xDC00 && uc<=0xDFFF) || uc==0)	break;	/* check for invalid.	*/

					if (uc>=0xD800 && uc<=0xDBFF)	/* UTF16 surrogate pairs.	*/
					{
						if (ptr[1]!='\\' || ptr[2]!='u')	break;	/* missing second-half of surrogate.	*/
						uc2=parse_hex4(ptr+3);ptr+=6;
						if (uc2<0xDC00 || uc2>0xDFFF)		break;	/* invalid second-half of surrogate.	*/
						uc=0x10000 + (((uc&0x3FF)<<10) | (uc2&0x3FF));
					}

					len=4;if (uc<0x80) len=1;else if (uc<0x800) len=2;else if (uc<0x10000) len=3; ptr2+=len;

					switch (len) {
						case 4: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
						case 3: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
						case 2: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
						case 1: *--ptr2 =(uc | firstByteMark[len]);
					}
					ptr2+=len;
					break;
				default:  *ptr2++=*ptr; break;
			}
			ptr++;
		}
	}
	*ptr2=0;
	if (*ptr=='\"') ptr++;
	item->valuestring=out;
	item->type=tretap_CJson_String;
	return ptr;
}

/* Render the cstring provided to an escaped version that can be printed. */
static char *print_string_ptr(const char *str,printbuffer *p)
{
	const char *ptr;char *ptr2,*out;int len=0,flag=0;unsigned char token;

	for (ptr=str;*ptr;ptr++) flag|=((*ptr>0 && *ptr<32)||(*ptr=='\"')||(*ptr=='\\'))?1:0;
	if (!flag)
	{
		len=ptr-str;
		if (p) out=ensure(p,len+3);
		else		out=(char*)tretap_CJson_malloc(len+3);
		if (!out) return 0;
		ptr2=out;*ptr2++='\"';
		strcpy(ptr2,str);
		ptr2[len]='\"';
		ptr2[len+1]=0;
		return out;
	}

	if (!str)
	{
		if (p)	out=ensure(p,3);
		else	out=(char*)tretap_CJson_malloc(3);
		if (!out) return 0;
		strcpy(out,"\"\"");
		return out;
	}
	ptr=str;while ((token=*ptr) && ++len) {if (strchr("\"\\\b\f\n\r\t",token)) len++; else if (token<32) len+=5;ptr++;}

	if (p)	out=ensure(p,len+3);
	else	out=(char*)tretap_CJson_malloc(len+3);
	if (!out) return 0;

	ptr2=out;ptr=str;
	*ptr2++='\"';
	while (*ptr)
	{
		if ((unsigned char)*ptr>31 && *ptr!='\"' && *ptr!='\\') *ptr2++=*ptr++;
		else
		{
			*ptr2++='\\';
			switch (token=*ptr++)
			{
				case '\\':	*ptr2++='\\';	break;
				case '\"':	*ptr2++='\"';	break;
				case '\b':	*ptr2++='b';	break;
				case '\f':	*ptr2++='f';	break;
				case '\n':	*ptr2++='n';	break;
				case '\r':	*ptr2++='r';	break;
				case '\t':	*ptr2++='t';	break;
				default: sprintf(ptr2,"u%04x",token);ptr2+=5;	break;	/* escape and print */
			}
		}
	}
	*ptr2++='\"';*ptr2++=0;
	return out;
}
/* Invote print_string_ptr (which is useful) on an item. */
static char *print_string(tretap_CJson *item,printbuffer *p)	{return print_string_ptr(item->valuestring,p);}

/* Predeclare these prototypes. */
static const char *parse_value(tretap_CJson *item,const char *value);
static char *print_value(tretap_CJson *item,int depth,int fmt,printbuffer *p);
static const char *parse_array(tretap_CJson *item,const char *value);
static char *print_array(tretap_CJson *item,int depth,int fmt,printbuffer *p);
static const char *parse_object(tretap_CJson *item,const char *value);
static char *print_object(tretap_CJson *item,int depth,int fmt,printbuffer *p);

/* Utility to jump whitespace and cr/lf */
static const char *skip(const char *in) {while (in && *in && (unsigned char)*in<=32) in++; return in;}

/* Parse an object - create a new root, and populate. */
tretap_CJson *tretap_CJson_ParseWithOpts(const char *value,const char **return_parse_end,int require_null_terminated)
{
	const char *end=0;
	tretap_CJson *c=tretap_CJson_New_Item();
	ep=0;
	if (!c) return 0;       /* memory fail */
	end=parse_value(c,skip(value));
	if (!end)	{tretap_CJson_Delete(c);return 0;}	/* parse failure. ep is set. */
	/* if we require null-terminated JSON without appended garbage, skip and then check for a null terminator */
	if (require_null_terminated) {end=skip(end);if (*end) {tretap_CJson_Delete(c);ep=end;return 0;}}
	if (return_parse_end) *return_parse_end=end;
	return c;
}
/* Default options for tretap_CJson_Parse */
tretap_CJson *tretap_CJson_Parse(const char *value) {return tretap_CJson_ParseWithOpts(value,0,0);}

/* Render a tretap_CJson item/entity/structure to text. */
char *tretap_CJson_Print(tretap_CJson *item)				{return print_value(item,0,1,0);}
char *tretap_CJson_PrintUnformatted(tretap_CJson *item)	{return print_value(item,0,0,0);}

char *tretap_CJson_PrintBuffered(tretap_CJson *item,int prebuffer,int fmt)
{
	printbuffer p;
	p.buffer=(char*)tretap_CJson_malloc(prebuffer);
	p.length=prebuffer;
	p.offset=0;
	return print_value(item,0,fmt,&p);
	return p.buffer;
}


/* Parser core - when encountering text, process appropriately. */
static const char *parse_value(tretap_CJson *item,const char *value)
{
	if (!value)						return 0;	/* Fail on null. */
	if (!strncmp(value,"null",4))	{ item->type=tretap_CJson_NULL;  return value+4; }
	if (!strncmp(value,"false",5))	{ item->type=tretap_CJson_False; return value+5; }
	if (!strncmp(value,"true",4))	{ item->type=tretap_CJson_True; item->valueint=1;	return value+4; }
	if (*value=='\"')				{ return parse_string(item,value); }
	if (*value=='-' || (*value>='0' && *value<='9'))	{ return parse_number(item,value); }
	if (*value=='[')				{ return parse_array(item,value); }
	if (*value=='{')				{ return parse_object(item,value); }

	ep=value;return 0;	/* failure. */
}

/* Render a value to text. */
static char *print_value(tretap_CJson *item,int depth,int fmt,printbuffer *p)
{
	char *out=0;
	if (!item) return 0;
	if (p)
	{
		switch ((item->type)&255)
		{
			case tretap_CJson_NULL:	{out=ensure(p,5);	if (out) strcpy(out,"null");	break;}
			case tretap_CJson_False:	{out=ensure(p,6);	if (out) strcpy(out,"false");	break;}
			case tretap_CJson_True:	{out=ensure(p,5);	if (out) strcpy(out,"true");	break;}
			case tretap_CJson_Number:	out=print_number(item,p);break;
			case tretap_CJson_String:	out=print_string(item,p);break;
			case tretap_CJson_Array:	out=print_array(item,depth,fmt,p);break;
			case tretap_CJson_Object:	out=print_object(item,depth,fmt,p);break;
		}
	}
	else
	{
		switch ((item->type)&255)
		{
			case tretap_CJson_NULL:	out=tretap_CJson_strdup("null");	break;
			case tretap_CJson_False:	out=tretap_CJson_strdup("false");break;
			case tretap_CJson_True:	out=tretap_CJson_strdup("true"); break;
			case tretap_CJson_Number:	out=print_number(item,0);break;
			case tretap_CJson_String:	out=print_string(item,0);break;
			case tretap_CJson_Array:	out=print_array(item,depth,fmt,0);break;
			case tretap_CJson_Object:	out=print_object(item,depth,fmt,0);break;
		}
	}
	return out;
}

/* Build an array from input text. */
static const char *parse_array(tretap_CJson *item,const char *value)
{
	tretap_CJson *child;
	if (*value!='[')	{ep=value;return 0;}	/* not an array! */

	item->type=tretap_CJson_Array;
	value=skip(value+1);
	if (*value==']') return value+1;	/* empty array. */

	item->child=child=tretap_CJson_New_Item();
	if (!item->child) return 0;		 /* memory fail */
	value=skip(parse_value(child,skip(value)));	/* skip any spacing, get the value. */
	if (!value) return 0;

	while (*value==',')
	{
		tretap_CJson *new_item;
		if (!(new_item=tretap_CJson_New_Item())) return 0; 	/* memory fail */
		child->next=new_item;new_item->prev=child;child=new_item;
		value=skip(parse_value(child,skip(value+1)));
		if (!value) return 0;	/* memory fail */
	}

	if (*value==']') return value+1;	/* end of array */
	ep=value;return 0;	/* malformed. */
}

/* Render an array to text */
static char *print_array(tretap_CJson *item,int depth,int fmt,printbuffer *p)
{
	char **entries;
	char *out=0,*ptr,*ret;int len=5;
	tretap_CJson *child=item->child;
	int numentries=0,i=0,fail=0;
	size_t tmplen=0;

	/* How many entries in the array? */
	while (child) numentries++,child=child->next;
	/* Explicitly handle numentries==0 */
	if (!numentries)
	{
		if (p)	out=ensure(p,3);
		else	out=(char*)tretap_CJson_malloc(3);
		if (out) strcpy(out,"[]");
		return out;
	}

	if (p)
	{
		/* Compose the output array. */
		i=p->offset;
		ptr=ensure(p,1);if (!ptr) return 0;	*ptr='[';	p->offset++;
		child=item->child;
		while (child && !fail)
		{
			print_value(child,depth+1,fmt,p);
			p->offset=update(p);
			if (child->next) {len=fmt?2:1;ptr=ensure(p,len+1);if (!ptr) return 0;*ptr++=',';if(fmt)*ptr++=' ';*ptr=0;p->offset+=len;}
			child=child->next;
		}
		ptr=ensure(p,2);if (!ptr) return 0;	*ptr++=']';*ptr=0;
		out=(p->buffer)+i;
	}
	else
	{
		/* Allocate an array to hold the values for each */
		entries=(char**)tretap_CJson_malloc(numentries*sizeof(char*));
		if (!entries) return 0;
		memset(entries,0,numentries*sizeof(char*));
		/* Retrieve all the results: */
		child=item->child;
		while (child && !fail)
		{
			ret=print_value(child,depth+1,fmt,0);
			entries[i++]=ret;
			if (ret) len+=strlen(ret)+2+(fmt?1:0); else fail=1;
			child=child->next;
		}

		/* If we didn't fail, try to malloc the output string */
		if (!fail)	out=(char*)tretap_CJson_malloc(len);
		/* If that fails, we fail. */
		if (!out) fail=1;

		/* Handle failure. */
		if (fail)
		{
			for (i=0;i<numentries;i++) if (entries[i]) tretap_CJson_free((void**)&entries[i]);
			tretap_CJson_free((void**)&entries);
			return 0;
		}

		/* Compose the output array. */
		*out='[';
		ptr=out+1;*ptr=0;
		for (i=0;i<numentries;i++)
		{
			tmplen=strlen(entries[i]);memcpy(ptr,entries[i],tmplen);ptr+=tmplen;
			if (i!=numentries-1) {*ptr++=',';if(fmt)*ptr++=' ';*ptr=0;}
			tretap_CJson_free((void**)&entries[i]);
		}
		tretap_CJson_free((void**)&entries);
		*ptr++=']';*ptr++=0;
	}
	return out;
}

/* Build an object from the text. */
static const char *parse_object(tretap_CJson *item,const char *value)
{
	tretap_CJson *child;
	if (*value!='{')	{ep=value;return 0;}	/* not an object! */

	item->type=tretap_CJson_Object;
	value=skip(value+1);
	if (*value=='}') return value+1;	/* empty array. */

	item->child=child=tretap_CJson_New_Item();
	if (!item->child) return 0;
	value=skip(parse_string(child,skip(value)));
	if (!value) return 0;
	child->string=child->valuestring;child->valuestring=0;
	if (*value!=':') {ep=value;return 0;}	/* fail! */
	value=skip(parse_value(child,skip(value+1)));	/* skip any spacing, get the value. */
	if (!value) return 0;

	while (*value==',')
	{
		tretap_CJson *new_item;
		if (!(new_item=tretap_CJson_New_Item()))	return 0; /* memory fail */
		child->next=new_item;new_item->prev=child;child=new_item;
		value=skip(parse_string(child,skip(value+1)));
		if (!value) return 0;
		child->string=child->valuestring;child->valuestring=0;
		if (*value!=':') {ep=value;return 0;}	/* fail! */
		value=skip(parse_value(child,skip(value+1)));	/* skip any spacing, get the value. */
		if (!value) return 0;
	}

	if (*value=='}') return value+1;	/* end of array */
	ep=value;return 0;	/* malformed. */
}

/* Render an object to text. */
static char *print_object(tretap_CJson *item,int depth,int fmt,printbuffer *p)
{
	char **entries=0,**names=0;
	char *out=0,*ptr,*ret,*str;int len=7,i=0,j;
	tretap_CJson *child=item->child;
	int numentries=0,fail=0;
	size_t tmplen=0;
	/* Count the number of entries. */
	while (child) numentries++,child=child->next;
	/* Explicitly handle empty object case */
	if (!numentries)
	{
		if (p) out=ensure(p,fmt?depth+4:3);
		else	out=(char*)tretap_CJson_malloc(fmt?depth+4:3);
		if (!out)	return 0;
		ptr=out;*ptr++='{';
		if (fmt) {*ptr++='\n';for (i=0;i<depth-1;i++) *ptr++='\t';}
		*ptr++='}';*ptr++=0;
		return out;
	}
	if (p)
	{
		/* Compose the output: */
		i=p->offset;
		len=fmt?2:1;	ptr=ensure(p,len+1);	if (!ptr) return 0;
		*ptr++='{';	if (fmt) *ptr++='\n';	*ptr=0;	p->offset+=len;
		child=item->child;depth++;
		while (child)
		{
			if (fmt)
			{
				ptr=ensure(p,depth);	if (!ptr) return 0;
				for (j=0;j<depth;j++) *ptr++='\t';
				p->offset+=depth;
			}
			print_string_ptr(child->string,p);
			p->offset=update(p);

			len=fmt?2:1;
			ptr=ensure(p,len);	if (!ptr) return 0;
			*ptr++=':';if (fmt) *ptr++='\t';
			p->offset+=len;

			print_value(child,depth,fmt,p);
			p->offset=update(p);

			len=(fmt?1:0)+(child->next?1:0);
			ptr=ensure(p,len+1); if (!ptr) return 0;
			if (child->next) *ptr++=',';
			if (fmt) *ptr++='\n';*ptr=0;
			p->offset+=len;
			child=child->next;
		}
		ptr=ensure(p,fmt?(depth+1):2);	 if (!ptr) return 0;
		if (fmt)	for (i=0;i<depth-1;i++) *ptr++='\t';
		*ptr++='}';*ptr=0;
		out=(p->buffer)+i;
	}
	else
	{
		/* Allocate space for the names and the objects */
		entries=(char**)tretap_CJson_malloc(numentries*sizeof(char*));
		if (!entries) return 0;
		names=(char**)tretap_CJson_malloc(numentries*sizeof(char*));
		if (!names) {tretap_CJson_free((void**)&entries);return 0;}
		memset(entries,0,sizeof(char*)*numentries);
		memset(names,0,sizeof(char*)*numentries);

		/* Collect all the results into our arrays: */
		child=item->child;depth++;if (fmt) len+=depth;
		while (child)
		{
			names[i]=str=print_string_ptr(child->string,0);
			entries[i++]=ret=print_value(child,depth,fmt,0);
			if (str && ret) len+=strlen(ret)+strlen(str)+2+(fmt?2+depth:0); else fail=1;
			child=child->next;
		}

		/* Try to allocate the output string */
		if (!fail)	out=(char*)tretap_CJson_malloc(len);
		if (!out) fail=1;

		/* Handle failure */
		if (fail)
		{
			for (i=0;i<numentries;i++) {if (names[i]) tretap_CJson_free((void**)&names[i]);if (entries[i]) tretap_CJson_free((void**)&entries[i]);}
			tretap_CJson_free((void**)&names);tretap_CJson_free((void**)&entries);
			return 0;
		}

		/* Compose the output: */
		*out='{';ptr=out+1;if (fmt)*ptr++='\n';*ptr=0;
		for (i=0;i<numentries;i++)
		{
			if (fmt) for (j=0;j<depth;j++) *ptr++='\t';
			tmplen=strlen(names[i]);memcpy(ptr,names[i],tmplen);ptr+=tmplen;
			*ptr++=':';if (fmt) *ptr++='\t';
			strcpy(ptr,entries[i]);ptr+=strlen(entries[i]);
			if (i!=numentries-1) *ptr++=',';
			if (fmt) *ptr++='\n';*ptr=0;
			tretap_CJson_free((void**)&names[i]);tretap_CJson_free((void**)&entries[i]);
		}

		tretap_CJson_free((void**)&names);tretap_CJson_free((void**)&entries);
		if (fmt) for (i=0;i<depth-1;i++) *ptr++='\t';
		*ptr++='}';*ptr++=0;
	}
	return out;
}

/* Get Array size/item / object item. */
int    tretap_CJson_GetArraySize(tretap_CJson *array)
{
	tretap_CJson *c=array->child;
	int i=0;

	while(c)i++,c=c->next;
	return i;
}

tretap_CJson *tretap_CJson_GetArrayItem(tretap_CJson *array,int item)
{
	tretap_CJson *c=array->child;
	while (c && item>0) item--,c=c->next;
	return c;
}

tretap_CJson *tretap_CJson_GetObjectItem(tretap_CJson *object,const char *string)
{
	tretap_CJson *c=object->child;
	while (c && tretap_CJson_strcasecmp(c->string,string)) c=c->next;
	return c;
}

/* Utility for array list handling. */
static void suffix_object(tretap_CJson *prev,tretap_CJson *item) {prev->next=item;item->prev=prev;}
/* Utility for handling references. */
static tretap_CJson *create_reference(tretap_CJson *item) {tretap_CJson *ref=tretap_CJson_New_Item();if (!ref) return 0;memcpy(ref,item,sizeof(tretap_CJson));ref->string=0;ref->type|=tretap_CJson_IsReference;ref->next=ref->prev=0;return ref;}

/* Add item to array/object. */
void   tretap_CJson_AddItemToArray(tretap_CJson *array, tretap_CJson *item)						{tretap_CJson *c=array->child;if (!item) return; if (!c) {array->child=item;} else {while (c && c->next) c=c->next; suffix_object(c,item);}}
void   tretap_CJson_AddItemToObject(tretap_CJson *object,const char *string,tretap_CJson *item)	{if (!item) return; if (item->string) tretap_CJson_free((void**)&item->string);item->string=tretap_CJson_strdup(string);tretap_CJson_AddItemToArray(object,item);}
void   tretap_CJson_AddItemToObjectCS(tretap_CJson *object,const char *string,tretap_CJson *item)	{if (!item) return; if (!(item->type&tretap_CJson_StringIsConst) && item->string) tretap_CJson_free((void**)&item->string);item->string=(char*)string;item->type|=tretap_CJson_StringIsConst;tretap_CJson_AddItemToArray(object,item);}
void	tretap_CJson_AddItemReferenceToArray(tretap_CJson *array, tretap_CJson *item)						{tretap_CJson_AddItemToArray(array,create_reference(item));}
void	tretap_CJson_AddItemReferenceToObject(tretap_CJson *object,const char *string,tretap_CJson *item)	{tretap_CJson_AddItemToObject(object,string,create_reference(item));}

tretap_CJson *tretap_CJson_DetachItemFromArray(tretap_CJson *array,int which)			{tretap_CJson *c=array->child;while (c && which>0) c=c->next,which--;if (!c) return 0;
	if (c->prev) c->prev->next=c->next;if (c->next) c->next->prev=c->prev;if (c==array->child) array->child=c->next;c->prev=c->next=0;return c;}
void   tretap_CJson_DeleteItemFromArray(tretap_CJson *array,int which)			{tretap_CJson_Delete(tretap_CJson_DetachItemFromArray(array,which));}
tretap_CJson *tretap_CJson_DetachItemFromObject(tretap_CJson *object,const char *string) {int i=0;tretap_CJson *c=object->child;while (c && tretap_CJson_strcasecmp(c->string,string)) i++,c=c->next;if (c) return tretap_CJson_DetachItemFromArray(object,i);return 0;}
void   tretap_CJson_DeleteItemFromObject(tretap_CJson *object,const char *string) {tretap_CJson_Delete(tretap_CJson_DetachItemFromObject(object,string));}

/* Replace array/object items with new ones. */
void   tretap_CJson_InsertItemInArray(tretap_CJson *array,int which,tretap_CJson *newitem)		{tretap_CJson *c=array->child;while (c && which>0) c=c->next,which--;if (!c) {tretap_CJson_AddItemToArray(array,newitem);return;}
	newitem->next=c;newitem->prev=c->prev;c->prev=newitem;if (c==array->child) array->child=newitem; else newitem->prev->next=newitem;}
void   tretap_CJson_ReplaceItemInArray(tretap_CJson *array,int which,tretap_CJson *newitem)		{tretap_CJson *c=array->child;while (c && which>0) c=c->next,which--;if (!c) return;
	newitem->next=c->next;newitem->prev=c->prev;if (newitem->next) newitem->next->prev=newitem;
	if (c==array->child) array->child=newitem; else newitem->prev->next=newitem;c->next=c->prev=0;tretap_CJson_Delete(c);}
void   tretap_CJson_ReplaceItemInObject(tretap_CJson *object,const char *string,tretap_CJson *newitem){int i=0;tretap_CJson *c=object->child;while(c && tretap_CJson_strcasecmp(c->string,string))i++,c=c->next;if(c){newitem->string=tretap_CJson_strdup(string);tretap_CJson_ReplaceItemInArray(object,i,newitem);}}

/* Create basic types: */
tretap_CJson *tretap_CJson_CreateNull(void)					{tretap_CJson *item=tretap_CJson_New_Item();if(item)item->type=tretap_CJson_NULL;return item;}
tretap_CJson *tretap_CJson_CreateTrue(void)					{tretap_CJson *item=tretap_CJson_New_Item();if(item)item->type=tretap_CJson_True;return item;}
tretap_CJson *tretap_CJson_CreateFalse(void)					{tretap_CJson *item=tretap_CJson_New_Item();if(item)item->type=tretap_CJson_False;return item;}
tretap_CJson *tretap_CJson_CreateBool(int b)					{tretap_CJson *item=tretap_CJson_New_Item();if(item)item->type=b?tretap_CJson_True:tretap_CJson_False;return item;}
tretap_CJson *tretap_CJson_CreateNumber(double num)			{tretap_CJson *item=tretap_CJson_New_Item();if(item){item->type=tretap_CJson_Number;item->valuedouble=num;item->valueint=(int)num;}return item;}
tretap_CJson *tretap_CJson_CreateString(const char *string)	{tretap_CJson *item=tretap_CJson_New_Item();if(item){item->type=tretap_CJson_String;item->valuestring=tretap_CJson_strdup(string);}return item;}
tretap_CJson *tretap_CJson_CreateArray(void)					{tretap_CJson *item=tretap_CJson_New_Item();if(item)item->type=tretap_CJson_Array;return item;}
tretap_CJson *tretap_CJson_CreateObject(void)					{tretap_CJson *item=tretap_CJson_New_Item();if(item)item->type=tretap_CJson_Object;return item;}

/* Create Arrays: */
tretap_CJson *tretap_CJson_CreateIntArray(const int *numbers,int count)		{int i;tretap_CJson *n=0,*p=0,*a=tretap_CJson_CreateArray();for(i=0;a && i<count;i++){n=tretap_CJson_CreateNumber(numbers[i]);if(!i)a->child=n;else suffix_object(p,n);p=n;}return a;}
tretap_CJson *tretap_CJson_CreateFloatArray(const float *numbers,int count)	{int i;tretap_CJson *n=0,*p=0,*a=tretap_CJson_CreateArray();for(i=0;a && i<count;i++){n=tretap_CJson_CreateNumber(numbers[i]);if(!i)a->child=n;else suffix_object(p,n);p=n;}return a;}
tretap_CJson *tretap_CJson_CreateDoubleArray(const double *numbers,int count)	{int i;tretap_CJson *n=0,*p=0,*a=tretap_CJson_CreateArray();for(i=0;a && i<count;i++){n=tretap_CJson_CreateNumber(numbers[i]);if(!i)a->child=n;else suffix_object(p,n);p=n;}return a;}
tretap_CJson *tretap_CJson_CreateStringArray(const char **strings,int count)	{int i;tretap_CJson *n=0,*p=0,*a=tretap_CJson_CreateArray();for(i=0;a && i<count;i++){n=tretap_CJson_CreateString(strings[i]);if(!i)a->child=n;else suffix_object(p,n);p=n;}return a;}

/* Duplication */
tretap_CJson *tretap_CJson_Duplicate(tretap_CJson *item,int recurse)
{
	tretap_CJson *newitem,*cptr,*nptr=0,*newchild;
	/* Bail on bad ptr */
	if (!item) return 0;
	/* Create new item */
	newitem=tretap_CJson_New_Item();
	if (!newitem) return 0;
	/* Copy over all vars */
	newitem->type=item->type&(~tretap_CJson_IsReference),newitem->valueint=item->valueint,newitem->valuedouble=item->valuedouble;
	if (item->valuestring)	{newitem->valuestring=tretap_CJson_strdup(item->valuestring);	if (!newitem->valuestring)	{tretap_CJson_Delete(newitem);return 0;}}
	if (item->string)		{newitem->string=tretap_CJson_strdup(item->string);			if (!newitem->string)		{tretap_CJson_Delete(newitem);return 0;}}
	/* If non-recursive, then we're done! */
	if (!recurse) return newitem;
	/* Walk the ->next chain for the child. */
	cptr=item->child;
	while (cptr)
	{
		newchild=tretap_CJson_Duplicate(cptr,1);		/* Duplicate (with recurse) each item in the ->next chain */
		if (!newchild) {tretap_CJson_Delete(newitem);return 0;}
		if (nptr)	{nptr->next=newchild,newchild->prev=nptr;nptr=newchild;}	/* If newitem->child already set, then crosswire ->prev and ->next and move on */
		else		{newitem->child=newchild;nptr=newchild;}					/* Set newitem->child and move to it */
		cptr=cptr->next;
	}
	return newitem;
}

void tretap_CJson_Minify(char *json)
{
	char *into=json;
	while (*json)
	{
		if (*json==' ') json++;
		else if (*json=='\t') json++;	/* Whitespace characters. */
		else if (*json=='\r') json++;
		else if (*json=='\n') json++;
		else if (*json=='/' && json[1]=='/')  while (*json && *json!='\n') json++;	/* double-slash comments, to end of line. */
		else if (*json=='/' && json[1]=='*') {while (*json && !(*json=='*' && json[1]=='/')) json++;json+=2;}	/* multiline comments. */
		else if (*json=='\"'){*into++=*json++;while (*json && *json!='\"'){if (*json=='\\') *into++=*json++;*into++=*json++;}*into++=*json++;} /* string literals, which are \" sensitive. */
		else *into++=*json++;			/* All other characters. */
	}
	*into=0;	/* and null-terminate. */
}
