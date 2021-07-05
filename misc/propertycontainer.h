/*
 * propertycontainer.h
 *
 *  Created on: Jul 5, 2021
 *      Author: Bas
 */

#ifndef MAIN_ESPLIB_MISC_PROPERTYCONTAINER_H_
#define MAIN_ESPLIB_MISC_PROPERTYCONTAINER_H_

#include <cJSON.h>
#include <vector>
#include "propertybase.h"


class PropertyContainer		//Cant contain abstracts, is_convertible doenst work otherwise
{
public:
	std::vector<PropertyBase*> properties;

	virtual ~PropertyContainer(){}


	void AddProperty(PropertyBase* property)
	{
		properties.push_back(property);
	}

	virtual std::string GetName()
	{
		return "NoName";
	}

	cJSON* ToJSON()
	{
		cJSON* root = cJSON_CreateObject();
		for(int i=0; i<properties.size(); i++)
		{
			PropertyBase* property = properties[i];
			cJSON *propJSON = property->ToJSON();
			if(propJSON != NULL)
				cJSON_AddItemToObject(root, property->Name.c_str(), propJSON);
		}
		return root;
	}

	void FromJSON(cJSON* json)
	{
		for(int i=0; i<properties.size(); i++)
		{
			PropertyBase* property = properties[i];
			cJSON *propJSON = cJSON_GetObjectItem(json, property->Name.c_str());
			property->FromJSON(propJSON);
		}
	}

};


#endif /* MAIN_ESPLIB_MISC_PROPERTYCONTAINER_H_ */
