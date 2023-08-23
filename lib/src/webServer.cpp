#include "webServer.h"
#include "webRenderer.h"
#include <map>
#include <fstream>
#include <iostream>

std::map<std::string, webRenderer*> baseResources;

std::string current_time()
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%Y%m%d%H%M%S",timeinfo);
    std::string str(buffer);

    return (str);
}

void rootRenderer::render(const std::string url)
{
    //    url = url;

    std::ostringstream text;

    text << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    text << "<resources>" << std::endl;
    for (std::map<std::string, webRenderer*>::iterator it = baseResources.begin();
         it != baseResources.end();
         it++)
    {
        text << "  <item>" << std::endl;
        text << "    <link>" << it->first << "</link>" << std::endl;
        text << "    <name>" << it->first << "</name>" << std::endl;
        text << "  </item>" << std::endl;
    }
    text << "</resources>" << std::endl;
    setData(text.str().c_str(), text.str().length());
}

webServer::webServer(std::string host, uint16_t port)
    :
      serverHost(host),
      serverPort(port)
{
    addResource("/", new rootRenderer(std::string("text/xml"), this));
    addResource("/resources", new rootRenderer(std::string("text/xml"), this));
}

webServer::~webServer()
{
}


std::string webServer::getServerHost()
{
    return serverHost;
}

uint16_t    webServer::getServerPort()
{
    return serverPort;
}

void webServer::start()
{
    MHDdaemon = MHD_start_daemon (MHD_USE_SELECT_INTERNALLY,
                                  serverPort,
                                  NULL,
                                  NULL,
                                  &answer_to_connection, NULL,
                                  MHD_OPTION_NOTIFY_COMPLETED,
                                  &request_completed, this,
                                  MHD_OPTION_END);
    if (NULL == MHDdaemon)
        throw;
}

void webServer::stop()
{
    if (MHDdaemon != NULL)
    {
        MHD_stop_daemon(MHDdaemon);
    }
    MHDdaemon = NULL;
}

void webServer::addResource(std::string name, webRenderer *renderer)
{
    baseResources.insert(std::pair<std::string, webRenderer*>(name, renderer));
}

MHD_Result webServer::send_page (struct MHD_Connection *   connection,
                                 void *                    page,
                                 size_t                    size,
                                 const char *              mimeType,
                                 int                       status_code)
{
    MHD_Result                     ret;
    struct MHD_Response *	response;
    response = MHD_create_response_from_buffer (size,
                                                page,
                                                MHD_RESPMEM_MUST_COPY);
    if (!response)
        return MHD_NO;

    MHD_add_response_header (response, "Content-Type", mimeType);
    ret = MHD_queue_response (connection, status_code, response);
    if (ret == MHD_NO)
        throw;
    MHD_destroy_response (response);

    return ret;
}

MHD_Result webServer::iterate_post (void *                coninfo_cls,
                                    enum MHD_ValueKind    kind,
                                    const char *          key,
                                    const char *          filename,
                                    const char *          content_type,
                                    const char *          transfer_encoding,
                                    const char *          data,
                                    uint64_t              off,
                                    size_t                size)
{
    static std::map<std::string, std::string> postbuffer;

    transfer_encoding = transfer_encoding;
    webRenderer* res = (webRenderer*)coninfo_cls;
    //struct connection_info_struct *con_info = res->getConnexionInfo();

    std::string strKey  = std::string(key);
    std::string strValue = std::string(data);

    strValue[(int)size] = '\0';

    if (off == 0)
    {
        postbuffer[strKey].clear();
        postbuffer[strKey].reserve(POSTBUFFERSIZE);
        //        postbuffer[strKey].fill('\0', POSTBUFFERSIZE);
        postbuffer[strKey].insert(off, strValue);
    }
    else
        postbuffer[strKey].insert(off, strValue);

    postbuffer[strKey][(int)strlen(postbuffer[strKey].c_str())] = '\0';
    //postbuffer[strKey] = postbuffer[strKey].substr(0, postbuffer[strKey].length());
    res->setVar(strKey, postbuffer[strKey]);
    return MHD_YES;
}

