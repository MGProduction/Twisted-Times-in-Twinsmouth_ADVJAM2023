#if defined(ACOMPILER)

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#define TRUE 1
#define FALSE 0
typedef struct{
 int nil;
}lua_State;

void lua_settable(lua_State*l,int val) {}
void lua_pushliteral(lua_State*l,const char*key) {}
void lua_pushstring(lua_State*l,const char*key) {}
void lua_newtable( lua_State*l ) {}
void lua_pushinteger(lua_State*l,int i) {}
void dmLogInfo(const char*a,const char*b){}

#else
#include <dmsdk/sdk.h>
#include <ctype.h>
#endif

typedef struct {
                            int          size;
                            int          usedsize;
                            int          granularity;
                            int          nstrings;
                            int          kind;
                            char*        string;
                            int*         pos;
                           } stringset;                   

#define _STP_counter       2
#define _STP_extradata     4

int stringset_Create(stringset*stp,int granularity,int kind)
{
 stp->granularity=granularity;
 stp->size=stp->granularity;
 stp->nstrings=0;              
 stp->usedsize=0;
 stp->kind=kind;
 stp->string=(char*)calloc(stp->granularity,1);
 stp->pos=(int*)calloc(1024,sizeof(int));
 return 1;
}

int stringset_ResetContent(stringset*stp)
{
 free(stp->string);
 free(stp->pos);
 stp->nstrings=0;
 stp->size=0;
 stp->usedsize=0;
 return stringset_Create(stp,stp->granularity,stp->kind);
}

int stringset_SoftResetContent(stringset*stp)
{
 stp->nstrings=0;
 stp->usedsize=0;
 return 1;
}

int stringset_Destroy(stringset*stp)
{
 free(stp->string);
 free(stp->pos);
 stp->nstrings=0;
 stp->size=0;
 stp->usedsize=0;
 stp->granularity=0;
 return 1;
}

int stringset_FindStringCore(const stringset*stp,const char* string,int*lastwhere)
{
 int         i=0;
 char*   p=(char*)stp->string;
 int*  pos=stp->pos;
 if((stp->nstrings)&&string&&(*string))
  {
   for(i=0;i<stp->nstrings;i++)
    if(strcmp(string,p+pos[i])==0)
     return i;
  }
 if(lastwhere)
  *lastwhere=i;
 return -1;
}

int stringset_GetTextEx(const stringset*stp,int where,char* string,int* counter,char* data,unsigned short* sz)
{
 char*   p=(char*)stp->string;
 int*  pos=stp->pos;
 int         len;
 if((where<0)||(where>=stp->nstrings))
  {
   if(string)
    *string=0;
   if(counter)
    *counter=0;
   if(sz)
    *sz=0;
   return 0;
  }
 p+=pos[where];
 if(string)
  strcpy(string,p);
 len=strlen(p)*sizeof(char)+1;
 p+=len;
 if(counter)
  {
   if(stp->kind&_STP_counter)
    *counter=*(int*)p;
   else
    *counter=1;
  }
 
 if(stp->kind&_STP_counter)
  p+=sizeof(int);
 if(stp->kind&_STP_extradata)
  {
   int size=(unsigned char)p[0]+(unsigned char)p[1]*256;
   if(sz)
    *sz=size;
   p+=sizeof(unsigned short);
   if(data)
    memcpy(data,p,size);
  }
 else
  {
   if(sz)
    *sz=0;
  }
 return 1;
}

int stringset_FindString(const stringset*stp,const char* string)
{ 
 return stringset_FindStringCore(stp,string,NULL);
}

