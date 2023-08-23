#pragma once

#include <microhttpd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <map>
#include <sstream>

#define KILO 1024
#define MEGA 1024*1024
#define MAXNAMESIZE 256
#define MAXANSWERSIZE 16*MEGA
#define POSTBUFFERSIZE 16*MEGA
#define GET  0
#define POST 1
#define GRETING_PAGE "<html><body><h1>Welcome, %s!</center></h1></body></html>"
#define ERROR_PAGE "<html><body>This doesn't seem to be right.</body></html>"

#include "webRenderer.h"

struct connection_info_struct
{
    int connectiontype;
    char *answerstring;
    struct MHD_PostProcessor *postprocessor;
};

class rootRenderer : public webRenderer
{
public:
    rootRenderer(std::string mimeType, void * data)
        :
          webRenderer(mimeType, data)
    {
    }
    virtual void render(const std::string url);
};

class localeRenderer : public webRenderer
{
public:
    virtual void render(const std::string url);

private:
    std::string localeFile;
    std::string localeContent;
};

class webServer
{
public:
    webServer(std::string host, uint16_t port);
    ~webServer();

    void        start();
    void        stop();

    void		addResource(std::string name, webRenderer * renderer);

    std::string getServerHost();
    std::string getLocale();
    std::string getTranslation(std::string key);
    uint16_t    getServerPort();

private:

    std::string serverHost;
    uint16_t    serverPort;

    // Statics callbacks used by microhttpd library (C)
    static MHD_Result send_page (struct MHD_Connection *	connection,
                                 void *                     page,
                                 size_t                     size,
                                 const char *               mimeType,
                                 int                        status_code);

    static MHD_Result iterate_post(void *             coninfo_cls,
                                   enum MHD_ValueKind kind,
                                   const char *       key,
                                   const char *       filename,
                                   const char *       content_type,
                                   const char *       transfer_encoding,
                                   const char *       data,
                                   uint64_t           off,
                                   size_t             size);

    static MHD_Result request_completed (void *                     cls,
                                         struct MHD_Connection *    connection,
                                         void **                    con_cls,
                                         MHD_RequestTerminationCode toe);

    static MHD_Result print_out_key ( void *                cls,
                                      enum MHD_ValueKind    kind,
                                      const char *          key,
                                      const char *          value);

    static MHD_Result answer_to_connection(void *                   cls,
                                           struct MHD_Connection *  connection,
                                           const char *             url,
                                           const char *             method,
                                           const char *             version,
                                           const char *             upload_data,
                                           uint64_t *     upload_data_size,
                                           void **                  con_cls);


    struct MHD_Daemon *                 MHDdaemon;
};

extern std::map<std::string, webRenderer*> baseResources;
