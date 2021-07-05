/*
 * definition.h
 *
 *  Created on: Jul 5, 2021
 *      Author: Bas
 */

#ifndef MAIN_ESPLIB_OPENAPI_DEFINITION_H_
#define MAIN_ESPLIB_OPENAPI_DEFINITION_H_

#include <esp_log.h>
#include <cJSON.h>
#include <map>
#include <string>


namespace Swagger
{
	class Definition
	{
		std::map<std::string, std::string> simpleProperties;
		std::map<std::string, std::string> referenceProperties;

	public:
		std::string name;

		void AddSimpleProp(std::string name, std::string type)
		{
			simpleProperties[name] = type;
		}

		void AddRefProp(std::string name, std::string ref)
		{
			referenceProperties[name] = ref;
		}

		void AddToDefinitions(cJSON* definitions)
		{
			cJSON* pc = cJSON_CreateObject();
			cJSON_AddItemToObject(definitions, name.c_str(), pc);
			cJSON_AddStringToObject(pc, "type", "object");

			cJSON* props = cJSON_CreateObject();
			cJSON_AddItemToObject(pc, "properties", props);

			for(auto x : simpleProperties)
			{
				cJSON* prop = cJSON_CreateObject();
				cJSON_AddStringToObject(prop, "type", x.second.c_str());
				cJSON_AddItemToObject(props, x.first.c_str(), prop);
			}

			for(auto x : referenceProperties)
			{
				cJSON* prop = cJSON_CreateObject();
				cJSON_AddStringToObject(prop, "$ref", ("#/definitions/" + x.second).c_str());
				cJSON_AddItemToObject(props, x.first.c_str(), prop);
			}
		}

		void AddAsReference(cJSON* schema)
		{
			cJSON_AddStringToObject(schema, "$ref", ("#/definitions/" + name).c_str());
		}
	};


}





#endif /* MAIN_ESPLIB_OPENAPI_DEFINITION_H_ */