int stringset_AddStringEx(stringset*stp,const char* string,int counter,const char* data,unsigned short datasize)
{ 
 int         wherex,lastwhere=stp->nstrings;
 int         len=strlen(string)+1;
 char*   p=(char*)stp->string;
 int*  pos=stp->pos;
 wherex=stringset_FindStringCore(stp,string,&lastwhere);
 if(len>1)
  if(wherex==-1)
   {
    int add=len;
    if(stp->kind&_STP_counter)            add+=sizeof(int);
    if(stp->kind&_STP_extradata)          add+=sizeof(unsigned short)+datasize;   
    if(stp->usedsize+add+16>stp->size)
    {
     stp->size+=add+1024;
     p=stp->string=(char*)realloc(stp->string,stp->size);     
    }

    wherex=stp->nstrings;

    pos[wherex]=stp->usedsize;
    p+=pos[wherex];
    strcpy(p,string);
    p+=len;
    stp->usedsize+=len;
    if(stp->kind&_STP_counter)
     {
      *(int*)p=counter;
      p+=sizeof(int);
      stp->usedsize+=sizeof(int);
     }
    if(stp->kind&_STP_extradata)
     {
      p[0]=datasize&0xFF;
      p[1]=(datasize>>8)&0xFF;
      p+=sizeof(unsigned short);
      memcpy(p,data,datasize);
      stp->usedsize+=sizeof(unsigned short);
      stp->usedsize+=datasize;
     }
    stp->nstrings++;
    return wherex;
   }
  return -1;
}

typedef unsigned char u8;

typedef struct{
 char*sz;
 int  c,m;
}TFILE;
typedef TFILE* PFILE;

void F_writes(PFILE hf,const char*sz)
{
 int l=strlen(sz);
 if(hf->c+l+1>=hf->m)
  {
   hf->m=hf->c+l+256;
   hf->sz=(char*)realloc(hf->sz,hf->m);
  }
 memcpy(hf->sz+hf->c,sz,l);hf->c+=l;hf->sz[hf->c]=0;
}


void tabwrite(PFILE hf,int tb)
{
 tb--;
 while(tb--)
  F_writes(hf,"\t");
}

int empty(const char*l)
{
 while((*l==' ')||(*l=='\t'))
  l++;
 return *l==0; 
}

int cIsIn(const char*sz,const char*seq)
{
 if(sz&&seq&&*sz) 
  {
   const char*psz=strstr(seq,sz);
   if(psz)
    {
     int  len=strlen(sz);
     while(psz)
      {
       if(((psz==seq)||(psz[-1]=='|'))&&((psz[len]=='|')||(psz[len]==0)))
        return 1;
       else
        psz++;
       psz=strstr(psz,sz);
      }
    }
  }
 return 0;
}

const char* S_getline(const char* stringa,char* line)
{
 int i;
 for(i=0;(stringa[i]!=0)&&(stringa[i]!='\r')&&(stringa[i]!='\n')&&(stringa[i]!=11);i++)
  if(line)
   line[i]=stringa[i];
 if(line)
  line[i]=0;
 if((stringa[i]=='\n')||(stringa[i]==11))
  i++;
 else
  if(stringa[i]=='\r')
   {
    i++;
    if(stringa[i]=='\n')
     i++;
   }
 if(stringa[i]==0)
  return NULL;
 else
  return stringa+i;
}

const char* S_gettokenex(const char* stringa,char* token,char sep)
{
 int i=0;
 while(*stringa&&(*stringa!=sep))
  if(token)
   token[i++]=*stringa++;
  else 
   stringa++;
 if(token)  
  token[i]=0;
 if(*stringa)
  stringa++;
 if(*stringa==0)
  return NULL;
 else
  return stringa;
}

void getkeyval(PFILE hf,char*key,char*val)
{
 int i=0,j;
 while((hf->sz[i]=='\t')||(hf->sz[i]==' ')) i++;
 j=0;
 while((hf->sz[i]!=':')&&(hf->sz[i]!='\r')&&(hf->sz[i]!='\n')&&(hf->sz[i]))
  key[j++]=hf->sz[i++];
 key[j]=0;while(j&&((key[j-1]==' ')||(val[j-1]==' '))) key[--j]=0;

 if(hf->sz[i]==':')
  {
   i++;
   while((hf->sz[i]=='\t')||(hf->sz[i]==' ')) i++;
   j=0;
   while((hf->sz[i]!='\r')&&(hf->sz[i]!='\n')&&(hf->sz[i]))
    val[j++]=hf->sz[i++];
   val[j]=0;while(j&&((val[j-1]==' ')||(val[j-1]=='\t'))) val[--j]=0;
  }
 else
  *val=0; 
}

