/*
 * path.h
 *
 *  Created on: Jul 2, 2021
 *      Author: Bas
 */

#ifndef MAIN_ESPLIB_OPENAPI_PATH_H_
#define MAIN_ESPLIB_OPENAPI_PATH_H_

#include <esp_system.h>
#include <esp_log.h>
#include <cJSON.h>
#include <type_traits>
#include <vector>
#include <map>
#include <string>
#include <esp_http_server.h>
#include "methods.h"

namespace Swagger
{
	class Path
	{
		std::vector<Method*> methods;

	public:
		std::string uri;

		void AddMethod(Method* m)
		{
			//TODO check if non duplicates!
			methods.push_back(m);
		}

		void AddToPaths(cJSON* paths)
		{
			cJSON *path = cJSON_CreateObject();
			cJSON_AddItemToObject(paths, uri.c_str(), path);

			for(int i=0; i<methods.size(); i++)
				methods[i]->AddToPath(path);
		}

	};

}



#endif /* MAIN_ESPLIB_OPENAPI_PATH_H_ */
