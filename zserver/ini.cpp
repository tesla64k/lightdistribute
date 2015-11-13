#include "ini.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
// 
// char*    getKeyValu(char *filename, char *section, char *key)
// {
// 	char line[255];
// 	char sectname[255];
// 	char *skey, *valu;
// 	char seps[] = "=";
// 	bool flag = false;
// 	FILE *fp;
// 	fopen_s(&fp,filename, "r");
// 	assert(fp != NULL);
// 	//process the section and line
// 	memset(line, 0, 255);
// 	if (!strchr(section, '['))
// 	{
// 		strcpy_s(sectname, "[");
// 		strcat_s(sectname, section);
// 		strcat_s(sectname, "]");
// 	}
// 	else
// 	{
// 		strcpy_s(sectname, section);
// 	}
// 
// 
// 	while (fgets(line, 255, fp) != NULL)
// 	{
// 		//delete the  newline
// 		valu = strchr(line, '\n');
// 		*valu = 0;
// 
// 		if (flag)
// 		{
// 			skey = strtok_s(line, seps);
// 			if (strcmp(skey, key) == 0)
// 			{
// 				return strtok_s(NULL, seps);
// 			}
// 		}
// 		else
// 		{
// 			if (strcmp(sectname, line) == 0)
// 			{
// 				flag = true;  //find the section 
// 			}
// 
// 		}
// 	}
// 	return NULL;
// }