MHD_Result webServer::request_completed (  void *                          cls,
                                           struct MHD_Connection *         connection,
                                           void **                         con_cls,
                                           enum MHD_RequestTerminationCode toe)
{
    cls = cls;
    connection = connection;

    struct connection_info_struct *con_info = (struct connection_info_struct *)*con_cls;

    if (NULL == con_info) return MHD_NO;

    if (con_info->connectiontype == POST)
    {
        MHD_destroy_post_processor (con_info->postprocessor);
        //if (con_info->answerstring) free (con_info->answerstring);
        std::cerr << __FILE__ << ":" << __LINE__ << std::endl;

    }

    free (con_info);
    *con_cls = NULL;
    std::cerr << __FILE__ << ":" << __LINE__ << std::endl;

    return MHD_YES;
}

MHD_Result webServer::print_out_key (void *               cls,
                                     enum MHD_ValueKind	  kind,
                                     const char *         key,
                                     const char *         value)
{
    cls = cls;
    kind = kind;

    printf ("%s: %s\n", key, value);
    return MHD_YES;
}

MHD_Result webServer::answer_to_connection (void *                      cls,
                                            struct MHD_Connection *     connection,
                                            const char *                url,
                                            const char *                method,
                                            const char *                version,
                                            const char *                upload_data,
                                            uint64_t *    upload_data_size,
                                            void **                     con_cls)
{
    cls = cls;
    MHD_get_connection_values (connection, MHD_HEADER_KIND, &print_out_key, NULL);

    if(NULL == *con_cls)
    {
        struct connection_info_struct *con_info = new struct connection_info_struct;

        con_info->answerstring = NULL;

        if (0 == strcmp (method, "POST"))
        {
            std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
            if (baseResources.end() != baseResources.find(url))
            {
                webRenderer* res = baseResources[url];
                res->setConnexionInfo(con_info);

                con_info->postprocessor
                        = MHD_create_post_processor (connection, POSTBUFFERSIZE,
                                                     &iterate_post, (void*) res);

                if (NULL == con_info->postprocessor)
                {
                    delete (con_info);
                    return MHD_NO;
                }
                con_info->connectiontype = POST;
            }
            return MHD_YES;
        }
        else
        {
            std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
            con_info->connectiontype = GET;
        }
        *con_cls = (void*) con_info;
    }

    if ((0 == strcmp (method, "GET")) && (baseResources.end() != baseResources.find(url)))
    {
        std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
        if (baseResources.end() != baseResources.find(url))
        {
            webRenderer* res = baseResources[url];
            std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
            res->render(url);
            std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
            return send_page(connection, res->getData(), res->getSize(), res->getMimeType().c_str(), 200);
        }
        else
            printf("url = %s unknow !\n", url);
    }

    if (0 == strcmp (method, "POST"))
    {
        std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
        if (baseResources.end() != baseResources.find(url))
        {
            std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
            webRenderer* res = baseResources[url];

            struct connection_info_struct *con_info = (struct connection_info_struct *)*con_cls;
            res->setConnexionInfo(con_info);

            if (*upload_data_size != 0)
            {
                MHD_post_process (con_info->postprocessor, upload_data,
                                  *upload_data_size);
                *upload_data_size = 0;

                return MHD_YES;
            }
            else if (NULL != con_info->answerstring)
                return send_page (connection, con_info->answerstring, strlen(con_info->answerstring), "text/html", 200);

            std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
            res->render(url);
            res->getVarMap()->clear();
            std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
            return send_page(connection, res->getData(), res->getSize(), res->getMimeType().c_str(), 200);
        }
    }

    return send_page(connection, (void*)ERROR_PAGE, strlen(ERROR_PAGE), "text/html", 404);
}
