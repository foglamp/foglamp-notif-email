/*
 * FogLAMP "email" notification plugin.
 *
 * Copyright (c) 2018 Dianomic Systems
 *
 * Released under the Apache 2.0 Licence
 *
 * Author: Amandeep Singh Arora
 */

#include <string>
#include <plugin_api.h>
#include <config_category.h>
#include <filter.h>
#include <email_config.h>

#define EMAIL_CFG "\"email_from\":\"alerts@dianomic.com\",\"email_from_name\":\"Notification alert\",\"email_to\":\"alerts@dianomic.com\",\"email_to_name\":\"Notification alert subscriber\",\"server\":\"smtp.gmail.com\",\"port\":587,\"subject\":\"Foglamp alert notification\",\"messageId\":\"dcd7cb36-11db-487a-9f3a-e652a9458efd@dianomic.com\",\"use_ssl_tls\":true,\"username\":\"user\",\"password\":\"pass\""

#define PLUGIN_NAME "email"

#define DEFAULT_CONFIG "{\"plugin\" : { \"description\" : \"Email notification plugin\", " \
                       		"\"type\" : \"string\", " \
				"\"default\" : \"" PLUGIN_NAME "\" }, " \
			 "\"enable\": {\"description\": \"A switch that can be used to enable or disable execution of " \
					 "the email notification plugin.\", " \
				"\"type\": \"boolean\", " \
				"\"default\": \"false\" }, " \
			"\"config\" : {\"description\" : \"Email notification plugin configuration.\", " \
				"\"type\" : \"JSON\", " \
				"\"default\" : {" EMAIL_CFG "}} }"

using namespace std;

/**
 * The Notification plugin interface
 */