void local_S_stripext(char*fn)
{
 int l=strlen(fn);
 while(l--)
  if(fn[l]=='.')
   {fn[l]=0;break;}
}

char* local_S_replace(char* sz,int where,int len,const char* what)
{
 int szlen;
 int whlen; 
 szlen=(int)strlen(sz);
 if(what)
  whlen=(int)strlen(what);
 else
  whlen=0;
 if((szlen-(where+len)+1>0)&&(whlen!=len))
  memmove(sz+where+whlen,sz+where+len,szlen-(where+len)+1);
 if(whlen)
  memcpy(sz+where,what,whlen);
 return sz;
}

void local_adjustcmd(char*cmd)
{
const char*t=strstr(cmd,",");
if(t)
 {
  char piece[256];
  memcpy(piece,cmd,t-cmd);piece[t-cmd]=0;
  if(cIsIn(piece,"|s|"))
   local_S_replace(cmd,0,t-cmd,"say");
  else 
  if(cIsIn(piece,"|addL|addl|"))
   local_S_replace(cmd,0,t-cmd,"addline");  
 }
else
 {
     if(cIsIn(cmd,"|ifknow|ifk|ifK|iflearn|iflearnt|"))
      strcpy(cmd,"ifknown");
     else
     if(cIsIn(cmd,"|ifnotknow|ifnk|ifnK|ifnotlearn|ifnotlearnt|"))
      strcpy(cmd,"ifnotknown");
     else
     if(cIsIn(cmd,"|ifHT|ifht|"))
      strcpy(cmd,"iftaken");
     else
     if(cIsIn(cmd,"|ifnHT|ifnht|ifdHT|ifdht|"))
      strcpy(cmd,"ifnottaken");
     else
     if(cIsIn(cmd,"|ifH|ifh|"))
      strcpy(cmd,"ifhave");
     else
     if(cIsIn(cmd,"|ifnH|ifnh|ifdH|ifdh|"))
      strcpy(cmd,"ifdonthave");
     else
     if(cIsIn(cmd,"|end|"))
      strcpy(cmd,"endif");
     else
     if(cIsIn(cmd,"|s|"))
      strcpy(cmd,"say");
     else
     if(cIsIn(cmd,"|newD|newd|"))
      strcpy(cmd,"newdialog");
     else
     if(cIsIn(cmd,"|playD|playd|"))
      strcpy(cmd,"playdialog");
     else
     if(cIsIn(cmd,"|delD|deld|"))
      strcpy(cmd,"deletedialog");
     else
     if(cIsIn(cmd,"|addL|addl|"))
      strcpy(cmd,"addline");
     else
     if(cIsIn(cmd,"|endL|endl|"))
      strcpy(cmd,"endadd");
     else
     if(cIsIn(cmd,"|end|"))
      strcpy(cmd,"endif");
 }
}

int local_ionce=0,local_err=0;
int local_ioncebase;
int local_onces[8192];
char local_szbase[256]={0};

void szbase_normalize(char*sz)
{
 char w[256];
 int  i=0,j=0;
 for(i=0;sz[i];i++)
  if((sz[i]>='a')&&(sz[i]<='z'))
   w[j++]=sz[i];
  else
   if((sz[i]>='A')&&(sz[i]<='Z'))
     w[j++]=(sz[i]-'A')+'a';
 if(j>10)
  j=10;
 w[j]=0;
 strcpy(sz,w);
}

int local_getonce()
{
 return local_ioncebase*100+local_ionce++;
}
const char*local__getonce()
{
 static char o[256];
 sprintf(o,"once_%s_%d",local_szbase,local_getonce());
 return o;
}
int local_single_strstr(const char*wh,const char*what,char**pos)
 {
  const char*p=strstr(wh,what);
  if(p)
   {
    int l=strlen(what);
    if( ((p==wh)||!isalnum((unsigned char)p[-1])) && ((p[l]==0)||(!isalnum((unsigned char)p[l]))) )
    {
     if(pos) *pos=(char*)p;
     return 1;
    }
   }
  return 0;
 }

stringset local_macros;

