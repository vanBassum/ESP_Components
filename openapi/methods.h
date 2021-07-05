/*
 * methods.h
 *
 *  Created on: Jul 5, 2021
 *      Author: Bas
 */

#ifndef MAIN_ESPLIB_OPENAPI_METHODS_H_
#define MAIN_ESPLIB_OPENAPI_METHODS_H_

#include <esp_system.h>
#include <esp_log.h>
#include <cJSON.h>
#include <esp_http_server.h>
#include <type_traits>
#include <vector>
#include <map>
#include <string>
#include "definition.h"
#include "../misc/propertycontainer.h"

namespace Swagger
{

	class Method
	{
	public:
		virtual ~Method(){}
		virtual esp_err_t ExecuteMethod(httpd_req_t *req) = 0;
		virtual void AddToPath(cJSON* path) = 0;
	};


	template<typename T>
	class GetMethod : public Method
	{
	public:
		Definition* result = NULL;

		T (*func)();

		esp_err_t ExecuteMethod(httpd_req_t *req)
		{

			T t = func();
			PropertyContainer* descriptor = static_cast<PropertyContainer*>(&t);
			cJSON* json = descriptor->ToJSON();
			char *bb = cJSON_Print(json);
			httpd_resp_send(req, bb, HTTPD_RESP_USE_STRLEN);
			free(bb);
			cJSON_Delete(json);
			return ESP_OK;
		}

		void AddToPath(cJSON* path)
		{
			cJSON *get = cJSON_CreateObject();
			cJSON_AddItemToObject(path, "get", get);
			cJSON *responses = cJSON_CreateObject();
			cJSON_AddItemToObject(get, "responses", responses);
			cJSON *r200 = cJSON_CreateObject();
			cJSON_AddItemToObject(responses, "200", r200);
			cJSON *schema = cJSON_CreateObject();
			cJSON_AddItemToObject(r200, "schema", schema);
			cJSON_AddStringToObject(r200, "description", "descr");
			result->AddAsReference(schema);
		}
	};



	template<typename T>
	class PostMethod : public Method
	{
	public:
		Definition* argument = NULL;

		bool (*func)(T argument);

		esp_err_t ExecuteMethod(httpd_req_t *req)
		{
			cJSON *json = NULL;
			char* content = (char*) malloc(req->content_len);
			if(content != NULL)
			{
				int ret = httpd_req_recv(req, content, req->content_len);

				if (ret <= 0)
				{
					if (ret == HTTPD_SOCK_ERR_TIMEOUT)
						httpd_resp_send_408(req);
					return ESP_FAIL;
				}
				else
				{
					json = cJSON_Parse(content);
					free(content);
				}
			}


			if(json != NULL)
			{
				//Parse the JSON
				T t;
				PropertyContainer* pc = static_cast<PropertyContainer*>(&t);
				pc->FromJSON(json);
				cJSON_Delete(json);
				bool result = func(t);
				if(result)
				{
					httpd_resp_set_status(req, "201 OK");
					httpd_resp_send(req, "Success", HTTPD_RESP_USE_STRLEN);
				}
				else
				{
					httpd_resp_set_status(req, "202 NOK");
					httpd_resp_send(req, "Fail", HTTPD_RESP_USE_STRLEN);
				}
				return ESP_OK;
			}
			return ESP_FAIL;
		}

		void AddToPath(cJSON* path)
		{
			cJSON *post 		= cJSON_AddObjectToObject(path, "post");
			cJSON *parameters 	= cJSON_AddArrayToObject(post, "parameters");


			cJSON *parameter 	= cJSON_CreateObject();
			cJSON_AddItemToArray(parameters, parameter);

			cJSON_AddStringToObject(parameter, "in", "body");
			cJSON_AddStringToObject(parameter, "name", argument->name.c_str());
			cJSON *schema 		= cJSON_AddObjectToObject(parameter, "schema");
			argument->AddAsReference(schema);

			cJSON *responses 	= cJSON_AddObjectToObject(post, "responses");
			cJSON *r201 		= cJSON_AddObjectToObject(responses, "201");
			cJSON_AddStringToObject(r201, "description", "Success");

			cJSON *r202 		= cJSON_AddObjectToObject(responses, "202");
			cJSON_AddStringToObject(r202, "description", "Fail");
		}
	};


}


#endif /* MAIN_ESPLIB_OPENAPI_METHODS_H_ */
