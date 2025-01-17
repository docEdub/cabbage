/*
  Copyright (C) 2016 Rory Walsh

  Cabbage is free software; you can redistribute it
  and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  Cabbage is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU General Public
  License along with Csound; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  02111-1307 USA
*/

#include "CabbageWebView.h"
#include "../Audio/Plugins/CabbagePluginEditor.h"


CabbageWebView::CabbageWebView (ValueTree wData, CabbagePluginEditor* o)
    : widgetData (wData),
    CabbageWidgetBase(o),
    owner(o)
{
    setName(CabbageWidgetData::getStringProp(wData, CabbageIdentifierIds::name));
    setLookAndFeel(nullptr);
    choc::ui::WebView::Options options;
    options.enableDebugMode = true;
    webView.reset(new choc::ui::WebView(options));

    nativeWindow = std::make_unique<NativeWindowComponent>(webView->getViewHandle());
    addAndMakeVisible(nativeWindow.get());


    const String mntDir = CabbageWidgetData::getStringProp(wData, CabbageIdentifierIds::mountPoint);
    File mntPoint(File(CabbageWidgetData::getStringProp(wData, CabbageIdentifierIds::csdfile)).getParentDirectory().getChildFile(mntDir));
    
    
    
    if(!mntPoint.exists())
    {
        owner->getProcessor().getCsound()->Message("Cabbage: Could not find mount point for webview");
    }
    else
    {
#if !Cabbage_IDE_Build
    server = std::make_unique<CabbageHttpServer>();
#else
    auto* server = CabbageHttpServer::getInstance();
#endif
    if (!server->isRunning())
        server->start(mntPoint.getFullPathName().toStdString());
        
        const auto port = server->getCurrentPort();
        CabbageWidgetData::setNumProp(wData, CabbageIdentifierIds::serverport, port);
        
        //this method will be called by the cabbageWebSend opcode. Users should register
        //an event listener in their html code to pick these up
        std::string js = R"(
            let cabbageHasLoadedWebView = false;
            class Cabbage{
              constructor(){}
  
              setChannel(name, value){
                updateCabbageChannel({name:name, value:value});
              }
  
              addListener(name, callback){
                  window.addEventListener(name, (ev) => {
                        callback(JSON.parse(ev.detail));
                  });
              }
            }

            
            window.addEventListener("load", (event) => {
                    console.log("page loaded")              
                    cabbageHasLoadedWebView = true;
            });
                  

            //window.addEventListener("cabbageChannelUpdate", function(e){
            //    updateCabbageChannel({name:e.detail.name, value:e.detail.value});
            //});
        
            function sendDataToWebUI(data){
              if(cabbageHasLoadedWebView){
                  const obj = JSON.parse(data);
                  //console.log(obj["name"]);
                  event = new CustomEvent(obj["name"], { detail: obj["data"]});
                  window.dispatchEvent(event);
                }
            }
        
        )";
        
        webView->addInitScript(js);
        auto file = CabbageWidgetData::getStringProp(wData, CabbageIdentifierIds::file);
        if(file.isEmpty())
            file = "index.html";
        
        auto urlParams = CabbageWidgetData::getStringProp(wData, CabbageIdentifierIds::channel);
        webView->navigate("http://127.0.0.1:"+std::to_string(port)+"/"+file.toStdString()+"?params="+ urlParams.toStdString());
        
        webView->bind("updateCabbageChannel", [this](const choc::value::ValueView &args) -> choc::value::Value {

            auto p = choc::json::toString(args);
                    var parsedJson;

                    if (JSON::parse(p, parsedJson).wasOk()) {
                        if(parsedJson[0].getDynamicObject())
                        {
                            DBG(parsedJson[0].getDynamicObject()->getProperties().size());
                        }
                        auto p1 = parsedJson[0];
                        
                        auto name = p1.getProperty("name", "NULL").toString();
                        
                        double value = double(p1.getProperty("value", 0));
                        if (CabbagePluginParameter* param = owner->getParameterForComponent (name))
                        {
                            param->setValueNotifyingHost (param->getNormalisableRange().convertTo0to1 (value));
                        }
                        else
                        {
                            owner->sendChannelDataToCsound(name, value);
                        }
                        //updateParameter(name, static_cast<float>(value), LatticeProcessor::NotifyHost);
                    };
                    return choc::value::createString("Cabbage has received update info from webUI");
                });
    }
  

    setName (CabbageWidgetData::getStringProp (wData, CabbageIdentifierIds::name));
    widgetData.addListener (this);              //add listener to valueTree so it gets notified when a widget's property changes
    initialiseCommonAttributes (this, wData);   //initialise common attributes such as bounds, name, rotation, etc..

}

CabbageWebView::~CabbageWebView()
{
#if !Cabbage_IDE_Build
    server->getHttpServer().stop();
    if(server->stopThread(-1))
        DBG("thread exited gracefully");
    else
        DBG("thread shat itself");
#endif
}

void CabbageWebView::resized() 
{
    nativeWindow->setBounds(getLocalBounds());
}

void CabbageWebView::valueTreePropertyChanged (ValueTree& valueTree, const Identifier& prop)
{   
    if(prop == CabbageIdentifierIds::jsonData)
    {
        auto data = JSON::toString(valueTree.getProperty(prop));
        String jsCode = R"(sendDataToWebUI(`DATA`);)";
        auto newString = jsCode.replace("DATA", data);
        //DBG(newString);

        if(webView)
            webView->evaluateJavascript(newString.toStdString());
        
    }
    else
    {
        handleCommonUpdates (this, valueTree, prop);      //handle comon updates such as bounds, alpha, rotation, visible, etc
    }
}
