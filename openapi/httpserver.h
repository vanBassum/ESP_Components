/*
 * httpserver.h
 *
 *  Created on: Jun 29, 2021
 *      Author: Bas
 */

#ifndef MAIN_HTTPSERVER_H_
#define MAIN_HTTPSERVER_H_

#include <esp_http_server.h>
#include <string>
#include <vector>
#include "../misc/callback.h"





size_t MIN(size_t a, size_t b)
{
	if(a<b)
		return a;
	else
		return b;
}



class HTTPServer
{
	std::vector<httpd_uri_t> httpUris;
	httpd_handle_t handle = NULL;

	static esp_err_t CallbackInvokeHandler(httpd_req_t *req)
	{
		Callback<esp_err_t, httpd_req_t*> *callback = (Callback<esp_err_t, httpd_req_t*>*) req->user_ctx;
		return callback->Invoke(req);
	}

public:

	~HTTPServer()
	{
		//TODO: Iterate over httpUris, unregister and delete callback!

		StopServer();
	}

	void AddHandler(std::string uri, http_method method, Callback<esp_err_t, httpd_req_t*>* callback)
	{
		void* context = (void*)callback;
		httpd_uri_t httpUri =
		{
			.uri      = uri.c_str(),
			.method   = method,
			.handler  = &CallbackInvokeHandler,
			.user_ctx = context
		};

		ESP_LOGI("httpserver", "AddHandler '%s'", httpUri.uri);

		httpUris.push_back(httpUri);
		if(handle != NULL)
			httpd_register_uri_handler(handle, &httpUri);
	}

	template<typename T>
	void AddHandler(std::string uri, http_method httpMethod, T* instance, esp_err_t (T::*method)(httpd_req_t*))
	{
		Callback<esp_err_t, httpd_req_t*> *callback = new Callback<esp_err_t, httpd_req_t*>();
		callback->Bind(instance, method);
		AddHandler(uri, httpMethod, callback);
	}


	void AddHandler(std::string uri, http_method httpMethod, esp_err_t (*method)(httpd_req_t*))
	{
		Callback<esp_err_t, httpd_req_t*> *callback = new Callback<esp_err_t, httpd_req_t*>();
		callback->Bind(method);
		AddHandler(uri, httpMethod, callback);
	}




	void StartServer()
	{
	    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
	    handle = NULL;
	    if (httpd_start(&handle, &config) == ESP_OK)
	    {
	    	for(int i=0; i<httpUris.size(); i++)
	    		httpd_register_uri_handler(handle, &httpUris[i]);
	    }
	}


	void StopServer()
	{
	    if (handle)
	        httpd_stop(handle);
	}


};


#endif /* MAIN_HTTPSERVER_H_ */
