// webResource.h
#ifndef __WEB_RENDERER_H__
#define __WEB_RENDERER_H__

#include <cstddef>
#include <map>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/**
 * @brief The webRenderer class allows to simply answer to web requests (GET and POST)
 */
class webRenderer
{
public:
    typedef std::map<std::string, std::string>  postVarList;
    webRenderer(std::string mimeType, void * data = NULL);
    ~webRenderer();

    /**
     * @fn render
     * @brief To be implemented by the specialized class. The methods have to produce the HTML/whatever code and push it using setData()
     * @param url
     */
    virtual void    render(const std::string url) = 0;

    void            renderFile(std::string fileName);

    void            setData(const void * value, size_t valueSize);
    void *          getData() const;
    void *          getCustomData() const;
    size_t          getSize() const;
    const std::string   getMimeType() const;

    void            setVar(std::string &key, const std::string &value);
    const std::string   getVar(const std::string key) const;
    postVarList *   getVarMap();

    struct connection_info_struct *
                    getConnexionInfo() const;

    void            setConnexionInfo(struct connection_info_struct *value);

private:
    int         fd;
    void *      data;
    void *      customData;
    size_t      size;
    std::string mimeType;
    struct connection_info_struct *connexionInfo;
    postVarList postVars;
};

#endif
