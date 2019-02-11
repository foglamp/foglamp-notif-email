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
#include <logger.h>
#include <email_config.h>
#include <version.h>

#define EMAIL_CFG "\"email_from\":\"dianomic.alerts@gmail.com\",\"email_from_name\":\"Notification alert\",\"email_to\":\"alert.subscriber@dianomic.com\"," \
"\"email_to_name\":\"Notification alert subscriber\",\"server\":\"smtp.gmail.com\",\"port\":587,\"subject\":\"Foglamp alert notification\"," \
"\"use_ssl_tls\":true,\"username\":\"dianomic.alerts@gmail.com\",\"password\":\"pass\""

#define PLUGIN_NAME "email"

#define DEFAULT_CONFIG "{\"plugin\" : { \"description\" : \"Email notification plugin\", " \
                       		"\"type\" : \"string\", " \
				"\"default\" : \"" PLUGIN_NAME "\" }, \"readonly\" : \"true\" }, " \
			 "\"enable\": {\"description\": \"A switch that can be used to enable or disable execution of " \
					 "the email notification plugin.\", " \
				"\"type\": \"boolean\", " \
				"\"displayName\" : \"Enabled\", " \
				"\"default\": \"false\" }, " \
			"\"emailCfg\" : {\"description\" : \"Email server & account config\", " \
				"\"type\" : \"JSON\", " \
				"\"default\" : {" EMAIL_CFG "}\", "\
				"\"order\" : \"1\", \"displayName\" : \"Email server & account config\"} }"

using namespace std;
using namespace rapidjson;

/**
 * The Notification plugin interface
 */
extern "C" {

/**
 * The plugin information structure
 */
static PLUGIN_INFORMATION info = {
        PLUGIN_NAME,              // Name
        VERSION,                  // Version
        0,                        // Flags
        PLUGIN_TYPE_NOTIFICATION_DELIVERY,       // Type
        "1.0.0",                  // Interface version
        DEFAULT_CONFIG	          // Default plugin configuration
};

typedef struct
{
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
	emailCfg->use_ssl_tls = false;
	emailCfg->username = "";
	emailCfg->password = "";
}

/**
 * Print EmailCfg structure
 */
void printConfig(EmailCfg *emailCfg)
{
	Logger::getLogger()->info("email_from=%s, email_from_name=%s, email_to=%s, email_to_name=%s",
						emailCfg->email_from.c_str(), emailCfg->email_from_name.c_str(), 
						emailCfg->email_to.c_str(), emailCfg->email_to_name.c_str());
	Logger::getLogger()->info("server=%s, port=%d, subject=%s, use_ssl_tls=%s, username=%s, password=%s",
						emailCfg->server.c_str(), emailCfg->port, emailCfg->subject.c_str(), 
						emailCfg->use_ssl_tls?"true":"false", emailCfg->username.c_str(), emailCfg->password.c_str());
}

/**
 * Fill EmailCfg structure from JSON document representing email server/account config
 */
void parseConfig(Document *document, EmailCfg *emailCfg)
{
	for (Value::ConstMemberIterator itr = document->MemberBegin(); itr != document->MemberEnd(); ++itr)
	{
		if (strcmp(itr->name.GetString(), "email_from") == 0 && itr->value.IsString())
		{
			emailCfg->email_from = itr->value.GetString();
		}
		else if (strcmp(itr->name.GetString(), "email_from_name") == 0 && itr->value.IsString())
		{
			emailCfg->email_from_name= itr->value.GetString();
		}
		else if (strcmp(itr->name.GetString(), "email_to") == 0 && itr->value.IsString())
		{
			emailCfg->email_to = itr->value.GetString();
		}
		else if (strcmp(itr->name.GetString(), "email_to_name") == 0 && itr->value.IsString())
		{
			emailCfg->email_to_name = itr->value.GetString();
		}
		else if (strcmp(itr->name.GetString(), "server") == 0 && itr->value.IsString())
		{
			emailCfg->server = itr->value.GetString();
		}
		else if (strcmp(itr->name.GetString(), "port") == 0 && itr->value.IsNumber())
		{
			emailCfg->port = (unsigned int) itr->value.GetInt();
		}
		else if (strcmp(itr->name.GetString(), "subject") == 0 && itr->value.IsString())
		{
			emailCfg->subject = itr->value.GetString();
		}
		else if (strcmp(itr->name.GetString(), "use_ssl_tls") == 0 && itr->value.IsBool())
		{
			emailCfg->use_ssl_tls = itr->value.IsTrue();
		}
		else if (strcmp(itr->name.GetString(), "username") == 0 && itr->value.IsString())
		{
			emailCfg->username = itr->value.GetString();
		}
		else if (strcmp(itr->name.GetString(), "password") == 0 && itr->value.IsString())
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
PLUGIN_HANDLE plugin_init(ConfigCategory* config)
{
	PLUGIN_INFO *info = new PLUGIN_INFO;
	
	// Handle plugin configuration
	if (config)
	{
		Document	document;
		if (document.Parse(config->getValue("emailCfg").c_str()).HasParseError())
		{
			Logger::getLogger()->error("Unable to parse email plugin config: '%s'", config->getValue("emailCfg").c_str());
			return NULL;
		}
		Logger::getLogger()->info("Email plugin config=%s", config->toJSON().c_str());

		resetConfig(&info->emailCfg);
		parseConfig(&document, &info->emailCfg);
		//printConfig(&info->emailCfg);
		
		if (info->emailCfg.email_to == "" || info->emailCfg.server == "" || info->emailCfg.port == 0)
		{
			Logger::getLogger()->error("Config for email notification plugin is incomplete, exiting...");
			delete info;
			info = NULL;
		}
	}
	else
	{
		Logger::getLogger()->info("No config provided for email plugin, exiting...");
		delete info;
		info = NULL;
	}
	
	return (PLUGIN_HANDLE)info;
}

/**
 * Deliver received notification data
 *
 * @param handle		The plugin handle returned from plugin_init
 * @param deliveryName		The delivery category name
 * @param notificationName	The notification name
 * @param triggerReason		The trigger reason for notification
 * @param message		The message from notification
 */
bool plugin_deliver(PLUGIN_HANDLE handle,
                    const std::string& deliveryName,
                    const std::string& notificationName,
                    const std::string& triggerReason,
                    const std::string& message)
{
	Logger::getLogger()->info("Email notification plugin_deliver(): deliveryName=%s, notificationName=%s, triggerReason=%s, message=%s",
							deliveryName.c_str(), notificationName.c_str(), triggerReason.c_str(), message.c_str());
	PLUGIN_INFO *info = (PLUGIN_INFO *) handle;
	
	int rv = sendEmailMsg(&info->emailCfg, message.c_str());
	if (rv)
		Logger::getLogger()->info("sendEmailMsg() returned %d", rv);
	else
		Logger::getLogger()->info("sendEmailMsg() returned SUCCESS");
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
		// How to indicate failure to caller?
		// Maybe keep a copy of previous emailCfg structure and restore it here in case of failure
	}
	
	return;
}

/**
 * Call the shutdown method in the plugin
 */
void plugin_shutdown(PLUGIN_HANDLE *handle)
{
	PLUGIN_INFO *info = (PLUGIN_INFO *) handle;
	delete info;
}

// End of extern "C"
};

