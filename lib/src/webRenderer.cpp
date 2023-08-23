#include "webRenderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

webRenderer::webRenderer(std::string mimeType, void * data)
    :
      mimeType(mimeType),
      fd(0),
      customData(data),
      data(NULL),
      size(0)
{
}

webRenderer::~webRenderer()
{
    postVars.clear();
    free(data);
}

void webRenderer::renderFile(std::string fileName)
{
    struct stat st;
    char * fileData;
    fd = open(fileName.c_str(), O_RDONLY);

    fstat(fd, &st);
    size = st.st_size;
    data = malloc(size * sizeof(*fileData));
    read(fd, data, size);
    close(fd);
}

void webRenderer::setData(const void * value, size_t valueSize)
{
    size = valueSize;
    data = malloc(size + 1);
    memcpy(data, value, size);
}

void * webRenderer::getData() const
{
    return data;
}

void * webRenderer::getCustomData() const
{
    return customData;
}

size_t webRenderer::getSize() const
{
    return size;
}

const std::string webRenderer::getMimeType() const
{
    return mimeType;
}

struct connection_info_struct *webRenderer::getConnexionInfo() const
{
    return connexionInfo;
}

void webRenderer::setConnexionInfo(struct connection_info_struct *value)
{
    connexionInfo = value;
}

void webRenderer::setVar(std::string & key, const std::string & value)
{
    if (postVars.find(key) != postVars.end())
        postVars[key] = value;
}

const std::string webRenderer::getVar(const std::string key) const
{
    if (postVars.find(key) != postVars.end())
        return postVars.at(key);
    std::string list;
    for(auto it = postVars.begin() ; it != postVars.end() ; it++)
        list += it->first + "" + it->second + "<br>\n";
    return(list);
}

webRenderer::postVarList * webRenderer::getVarMap()
{
    return &postVars;
}