extern "C" {

/**
 * The plugin information structure
 */
static PLUGIN_INFORMATION info = {
        PLUGIN_NAME,              // Name
        "1.0.0",                  // Version
        0,                        // Flags
        PLUGIN_TYPE_NOTIFICATION_DELIVERY,       // Type
        "1.0.0",                  // Interface version
        DEFAULT_CONFIG	          // Default plugin configuration
};

typedef struct
{
	FogLampFilter *handle;
	EmailCfg emailCfg;
} PLUGIN_INFO;

extern int sendEmailMsg(const EmailCfg *emailCfg, const char *msg);

/**
 * Return the information about this plugin
 */
PLUGIN_INFORMATION *plugin_info()
{
	return &info;
}

/**
 * Reset/init EmailCfg structure
 */
void resetConfig(EmailCfg *emailCfg)
{
	emailCfg->email_from = "";
	emailCfg->email_from_name = "";
	emailCfg->email_to = "";
	emailCfg->email_to_name = "";
	emailCfg->server = "";
	emailCfg->port = 0;
	emailCfg->subject = "";
	emailCfg->messageId = "";
	emailCfg->use_ssl_tls = false;
	emailCfg->username = "";
	emailCfg->password = "";
}

/**
 * Fill EmailCfg structure from JSON document representing plugin config
 */
void parseConfig(Document *document, EmailCfg *emailCfg)
{
	for (Value::ConstMemberIterator itr = document->MemberBegin(); itr != document->MemberEnd(); ++itr)
	{
		if (itr->name.GetString() == "email_from" && itr->value.IsString())
		{
			emailCfg->email_from = itr->value.GetString();
		}
		else if (itr->name.GetString() == "email_from_name" && itr->value.IsString())
		{
			emailCfg->email_from_name= itr->value.GetString();
		}
		else if (itr->name.GetString() == "email_to" && itr->value.IsString())
		{
			emailCfg->email_to = itr->value.GetString();
		}
		else if (itr->name.GetString() == "email_to_name" && itr->value.IsString())
		{
			emailCfg->email_to_name = itr->value.GetString();
		}
		else if (itr->name.GetString() == "server" && itr->value.IsString())
		{
			emailCfg->server = itr->value.GetString();
		}
		else if (itr->name.GetString() == "port" && itr->value.IsNumber())
		{
			emailCfg->port = (unsigned int) itr->value.GetInt();
		}
		else if (itr->name.GetString() == "subject" && itr->value.IsString())
		{
			emailCfg->subject = itr->value.GetString();
		}
		else if (itr->name.GetString() == "messageId" && itr->value.IsString())
		{
			emailCfg->messageId = itr->value.GetString();
		}
		else if (itr->name.GetString() == "use_ssl_tls" && itr->value.IsBool())
		{
			emailCfg->use_ssl_tls = itr->value.IsTrue();
		}
		else if (itr->name.GetString() == "username" && itr->value.IsString())
		{
			emailCfg->username = itr->value.GetString();
		}
		else if (itr->name.GetString() == "password" && itr->value.IsString())
		{
			emailCfg->password = itr->value.GetString();
		}
		else
		{
			Logger::getLogger()->error("Unable to parse/handle value for config field '%s', skipping...", itr->name.GetString());
		}
	}
}

/**
 * Initialise the plugin, called to get the plugin handle and setup the
 * plugin configuration
 *
 * @param config	The configuration category for the plugin
 * @return		An opaque handle that is used in all subsequent calls to the plugin
 */
PLUGIN_HANDLE plugin_init(ConfigCategory* config,
								OUTPUT_HANDLE *outHandle,
								OUTPUT_STREAM output)
{
	PLUGIN_INFO *info = new PLUGIN_INFO;
	info->handle = new FogLampFilter(PLUGIN_NAME, *config, outHandle, output);
	FogLampFilter *filter = info->handle;
	
	// Handle plugin configuration
	if (filter->getConfig().itemExists("config"))
	{
		Document	document;
		if (document.Parse(filter->getConfig().getValue("config").c_str()).HasParseError())
		{
			Logger::getLogger()->error("Unable to parse email plugin config: '%s'", filter->getConfig().getValue("config").c_str());
			return NULL;
		}
		Logger::getLogger()->info("Email plugin config=%s", filter->getConfig().getValue("config").c_str());

		resetConfig(&info->emailCfg);
		parseConfig(&document, &info->emailCfg);
		
		if (info->emailCfg.email_to == "" || info->emailCfg.server == "" || info->emailCfg.port == 0)
		{
			Logger::getLogger()->error("Config for email notification plugin is incomplete, exiting...");
			delete info->handle;
			delete info;
			info = NULL;
		}
	}
	else
	{
		Logger::getLogger()->info("No config provided for email plugin, exiting...");
		delete info->handle;
		delete info;
		info = NULL;
	}
	
	return (PLUGIN_HANDLE)info;
}

/**
 * Ingest a set of readings into the plugin for processing
 *
 * @param handle	The plugin handle returned from plugin_init
 * @param readingSet	The readings to process
 */
void plugin_deliver(PLUGIN_HANDLE *handle,
		   std::string message)
{
	Logger::getLogger()->info("Email notification plugin: plugin_deliver()");
	PLUGIN_INFO *info = (PLUGIN_INFO *) handle;
	FogLampFilter *filter = info->handle;
	
	if (!filter->isEnabled())
	{
		// Current plugin is not active: nothing to do
		return;
	}
	
	int rv = sendEmailMsg(&info->emailCfg, message.c_str());
	if (rv)
		Logger::getLogger()->info("sendEmailMsg() returned %d", rv);
}

/**
 * Reconfigure the plugin
 */
void plugin_reconfigure(PLUGIN_HANDLE *handle, string& newConfig)
{
	Logger::getLogger()->info("Email notification plugin: plugin_reconfigure()");
	PLUGIN_INFO *info = (PLUGIN_INFO *) handle;
	
	Document document;
	if (document.Parse(newConfig.c_str()).HasParseError())
		{
			Logger::getLogger()->error("Unable to parse new email notification plugin config: '%s'", newConfig.c_str());
			return;
		}
	Logger::getLogger()->info("Email plugin reconfig=%s", newConfig.c_str());

	parseConfig(&document, &info->emailCfg);
	
	if (info->emailCfg.email_to == "" || info->emailCfg.server == "" || info->emailCfg.port == 0)
	{
		Logger::getLogger()->error("New config for email notification plugin is incomplete");
		// TODO: not sure how to indicate failure to caller
	}
	
	return;
}

/**
 * Call the shutdown method in the plugin
 */
void plugin_shutdown(PLUGIN_HANDLE *handle)
{
	PLUGIN_INFO *info = (PLUGIN_INFO *) handle;
	delete info->handle;
	delete info;
}

// End of extern "C"
};

