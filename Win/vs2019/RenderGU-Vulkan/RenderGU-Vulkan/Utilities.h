#pragma once

// Indices (locations) of Queue Families
struct QueueFamilyIndices
{
	int graphicsFamily = -1;

	bool isValid()
	{
		return graphicsFamily >= 0;
	}
};

