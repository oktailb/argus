var ajaxlibjs;

if (typeof ajaxlibjs == 'undefined')
{
    ajaxlibjs = true;
    console.log("ajaxlib.js");

    function get_haut()
    {
        var haut;

        if (document.body)
        {
            haut = (document.body.clientHeight);
        }else{
            haut = (window.innerHeight);
        }
        return haut;
    };

    function get_larg()
    {
        var larg;

        if (document.body)
        {
            larg = (document.body.clientWidth);
        }else{
            larg = (window.innerWidth);
        }
        return larg;
    };

    function getData(page)
    {
        var xhr_object = null;
        if(window.XMLHttpRequest) // Firefox
            xhr_object = new XMLHttpRequest();
        else if(window.ActiveXObject) // Internet Explorer
            xhr_object = new ActiveXObject("Microsoft.XMLHTTP");
        else
        {
            console.log("Votre navigateur ne supporte pas les objets XMLHTTPRequest...");
            return;
        }
        xhr_object.open("GET", page, false);
        xhr_object.send(null);
        if(xhr_object.readyState == 4)
        {
            return (xhr_object.responseText);
        }
        else
        {
            return("Unable to load required content, check connection or proxy limitations");
        }
    };

    function urlencode(str)
    {
        return escape(str.replace(/%/g, '%25').replace(/\+/g, '%2B')).replace(/%25/g, '%');
    }

    function evalJsContent(target)
    {
        var scripts = document.getElementById(target).getElementsByTagName("script");
        if (scripts != null)
        {
            nbScripts = scripts.length;
            for(var i = 0; i < nbScripts; i++)
            {
                if ((scripts[i] != false) && (scripts[i].innerHTML.length != 0))
                {
                    try
                    {
                        var evalRes = eval(scripts[i].innerHTML);
                    }
                    catch (e)
                    {
                        console.log(e);
                    }
                }
                if (scripts[i] != null)
                {
                    if (scripts[i].getAttribute("src"))
                    {
                        var todo = scripts[i].getAttribute("src");
                        if (todo)
                        {
                            var script = getData(todo);
                            try
                            {
                                var evalRes = eval(script);
                            }
                            catch (e)
                            {
                                console.log(e);
                            }
                        }
                    }
                }
            }
        }
    }

    function setPage(data, target)
    {
        if (target != null)
        {
            var doc = document.getElementById(target);
            if (doc == null)
            {
                doc = document.getElementsByTagName('html')[0];
            }
            doc.innerHTML = data;
            evalJsContent(target);
        }
    }

    function copyContent(source, target)
    {
        if ((source != null) && (target != null))
        {
            var docSource = document.getElementById(source);
            var docTarget = document.getElementById(target);
            if ((docSource != null) && (docTarget != null))
                docTarget.innerHTML = docSource.innerHTML;
        }
    }

    function getPage(page, target, async=false)
    {
        var xhr_object = null;
        if(window.XMLHttpRequest) // Firefox
            xhr_object = new XMLHttpRequest();
        else if(window.ActiveXObject) // Internet Explorer
            xhr_object = new ActiveXObject("Microsoft.XMLHTTP");
        else
        {
            console.log("Votre navigateur ne supporte pas les objets XMLHTTPRequest...");
            return;
        }
        xhr_object.onload = function ()
        {
            if (target != null)
            {
                var doc = document.getElementById(target);
                if (doc == null)
                {
                    doc = document.getElementsByTagName('html')[0];
                }
                doc.innerHTML = xhr_object.responseText;
                evalJsContent(target);
            }
        };
        xhr_object.open("GET", page, async);
        xhr_object.send(null);
        if (async == false)
        {
            return xhr_object.responseText;
        }
        else
            return false;
    };

    function addPage(page, target)
    {
        var doc = document.getElementById(target);
        if (doc == null)
        {
            doc = document.getElementsByTagName('html')[0];
        }
        var xhr_object = null;
        if(window.XMLHttpRequest) // Firefox
            xhr_object = new XMLHttpRequest();
        else if(window.ActiveXObject) // Internet Explorer
            xhr_object = new ActiveXObject("Microsoft.XMLHTTP");
        else
        {
            console.log("Votre navigateur ne supporte pas les objets XMLHTTPRequest...");
            return;
        }
        xhr_object.open("GET", page, false);
        xhr_object.send(null);
        if(xhr_object.readyState == 4)
        {
            doc.innerHTML += xhr_object.responseText;
            evalJsContent(target);
        }
    };

    function purgePage(target)
    {
        var doc = document.getElementById(target);
        if (doc == null)
        {
            doc = document.getElementsByTagName('html')[0];
        }
        doc.innerHTML = "";
    };

    function post(path, params, target, async=true)
    {
        var method = 'post';


        var xForm = document.createElement("form");
        xForm.setAttribute("method", "post");
        xForm.setAttribute("action", path);
        for (var key in params)
        {
            if (params.hasOwnProperty(key))
            {
                var hiddenField = document.createElement("input");
                hiddenField.setAttribute("name", key);
                hiddenField.setAttribute("value", params[key]);
                xForm.appendChild(hiddenField);
            }
        }
        var xhr = new XMLHttpRequest();
        xhr.onload = function ()
        {
            if (target != null)
            {
                var doc = document.getElementById(target);
                if (doc != null)
                {
                    doc.innerHTML = xhr.responseText;
                    evalJsContent(target);
                }
            }
        };
        xhr.open(xForm.method, xForm.action, async);
        xhr.send(new FormData(xForm));
        if (async == false)
        {
            return xhr.responseText;
        }
        else
            return false;
    };

    function setPosAndSize(elem, w, h, x, y, z, position = "absolute")
    {
        try
        {
            elem = document.getElementById(elem);
            if (x == '100%')
                x = get_larg() - w;
            if (y == '100%')
                y = get_haut() - h;
            if (w == '100%')
                w = get_larg() - x;
            if (h == '100%')
                h = get_haut() - y;
            if (x < 0)
                x = get_larg() + x;
            if (y < 0)
                y = get_haut() + y;
            if (w < 0)
                w = get_larg() - x + w;
            if (h < 0)
                h = get_haut() - y + h;
            elem.style.position= position;
            elem.style.top = y;
            elem.style.left = x;
            elem.style.zIndex = z;
            elem.style.width = w;
            elem.style.height = h;
            elem.style.overflow = "hidden";
        }
        catch (e)
        {
            console.log(e);
        }
    };

    function createInput(id, type, value, onchange = "")
    {
        var input = document.createElement("input");
        input.setAttribute("id", id);
        input.setAttribute("type", type);
        input.setAttribute("value", value);
        input.setAttribute("onchange", onchange);

        return input.outerHTML;
    }

    function appendRow(target, cols, arrayAttr, arrayContent)
    {
        var doc = document.getElementById(target);
        if (doc == null)
        {
            doc = document.getElementsByTagName('html')[0];
        }
        var res = "";
        res += "<tr>";
        for (i = 0 ; i < cols ; i++)
        {
            var attr = arrayAttr.shift();
            var content = arrayContent.shift();
            res += "<td " + attr + ">";
            res += content;
            res += "</td>";
        }

        res += "</tr>";
        doc.innerHTML += res;
    }

    function appendItem(target, content, type, attributes="", container="")
    {
        var doc = document.getElementById(target);
        var out = "";
        if (doc != null)
        {
            if (container != "")
                out += "<" + container + ">";

            out += "<" + type + " " + attributes + ">";
            out += content;
            out += "</" + type + ">";

            if (container != "")
                out += "</" + container + ">";
        }
        doc.innerHTML += out;
    }

    function openTab(parent, tabName)
    {
        var i;
        var rootElement = document.getElementById(parent);
        if (rootElement != null)
        {
            var x = rootElement.getElementsByClassName("rc_content");

            for (i = 0; i < x.length; i++)
            {
                var currentTabName = x[i].id.split('_').pop()
                var button = document.getElementById("tab_" + currentTabName);

                if (x[i].id === (parent + "_" + tabName))
                {
                    x[i].style.display = "block";
                    if (button != null)
                        button.style.backgroundColor = "#526275";
                }
                else
                {
                    x[i].style.display = "none";

                    if (button != null)
                        button.style.backgroundColor = "#728295";
                }
            }
        }
    }

    function isVisible(elem)
    {
        if (!(elem instanceof Element))
            throw Error('DomUtil: elem is not an element.');
        const style = getComputedStyle(elem);
        if (style.display === 'none')
            return false;
        if (style.visibility !== 'visible')
            return false;
        if (style.opacity < 0.1)
            return false;
        if (elem.offsetWidth + elem.offsetHeight + elem.getBoundingClientRect().height +
                elem.getBoundingClientRect().width === 0)
        {
            return false;
        }
        const elemCenter = {
            x: elem.getBoundingClientRect().left + elem.offsetWidth / 2,
            y: elem.getBoundingClientRect().top + elem.offsetHeight / 2
        };
        if (elemCenter.x < 0)
            return false;
        if (elemCenter.x > (document.documentElement.clientWidth || window.innerWidth))
            return false;
        if (elemCenter.y < 0)
            return false;
        if (elemCenter.y > (document.documentElement.clientHeight || window.innerHeight))
            return false;
        let pointContainer = document.elementFromPoint(elemCenter.x, elemCenter.y);
        do
        {
            if (pointContainer === elem)
                return true;
        } while (pointContainer = pointContainer.parentNode);

        return false;
    }


    function setVisible(target)
    {
        var doc = document.getElementById(target);
        if (doc != null)
        {
            doc.style.display = 'block';
        }
    }

    function setHidden(target)
    {
        var doc = document.getElementById(target);
        if (doc != null)
        {
            doc.style.display = 'none';
        }
    }


    function updateInput(id)
    {
        var inputElement = document.getElementById(id);
        if (inputElement != null)
        {
            if ((inputElement != document.activeElement) && (isVisible(inputElement)))
            {
                var realId = id.split("_").pop();
                var value = post("/getVar", {id: realId}, null, false);
                inputElement.value = value;
            }
        }
    }

    function updateInputPolygon(id)
    {
//        var inputElement = document.getElementById(id);
//        if (inputElement != null)
//        {
//            if ((inputElement != document.activeElement) && (isVisible(inputElement)))
//            {
//                var realId = id.split("_").pop();
//                var value = post("/getVar", {id: realId}, null, false);
//                inputElement.value = value;
//            }
//        }
    }

    function receiveData(addr, port, handler)
    {
        var socket = new WebSocket('ws://' + addr + ':' + port);
        socket.binaryType = 'arraybuffer';

        socket.onmessage = function(event)
        {
            handler(event.data);
        }

        return socket;
    }

    function click_ok()
    {
        post('/setVar', {parentName: "root", id: "SaveSettings", val: 1}, null, false);
        open(location, '_self').close();
    }

    function click_apply()
    {
        post('/setVar', {parentName: "root", id: "SaveSettings", val: 1}, null, false);
    }

    function click_reset()
    {
        post('/setVar', {parentName: "root", id: "ReloadDefaultSettings", val: 1}, null, false);
    }

    function click_cancel()
    {
        post('/setVar', {parentName: "root", id: "ReloadLastSettings", val: 1}, null, false);
        open(location, '_self').close();
    }

    function addFieldTo(targetName, group, key, infoBubble = "", helpUrl = "", type = "tr")
    {
        var target = document.getElementById(targetName)
        if (target != null)
        {
            target.innerHTML += '<'  + type + ' id="' + key + '"></' + type + '>';
            post("/editForm", {target: targetName, name: group + "::" + key, info: infoBubble, url: helpUrl}, key);
        }
        else
            console.log("Target " + targetName + " not found in document");
    }

    function prepareFieldSet(targetName, title, id)
    {
        var target = document.getElementById(targetName)
        if (target != null)
        {
            var res = "";
            res += '<fieldset class="rc_box" id="' + id + '_fieldset">';
            res += ' <legend>' + title + '</legend>';
            res += ' <table>';
            res += '  <thead>';
            res += '  </thead>';
            res += '  <tbody id="' + id + '">';
            res += '  </tbody>';
            res += ' </table>';
            res += '</fieldset>';

            target.innerHTML += res;
        }
        else
            console.log("Target " + targetName + " not found in document");
    }

}
