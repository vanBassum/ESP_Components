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
#include <nvs.h>


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

	void SaveToNVS(std::string partition, std::string _namespace)
	{
		nvs_handle my_handle;
		esp_err_t err = nvs_open_from_partition(partition.c_str(), _namespace.c_str(), NVS_READWRITE, &my_handle);
		if (err != ESP_OK)
		{
			ESP_LOGE("PropertyContainer", "Error (%s) opening NVS handle, Settings not saved!", esp_err_to_name(err));
		} else
		{
			SaveToNVS(my_handle, "");
			err = nvs_commit(my_handle);
			if(err == ESP_OK)
				ESP_LOGI("PropertyContainer", "Settings saved");
			else
				ESP_LOGE("PropertyContainer", "Error (%s) committing nvs, Settings not saved!", esp_err_to_name(err));
			nvs_close(my_handle);
		}
	}

	void LoadFromNVS(std::string partition, std::string _namespace)
	{
		nvs_handle my_handle;
		esp_err_t err = nvs_open_from_partition(partition.c_str(), _namespace.c_str(), NVS_READWRITE, &my_handle);
		if (err != ESP_OK)
		{
			ESP_LOGE("PropertyContainer", "Error (%s) opening NVS handle! Loading default settings", esp_err_to_name(err));
		} else
		{
			LoadFromNVS(my_handle, "");
			ESP_LOGI("PropertyContainer", "Settings loaded");
			nvs_close(my_handle);
		}
	}

	void SaveToNVS(nvs_handle handle, std::string path)
	{

		for(int i=0; i<properties.size(); i++)
		{
			PropertyBase* property = properties[i];
			property->SaveToNVS(handle, path);
		}

	}

	void LoadFromNVS(nvs_handle handle, std::string path)
	{
		for(int i=0; i<properties.size(); i++)
		{
			PropertyBase* property = properties[i];
			property->LoadFromNVS(handle, path);
		}
	}

};


#endif /* MAIN_ESPLIB_MISC_PROPERTYCONTAINER_H_ */