char saycolors[][7]={"7FFFD4","FBCEB1","C0C0C0","FFFFF0","ABCDEF","F5F5DC","FFFEEF","F2F3F4","FAEBD7","FEFEE9","F8F8FF","FFFAF0","F5F5F5","FFDEAD","FFE4C4","F0F8FF","1E90FF","B0E0E6","F0DC82","D8BFD8","ACE1AF","99CBFF","00FFFF","FFF5EE","FFFDD0","DCDCDC","F7E89F","FFFF66",""};
int  isaycolors=0;
int gcnt=0,idx=1,unique=0;

void emitlua(lua_State* L,PFILE hf,int flag,int lv,char*map)
{
 char key[256]={0},val[8192]={0},e=0,err=0;
 char oncing[32]={0};
 int  l;
 getkeyval(hf,key,val);
 l=strlen(key);
 if(l>sizeof(key)-8)
  err++;
 l=strlen(val);
 if(l>sizeof(val)-8)
  err++;
 if(l&&(val[l-1]=='\t'))
  err++;
 if(lv>2)
 {
  int id=-1;
  local_adjustcmd(key);
  if(*key=='$')
   id=stringset_FindString(&local_macros,key);
  if(id!=-1)
   {
    char macro[4096];
    char vars[8][256];
    const char*t=val;
    int        it=0,r;
    while(t)
     t=S_gettokenex(t,vars[it++],',');
    stringset_GetTextEx(&local_macros,id,NULL,NULL,&macro[0],NULL); 
    for(r=0;r<it;r++)
     {
      char key[256];
      char*f;
      sprintf(key,"$%d",r+1);
      f=macro;
      while(f)
       { 
        f=strstr(f,key);
        if(f)
         local_S_replace(f,0,strlen(key),vars[r]);
       }
     }        
    t=macro;
    while(t)
    {
     char line[1024];
     t=S_getline(t,line);
     strcpy(hf->sz,line);
     hf->c=strlen(line);
     emitlua(L,hf,flag,lv,map);
    }
    //handle_cmd_line(hfT,texts,macros,constants,records,fakeline,bUseFakeline,bFT,locsect,title,lv,room,roomtemplate,macro); 
    return;
   }       
  if(cIsIn(key,"|addline,once|"))
   {    
    sprintf(oncing,"%s",local__getonce());
    sprintf(key,"addline,!%s",oncing);
   }
  else
   if(cIsIn(key,"|ifonce|")&&(*val==0))
    sprintf(val,"ifonce:%s",local__getonce());
   else
   {
    char*oncepos=NULL;
    if((memcmp(key,"addline",7)==0)&&local_single_strstr(key,"once",&oncepos))
     {
      char onc[256];
      sprintf(oncing,"%s",local__getonce());
      strcpy(onc,"!");strcat(onc,oncing);
      local_S_replace(oncepos,0,4,onc);
     }
    else
     if((memcmp(key,"if",2)==0)&&local_single_strstr(val,"once",&oncepos))
     {
      char onc[256];
      sprintf(oncing,"%s",local__getonce());
      strcpy(onc,"!");strcat(onc,oncing);
      local_S_replace(oncepos,0,4,onc);
     }
   }
 }
 //dmLogInfo("emit:%s:%d",hf->sz,unique++);
 if((lv==2)&&(flag&256))
  *val=0;
 if(lv>=1)
  {
   int l;
   for(l=16;l>=lv;l--)
    if(map[l]&1)
     {
#if !defined(ACOMPILER)
      //dmLogInfo("deletetable:%d",l);
#endif
      //dmLogInfo("lua_settable( L, -3 );","");
      lua_settable( L, -3 );            
      map[l]=0;
      gcnt--;
     }
   if(*key)
    {
     if(flag&1)
      {
       //dmLogInfo("flag:%s",key);

       lua_pushinteger ( L, idx++ );
       lua_newtable( L );

       lua_pushstring( L, key );
       lua_pushstring( L, val );
       lua_settable( L, -3 );
       e=1;

       lua_settable( L, -3 );

      }
     else
      {
      if(*val)
       {
        if(cIsIn(key,"|saycolor|")&&(*val=='_'))
        {
         strcpy(val,saycolors[isaycolors++]);
         if(*saycolors[isaycolors]==0)
          isaycolors=0;
        }
        else
        if(cIsIn(key,"|body_animset|head_animset|animset|shadow|")||strstr(key,"_propanimset"))
         {
          if(strcmp(key,"body_animset")==0)
           {
            int k;
            for(k=0;val[k];k++)
             if(val[k]==':')
              {
               char nm[256];
               strcpy(nm,val+k+1);
               strcpy(val,nm);
               break;
              }
           }
          else
           {
           int k;
           for(k=0;key[k];k++)
            if(key[k]=='_')
             {
              memcpy(val,key,k+1);val[k+1]=0;
              break;
             }
           }
          local_S_stripext(val);
         }
        /*dmLogInfo("lua_pushstring( L, \"%s\");",key);
        dmLogInfo("lua_pushstring( L, \"%s\");",val);
        dmLogInfo("lua_settable( L, -3 );",val);*/

        lua_pushstring( L, key );
        lua_pushstring( L, val );
        lua_settable( L, -3 );
       }
      else
       {
 #if !defined(ACOMPILER)
        //dmLogInfo("addtable:%s:%d",key,lv);
 #endif
       /*dmLogInfo("lua_pushstring( L, \"%s\");",key);
       dmLogInfo("lua_newtable(L);","");*/
       lua_pushstring ( L, key );
       lua_newtable( L );
       //lua_setglobal(L, key); 
       
       map[lv]|=1;
       gcnt++;
       }
      }
    }  
  } 
 if(e==0)
  idx=1;
 hf->c=0;hf->sz[0]=0;
 
 if(*oncing)
  {
   char line[1024];
   sprintf(line,"learn:%s",oncing);
   strcpy(hf->sz,line);
   hf->c=strlen(line);
   emitlua(L,hf,flag,lv,map);
  }
}

