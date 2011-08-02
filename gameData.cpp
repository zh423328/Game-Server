#include "Global.h"

//equipableClassEquipmentSlot.txt

HashTable_uint32_t ht_equipmentClassSlots;
HashTable_uint32_t ht_starterItemTemplateClassIds;


void _gameData_loadEquipmentClassSlots()
{
	hashTable_init(&ht_equipmentClassSlots, 128);
	file_t *file = fileMgr_open("gameData\\equipableClassEquipmentSlot.txt");
	char *line;
	while( line = fileMgr_readLine(file) )
	{
		if( line[0] == '(' )
		{
			int classId;
			int slotId;
			if( sscanf(line, "(%d, %d)", &classId, &slotId) != 2 )
			{
				__debugbreak();
			}
			hashTable_set(&ht_equipmentClassSlots, classId, (void*)slotId);
		}
		free(line);
	}
	fileMgr_close(file);
}

void _gameData_loadStarterItemTemplateClassIds()
{
	hashTable_init(&ht_starterItemTemplateClassIds, 128);
	file_t *file = fileMgr_open("gameData\\starterItemTemplateClassIds.txt");
	char *line;
	while( line = fileMgr_readLine(file) )
	{
		if( line[0] == '(' )
		{
			int templateId;
			int classId;
			if( sscanf(line, "(%d, %d)", &templateId, &classId) != 2 )
			{
				__debugbreak();
			}
			hashTable_set(&ht_starterItemTemplateClassIds, templateId, (void*)classId);
		}
		free(line);
	}
	fileMgr_close(file);
}

int mapInfoCount;
gameData_mapInfo_t *mapInfoArray;

void _gameData_loadMapInfo()
{
	// read number of maps
	mapInfoCount = 0;
	file_t *file = fileMgr_open("gameData\\mapInfo.txt");
	char *line;
	while( line = fileMgr_readLine(file) )
	{
		if( line[0] == '(' )
		{
			int contextId;
			char mapName[256];
			int version;
			int baseRegionId;
			if( sscanf(line, "(%d, '%[^']', %d, %d)", &contextId, mapName, &version, &baseRegionId) == 4 )
				mapInfoCount++;
		}
		free(line);
	}
	fileMgr_close(file);
	// allocate maps
	mapInfoArray = (gameData_mapInfo_t*)malloc(sizeof(gameData_mapInfo_t)*mapInfoCount);
	// #(contextId, 'mapName', version, baseRegionId)
	file = fileMgr_open("gameData\\mapInfo.txt");
	int i=0;
	while( line = fileMgr_readLine(file) )
	{
		if( line[0] == '(' )
		{
			int contextId;
			char mapName[256];
			int version;
			int baseRegionId;
			if( sscanf(line, "(%d, '%[^']', %d, %d)", &contextId, mapName, &version, &baseRegionId) == 4 )
			{
				mapInfoArray[i].contextId = contextId;
				strcpy(mapInfoArray[i].name, mapName);
				mapInfoArray[i].version = version;
				mapInfoArray[i].baseRegionId = baseRegionId;
				i++;
			}
		}
		free(line);
	}
	fileMgr_close(file);
}


int gameData_getEquipmentClassIdSlot(int classId)
{
	return (int)hashTable_get(&ht_equipmentClassSlots, classId);	
}

int gameData_getStarterItemTemplateClassId(int templateId)
{
	return (int)hashTable_get(&ht_starterItemTemplateClassIds, templateId);	
}

HashTable_uint32_t ht_itemTemplateById;
HashTable_string_t ht_itemTemplateByName;

itemTemplate_t *gameData_getItemTemplateById(unsigned int templateId)
{
	return (itemTemplate_t*)hashTable_get(&ht_itemTemplateById, templateId);	
}

itemTemplate_t *gameData_getItemTemplateByName(char *name)
{
	return (itemTemplate_t*)hashTable_get(&ht_itemTemplateByName, name);	
}


void _gameData_loadItemTemplates()
{
	hashTable_init(&ht_itemTemplateById, 128);
	hashTable_init(&ht_itemTemplateByName, 128);
	//sData_t *sData_open(char *path);
	//bool sData_nextCategory(sData_t *sData);
	//char *sData_currentCategoryName(sData_t *sData);
	//char *sData_findOption(sData_t *sData, char *optionName);
	sData_t *it = sData_open("gameData\\ItemTemplates.txt");
	int z = 0;
	while( sData_nextCategory(it) )
	{
		z++;
		if( z >= 128 )
		{
			printf("Too many item templates :(\n");
			Sleep(10000);
			ExitProcess(-1);
		}
		char *catName = sData_currentCategoryName(it);
		printf("it: %s\n", catName);

		char *s_ClassId = sData_findOption(it, "classId");
		char *s_templateId = sData_findOption(it, "templateId");
		if( !s_ClassId || !s_templateId )
		{
			printf("ClassID or TemplateID missing at [%s]\n", catName);
			Sleep(10000);
			ExitProcess(-1);
		}

		itemTemplate_t *itemTemplate = (itemTemplate_t*)malloc(sizeof(itemTemplate_t));
		itemTemplate->classId = atoi(s_ClassId);
		itemTemplate->templateId = atoi(s_templateId);
		// get other states
		char *s_type = sData_findOption(it, "type");
		if( s_type == NULL )
		{
			printf("'type' not set for [%s]\n", catName);
			Sleep(10000);
			ExitProcess(-1);
		}
		if( strcmp(s_type, "WEAPON")==0 )
		{
			itemTemplate->type = ITEMTYPE_WEAPON;
		}
		else if( strcmp(s_type, "ARMOR")==0 )
		{
			itemTemplate->type = ITEMTYPE_ARMOR;
		}
		else
		{
			printf("unknown type for [%s]\n", catName);
			Sleep(10000);
			ExitProcess(-1);
		}


		// set
		hashTable_set(&ht_itemTemplateById, itemTemplate->templateId, itemTemplate);
		hashTable_set(&ht_itemTemplateByName, catName, itemTemplate);
	}
	sData_close(it);
}




void gameData_load()
{
	_gameData_loadEquipmentClassSlots();
	_gameData_loadStarterItemTemplateClassIds();
	_gameData_loadMapInfo();
	_gameData_loadItemTemplates();
}