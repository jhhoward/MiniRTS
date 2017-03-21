#include "Resource.h"
#include "Pathing.h"

uint8_t ResourceState[MAX_RESOURCES >> 1];

uint8_t Resource_GetRemaining(EntityID id)
{
	if(id.type == Entity_Resource && id.id < MAX_RESOURCES)
	{
		uint8_t state = ResourceState[id.id >> 1];
		
		if(id.id & 1)
		{
			return state >> 4;
		}
		else
		{
			return state & 0xf;
		}
	}
	
	return 0;	
}

void Resource_ReduceCount(EntityID id)
{
	if(id.type == Entity_Resource && id.id < MAX_RESOURCES)
	{
		uint8_t index = id.id >> 1;
		uint8_t state = ResourceState[index];
		
		if(id.id & 1)
		{ 
			if(state & 0xf0)
			{
				ResourceState[index] = (state & 0x0f) & ((state & 0xf0) - 0x10);
			}
		}
		else
		{
			if(state & 0x0f)
			{
				ResourceState[index] = (state & 0xf0) & ((state & 0x0f) - 0x01);
			}
		}
	}
}

EntityID Resource_GetAtLocation(uint8_t x, uint8_t y)
{
	EntityID found;
	found.value = 0xff;
	
	uint8_t resourceIndex = 0;
	uint8_t* locationPtr = ResourceLocations;
	
	while(resourceIndex < MAX_RESOURCES)
	{
		if(x == locationPtr[0] && y == locationPtr[1])
		{
			found.id = resourceIndex;
			found.type = Entity_Resource;
			return found;
		}
		
		locationPtr += 2;
		resourceIndex++;
	}
	
	return found;
}

EntityID Resource_FindClosest(uint8_t x, uint8_t y, uint8_t maxDistance)
{
	uint8_t index;
	EntityID result;
	
	result.value = INVALID_ENTITY_VALUE;
	int closestDistance = 0;
	
	while(index < MAX_RESOURCES)
	{
		EntityID check;
		check.type = Entity_Resource;
		check.id = index;
		
		if(Resource_GetRemaining(check))
		{
			uint8_t checkX, checkY;
			
			Resource_GetLocation(check, &checkX, &checkY);
			
			int distance = Path_CalculateDistance(x, y, checkX, checkY);
			
			if(distance < maxDistance)
			{				
				if(result.value == INVALID_ENTITY_VALUE || distance < closestDistance)
				{
					result.value = check.value;
				}
			}
		}
		
		index++;
	}
	
	return result;
}