int treepad2txt(void*v,const char*p)
{
 lua_State* L=(lua_State*)v;
 const char*startp=p;
 char line[8192]; 
 char tied[32][64],prefix[32][64],head[256]={0};
 u8   usedtied[32];
 char lasttitle[256]={0};
 char tbl[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
 int  err=0,tlv=0,loc=0,constants=0,macros=0; 
 TFILE f;
 PFILE hf=&f;
 hf->c=0;hf->m=256;hf->sz=(char*)calloc(hf->m,1);

 isaycolors=0;
 local_ionce=local_err=local_ioncebase=0;
 *local_szbase=0;

 memset(tied,0,sizeof(tied));
 memset(prefix,0,sizeof(prefix));
 memset(usedtied,0,sizeof(usedtied));

 stringset_Create(&local_macros,8192,_STP_extradata);

 while(p&&*p)
  {
   p=S_getline(p,line);
   if(memcmp(line,"<node>",6)==0)
    {
     char title[2048],level[32];
     int  lv=0,emit,n,skip=0;
     p=S_getline(p,title);
     if(memcmp(title,"locations_",10)==0)
      {
       strcpy(head,title+10);strcat(head,"_");
       strcpy(title,"locations");
      }
     else
     if(memcmp(title,"scenes_",7)==0)
      {
       strcpy(head,title+7);strcat(head,"_");
       strcpy(title,"scenes");
      }
     else     
      {
       if(lv==1)
        *head=0;
       if(memcmp(title,"actors_",7)==0)
        strcpy(title,"actors");
      }
     p=S_getline(p,level);
     lv=atoi(level);

     if(lv==1)
     {
      if(strcmp(title,"constants")==0)
       constants=1;
      else
       constants=0;
      if(strcmp(title,"macros")==0)
       macros=1;
      else
       macros=0;
      if(strcmp(lasttitle,title)==0)
       skip=1;
      strcpy(lasttitle,title);

      if(0)
       if(cIsIn(title,"|locations|scenes|actors|objects|"))
        local_ioncebase++;
     }


     /*if(lv&&cIsIn(tied[lv-1],"|objclass|verbclass|"))
      STP_AddString(&sCLS,title);*/

     /*if(cIsIn(title,"|objclass|verbclass|"))     
      {
       int n;
       strcpy(prefix[lv],"");
       strcpy(tied[lv],title);usedtied[lv]=2;
       for(n=lv+1;n<sizeof(usedtied);n++)
        usedtied[n]=0;
       if(*p!='<')
        err++;
      } 
     else
     if((usedtied[lv-1]!=2)&&(cIsIn(title,"|include|verb|obj|room|")||cIsIn(title,"|scenery|sceneryobj|actor|genericverb|movementverb|normalobj|topics|")||(STP_FindString(&sCLS,title)!=-1)))     //scenery|sceneryobj|actor|genericverb|movementverb|normalobj|
      {
       int n;
       strcpy(prefix[lv],"");
       strcpy(tied[lv],title);usedtied[lv]=1;
       for(n=lv+1;n<sizeof(usedtied);n++)
        usedtied[n]=0;
       if(*p!='<')
        err++;
      } 
     else
     if((usedtied[lv-1]!=2)&&(memcmp(title,"room:",5)==0))
      {
       int n;
       strcpy(prefix[lv],title+5);
       strcpy(tied[lv],"room");       
       usedtied[lv]=1;
       for(n=lv+1;n<sizeof(usedtied);n++)
        usedtied[n]=0;
       if(*p!='<')
        err++;
      } 
     else*/
     if(lv==1)
      if(cIsIn(title,"|locations|scenes"))
       loc=256;
      else
       loc=0;

     if(lv==1)
      if(strcmp(title,"notes")==0)
       break;

     if(lv==2)
      {
       if(loc==256)
        {
         char sz[256];
         strcpy(sz,title);
         szbase_normalize(sz);
         if(strcmp(sz,local_szbase)==0)
          {
           strcpy(local_szbase,sz);
           local_ionce=0;
          }
        }
       else
        *local_szbase=0;
       if(loc==256)
        if(*head)
        {
         char ntitle[256];
         strcpy(ntitle,head);strcat(ntitle,title);
         strcpy(title,ntitle);
        }
      }

     if(skip)
      ;
     else
      { 
       int rl=0;
       for(n=0;n<lv;n++)
        if(usedtied[n])
         rl++;
       if(lv)
        {
         if(usedtied[lv-1])
          {
           const char*t=title;
           //lv=lv-1;
           while(t)
            {
             char nm[256];
             t=S_gettokenex(t,nm,'+');
             tabwrite(hf,lv-rl);
             F_writes(hf,tied[lv-1]);
             F_writes(hf,":");
             if(*prefix[lv-1])
              {
               F_writes(hf,prefix[lv-1]);
               F_writes(hf,"_");
              } 
             F_writes(hf,nm);
             F_writes(hf,"\r\n");
             emitlua(L,hf,loc|0,lv-rl,tbl);
            } 
           emit=1;
          }
         else
          { 
           tabwrite(hf,lv-rl);
           F_writes(hf,title);
           if(cIsIn(title,"|fulldesc|desc|it.desc|en.desc|")||(memcmp(title,"attr.",5)==0))
            {
             F_writes(hf,":");
             emit=0;
            } 
           else 
            {
             F_writes(hf,"\r\n");
             emitlua(L,hf,loc|0,lv-rl,tbl);
             emit=1;
            } 
          }          
        }
       if(macros&&(lv>1))
        {
         char macro[8192]={0};
         while(p&&*p)
          {
           p=S_getline(p,line);
           if(memcmp(line,"<end node>",10)==0)
            break;
           if(empty(line))
            continue;          
           strcat(macro,line);strcat(macro,"\r\n");
          }
         if(*macro)
          stringset_AddStringEx(&local_macros,title,-1,macro,strlen(macro)+1);
        }
       else
       while(p&&*p)
        {
         p=S_getline(p,line);
         if(memcmp(line,"<end node>",10)==0)
          break;
         if(empty(line))
          continue;          
         if(lv) 
          {
           if(emit)
            tabwrite(hf,lv+1-rl);
           //F_writes(hf,"+ ");
           F_writes(hf,line);
           F_writes(hf,"\r\n");
           emitlua(L,hf,1,lv+1-rl,tbl);

           emit=1;
          }
        }
      }  
    }  
  } 

 stringset_Destroy(&local_macros);
 
 emitlua(L,hf,0,1,tbl);

 free(hf->sz);
 return 1;
}