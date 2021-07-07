/*
 * OpenApi.h
 *
 *  Created on: Jun 30, 2021
 *      Author: Bas
 */

#ifndef MAIN_ESPLIB_OPENAPI_OPENAPI_H_
#define MAIN_ESPLIB_OPENAPI_OPENAPI_H_


#include <esp_system.h>
#include <esp_log.h>
#include <esp_http_server.h>
#include <cJSON.h>
#include <type_traits>
#include <vector>
#include <map>
#include <string>
#include "httpserver.h"
#include "swaggerui.h"
#include "../misc/callback.h"
#include "path.h"
#include "../misc/property.h"
#include "../misc/propertycontainer.h"

namespace Swagger
{
	class OpenAPI
	{
		std::vector<Path*> paths;
		std::vector<Definition*> definitions;
		HTTPServer server;
		cJSON* swagger = NULL;

		esp_err_t GetSwaggerUI(httpd_req_t *req)
		{
			ESP_LOGI("OpenAPI", "GetSwaggerUI");
			std::string jsonUrl = std::string(req->uri) + std::string("swagger.json");
			std::string swagUi = GetSwaggerUIHTML(jsonUrl);

			httpd_resp_send(req, swagUi.c_str(), HTTPD_RESP_USE_STRLEN);
			return ESP_OK;
		}

		esp_err_t GetSwaggerJSON(httpd_req_t *req)
		{
			char *bb = cJSON_Print(swagger);
			httpd_resp_send(req, bb, HTTPD_RESP_USE_STRLEN);
			free(bb);
			return ESP_OK;
		}

	public:
		OpenAPI()
		{
			server.StartServer();
			server.AddHandler("/", 				HTTP_GET, this, &OpenAPI::GetSwaggerUI);
			server.AddHandler("/swagger.json", 	HTTP_GET, this, &OpenAPI::GetSwaggerJSON);
		}

		void GenerateSwaggerJSON()
		{
			ESP_LOGI("OpenAPI", "GetSwaggerJSON");
			swagger = cJSON_CreateObject();
			cJSON_AddStringToObject(swagger, "swagger", "2.0");

			cJSON *info = cJSON_CreateObject();
			cJSON_AddStringToObject(info, "title", "Some title");
			cJSON_AddStringToObject(info, "version", "1.0.0");

			cJSON_AddItemToObject(swagger, "info", info);
			cJSON *jPaths = cJSON_CreateObject();

			for (auto x : paths)
			{
				x->AddToPaths(jPaths);
			}
			cJSON_AddItemToObject(swagger, "paths", jPaths);

			//Add definitions

			cJSON *jDefinitions = cJSON_CreateObject();

			for (auto x : definitions)
			{
				x->AddToDefinitions(jDefinitions);
			}

			cJSON_AddItemToObject(swagger, "definitions", jDefinitions);
		}

		Path* GetPath(std::string uri)
		{
			Path* p = NULL;
			for(auto x : paths)
			{
				if(x->uri == uri)
				{
					p = x;
					break;
				}
			}

			if(p == NULL)
			{
				p = new Path();
				p->uri = uri;
				paths.push_back(p);
			}
			return p;
		}


		Definition* GetDefinition(PropertyContainer* pc)
		{
			Definition* d = NULL;
			for(auto x : definitions)
			{
				if(x->name == pc->GetName())
				{
					d = x;
					break;
				}
			}
			if(d == NULL)
			{
				d = new Definition();
				d->name = pc->GetName();
				definitions.push_back(d);
				for(auto x : pc->properties)
				{
					switch(x->GetType())
					{
					case Types::UINT8_t:	d->AddSimpleProp(x->Name, "integer"); break;
					case Types::UINT16_t:	d->AddSimpleProp(x->Name, "integer"); break;
					case Types::UINT32_t:	d->AddSimpleProp(x->Name, "integer"); break;
					case Types::UINT64_t:	d->AddSimpleProp(x->Name, "integer"); break;
					case Types::INT8_t:		d->AddSimpleProp(x->Name, "integer"); break;
					case Types::INT16_t:	d->AddSimpleProp(x->Name, "integer"); break;
					case Types::INT32_t:	d->AddSimpleProp(x->Name, "integer"); break;
					case Types::INT64_t:	d->AddSimpleProp(x->Name, "integer"); break;
					case Types::FLOAT:		d->AddSimpleProp(x->Name, "number" ); break;
					case Types::DOUBLE:		d->AddSimpleProp(x->Name, "number" ); break;
					case Types::STRING:		d->AddSimpleProp(x->Name, "string" ); break;
					case Types::PROPCONT:
					{
						Prop<PropertyContainer>* prop = reinterpret_cast<Prop<PropertyContainer>*>(&x);

						PropertyContainer nestedPC = prop->Get();
						Definition* nestedD = GetDefinition(&nestedPC);
						d->AddRefProp(x->Name, nestedD->name);
						break;
					}
					default:
						break;
					}
				}
			}
			return d;
		}



		template<typename T>
		void RegisterGetFunction(std::string uri, T(*func)())
		{
			T t;
			PropertyContainer* pc = static_cast<PropertyContainer*>(&t);
			Path* p = GetPath(uri);

			GetMethod<T>* method = new GetMethod<T>();
			method->func = func;
			method->result = GetDefinition(pc);
			p->AddMethod(method);

			server.AddHandler(uri.c_str(), HTTP_GET, method, &GetMethod<T>::ExecuteMethod);
		}

		template<typename T>
		void RegisterPostFunction(std::string uri, bool(*func)(T arg))
		{
			T t;
			PropertyContainer* pc = static_cast<PropertyContainer*>(&t);
			Path* p = GetPath(uri);

			PostMethod<T>* method = new PostMethod<T>();
			method->func = func;
			method->argument = GetDefinition(pc);
			p->AddMethod(method);

			server.AddHandler(uri.c_str(), HTTP_POST, method, &PostMethod<T>::ExecuteMethod);
		}


	};
}







#endif /* MAIN_ESPLIB_OPENAPI_OPENAPI_H_ */
