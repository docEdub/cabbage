/*
 Copyright (C) 2018 Rory Walsh

 You should have received a copy of the GNU Lesser General Public
 License along with Csound; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 02110-1301 USA
 */

#include <plugin.h>
#include <string>
 // #include <iomanip> 
#include <fstream>
// #include <iostream>
#include "json.hpp"
// #include <algorithm>
#include "../CabbageCommonHeaders.h"
using json = nlohmann::json;

#include "../Widgets/CabbageWidgetData.h"
#include "../JuceLibraryCode/JuceHeader.h"

//====================================================================================================
// Set Widget Attribute
//====================================================================================================

#define I_RATE 1
#define K_RATE 2

class CabbagePersistentData 
{
public:
    
    CabbagePersistentData(){}
    
    std::string data = "";
    int size = 0;
};

//====================================================================================================
// ReadStateData
//====================================================================================================
struct ReadStateData : csnd::Plugin<1, 0>
{
    int init()
    {
        return dumpData();
    }
    
    int perf()
    {
        return dumpData();
    }
    
    int dumpData()
    {
        CabbagePersistentData** pd = (CabbagePersistentData**)csound->query_global_variable("cabbageData");
        if(pd != nullptr)
        {
            auto perData = *pd;
            //csound->message(perData->data);
            if (perData->data.empty())
            {
                csound->message("No data, temporary or persistent, has been written to internal state...\n");
            }
            
            outargs.str_data(0).size = int(strlen(perData->data.c_str()));
            outargs.str_data(0).data = csound->strdup((char*)perData->data.c_str());
            return OK;
        }
        
        csound->message("There was a problem reading internal state data\n");
        return OK;
    }
};

//====================================================================================================
// Write State Data
//====================================================================================================
struct WriteStateData : csnd::InPlug<2>
{
    int init()
    {
        writeJsonDataToGlobalVar(I_RATE);
        return OK;
    }

    void writeJsonDataToGlobalVar(int mode)
    {

        std::string jsonString(args.str_data(1).data);
        if(jsonString.empty())
            if(mode == K_RATE)
                csound->perf_error("JSON string is empty\n", this);
            else
                csound->init_error("JSON string is empty:\n");
        
        std::string jsonData = "";
        int writeMode = args[0];
        json j;

        CabbagePersistentData** pd = (CabbagePersistentData**)csound->query_global_variable("cabbageData");
        auto perData = *pd;
        if(perData != nullptr)
        {
            jsonData = perData->data;
        }
        else
        {
            csound->message("Internal JSON global var is not valid.\n");
            return;
        }

        if (json::accept(jsonString) == false)
        {
            csound->init_error("Invalid JSON data:" + jsonString + "\n");
            return;
        }

        if (writeMode == 1)
        {
                if(json::accept(jsonData))
                    j = json::parse(jsonData);
            
                auto j2 = json::parse(jsonString);
                j.update(j2);
        }
        else
        {
            j = json::parse(jsonString);
        }
        
        perData->data = j.dump();
    }
};

//====================================================================================================
// Set float values 
//====================================================================================================
struct SetStateFloatData : csnd::InPlug<2>
{
    int init()
    {
        if(writeJsonDataToGlobalVar(I_RATE))
            return OK;
        else
            return NOTOK;
    }

    int kperf()
    {
        if(writeJsonDataToGlobalVar(K_RATE))
            return OK;
        else
            return NOTOK;
    }
    
    bool writeJsonDataToGlobalVar(int mode)
    {
        
        if (in_count() != 2)
        {
            if(mode == K_RATE)
                csound->perf_error("Not enough input arguments:\n", this);
            else
                csound->init_error("Not enough input arguments:\n");
            return NOTOK;
            return false;
        }
        
        
        
        std::string jsonKeyName(args.str_data(0).data);
        
        if(jsonKeyName.empty())
            if(mode == K_RATE)
                csound->perf_error("JSON key is empty\n", this);
            else
                csound->init_error("JSON key is empty:\n");
        
        
        std::string jsonData;
        MYFLT value = args[1];

        json j;

        CabbagePersistentData** pd = (CabbagePersistentData**)csound->query_global_variable("cabbageData");
        CabbagePersistentData* perData;
        if (pd != nullptr)
        {
            perData = *pd;
            jsonData = perData->data;
        }
        else
        {
            csound->create_global_variable("cabbageData", sizeof(CabbagePersistentData*));
            CabbagePersistentData** pd = (CabbagePersistentData**)csound->query_global_variable("cabbageData");
            *pd = new CabbagePersistentData();
            perData = *pd;
            csound->message("Creating new internal state object...\n");
            jsonData = "{}";
        }

        std::string newData = "{ \"" + jsonKeyName + "\" : " + std::to_string(value) + "}";

        if (json::accept(newData) == false)
        {
            if(mode == K_RATE)
                csound->perf_error("Invalid JSON data:" + newData + "\n", this);
            else
                csound->init_error("Invalid JSON data:" + newData + "\n");
            return false;

            return false;
        }

        j = json::parse(jsonData.empty() ? "{}" : jsonData);
        auto j2 = json::parse(newData);
        j.update(j2);
        perData->data = j.dump();
        return true;
    }
};

struct SetStateFloatArrayData : csnd::InPlug<2>
{
    int init()
    {
        if(writeJsonDataToGlobalVar(I_RATE))
            return OK;
        else
            return NOTOK;
    }
    
    int kperf()
    {
        if(writeJsonDataToGlobalVar(K_RATE))
            return OK;
        else
            return NOTOK;
    }
    
    bool writeJsonDataToGlobalVar(int mode)
    {
        if (in_count() != 2)
        {
            if(mode == K_RATE)
                csound->perf_error("Not enough input arguments\n", this);
            else
                csound->init_error("Not enough input arguments\n");
            return false;
        }
        
        std::string jsonKeyName(args.str_data(0).data);
        std::string jsonData;
        csnd::Vector<MYFLT>& inputArgs = args.myfltvec_data(1);

        json j;

        CabbagePersistentData** pd = (CabbagePersistentData**)csound->query_global_variable("cabbageData");
        CabbagePersistentData* perData;
        if (pd != nullptr)
        {
            perData = *pd;
            jsonData = perData->data;
        }
        else
        {
            csound->create_global_variable("cabbageData", sizeof(CabbagePersistentData*));
            CabbagePersistentData** pd = (CabbagePersistentData**)csound->query_global_variable("cabbageData");
            *pd = new CabbagePersistentData();
            perData = *pd;
            csound->message("Creating new internal state object...\n");
            jsonData = "{}";
        }

        std::string newData = "{ \"" + jsonKeyName + "\" : [";
        for (int i = 0; i < inputArgs.len(); i++) {
            newData += std::to_string(inputArgs[i]) + (i < inputArgs.len() - 1 ? ", " : "");
        }
        newData += "]}";

        if (json::accept(newData) == false)
        {
            if(mode == K_RATE)
                csound->perf_error("Invalid JSON data:" + newData + "\n", this);
            else
                csound->init_error("Invalid JSON data:" + newData + "\n");
            return false;
        }

        j = json::parse(jsonData.empty() ? "{}" : jsonData);
        auto j2 = json::parse(newData);
        j.update(j2);
        perData->data = j.dump();
        return true;
    }
};

//====================================================================================================
// Set float values 
//====================================================================================================
struct SetStateStringData : csnd::InPlug<2>
{
    int init()
    {
        if(writeJsonDataToGlobalVar(I_RATE))
            return OK;
        else
            return NOTOK;
    }
    
    int kperf()
    {
        if(writeJsonDataToGlobalVar(K_RATE))
            return OK;
        else
            return NOTOK;
    }
    
    bool writeJsonDataToGlobalVar(int mode)
    {
        if (in_count() != 2)
        {
            if(mode == K_RATE)
                csound->perf_error("Not enough input arguments\n", this);
            else
                csound->init_error("Not enough input arguments\n)");
            return false;
        }
        
        std::string jsonKeyName(args.str_data(0).data);
        std::string jsonData;
        std::string value(args.str_data(1).data);

        json j;

        CabbagePersistentData** pd = (CabbagePersistentData**)csound->query_global_variable("cabbageData");
        CabbagePersistentData* perData;
        if (pd != nullptr)
        {
            perData = *pd;
            jsonData = perData->data;
        }
        else
        {
            csound->create_global_variable("cabbageData", sizeof(CabbagePersistentData*));
            CabbagePersistentData** pd = (CabbagePersistentData**)csound->query_global_variable("cabbageData");
            *pd = new CabbagePersistentData();
            perData = *pd;
            csound->message("Creating new internal state object...\n");
            jsonData = "{}";
        }

        std::string newData = "{ \"" + jsonKeyName + "\" : \"" + value + "\"}";

        if (json::accept(newData) == false)
        {
            if(mode == K_RATE)
                csound->perf_error("Invalid JSON data:" + newData + "\n", this);
            else
                csound->init_error("Invalid JSON data:" + newData + "\n");

            return false;
        }

        j = json::parse(jsonData.empty() ? "{}" : jsonData);
        auto j2 = json::parse(newData);
        j.update(j2);

        perData->data = j.dump();
        return true;
    }
};

struct SetStateStringArrayData : csnd::InPlug<2>
{
    int init()
    {
        if(writeJsonDataToGlobalVar(I_RATE))
            return OK;
        else
            return NOTOK;
    }
    
    int kperf()
    {
        if(writeJsonDataToGlobalVar(K_RATE))
            return OK;
        else
            return NOTOK;
    }
    
    bool writeJsonDataToGlobalVar(int mode)
    {
        if (in_count() != 2)
        {
            if(mode == K_RATE)
                csound->perf_error("Not enough input arguments\n", this);
            else
                csound->init_error("Not enough input arguments\n");
            return false;
        }
                                   
        std::string jsonKeyName(args.str_data(0).data);
        std::string jsonData;
        csnd::Vector<STRINGDAT>& strs = args.vector_data<STRINGDAT>(1);

        json j;

        CabbagePersistentData** pd = (CabbagePersistentData**)csound->query_global_variable("cabbageData");
        CabbagePersistentData* perData;
        if (pd != nullptr)
        {
            perData = *pd;
            jsonData = perData->data;
        }
        else
        {
            csound->create_global_variable("cabbageData", sizeof(CabbagePersistentData*));
            CabbagePersistentData** pd = (CabbagePersistentData**)csound->query_global_variable("cabbageData");
            *pd = new CabbagePersistentData();
            perData = *pd;
            csound->message("Creating new internal state object...\n");
            jsonData = "{}";
        }

        std::string newData = "{ \"" + jsonKeyName + "\" : [";
        for (int i = 0; i < strs.len(); i++) {
            newData += "\""+(strs[i].data + std::string(i < strs.len() - 1 ? "\", " : ""));
        }
        newData += "\"]}";

        if (json::accept(newData) == false)
        {
            if(mode == K_RATE)
                csound->perf_error("Invalid JSON data:" + newData + "\n", this);
            else
                csound->init_error("Invalid JSON data:" + newData + "\n");

            
            return false;
        }

        j = json::parse(jsonData.empty() ? "{}" : jsonData);
        auto j2 = json::parse(newData);
        j.update(j2);

        perData->data = j.dump();
        return true;
    }
};
//====================================================================================================
// Get string values..
//====================================================================================================
struct GetStateStringValue : csnd::Plugin<1, 1>
{
    int init()
    {
        readData(I_RATE);
        return OK;
    }

    int kperf()
    {
        readData(K_RATE);
        return OK;
    }

    void readData(int mode)
    {
        std::string chString;
        json j;
        bool firstTimeSuccess = false;
        std::string channelKey(inargs.str_data(0).data);
        std::string jsonData;

        if(channelKey.empty()){
            return;
        }
        
        
        CabbagePersistentData** pd = (CabbagePersistentData**)csound->query_global_variable("cabbageData");
        auto perData = *pd;
        if (perData != nullptr)
        {
            jsonData = perData->data;
        }
        else
        {
            csound->message("Internal JSON global var is not valid.\n");
            return;
        }

        if (json::accept(jsonData) == false)
        {
            outargs.str_data(0).size = 0;
            outargs.str_data(0).data = "";
            return;
        }

        j = json::parse(jsonData);

        for (json::iterator it = j.begin(); it != j.end(); ++it)
        {
            if (it.key() == channelKey && it.value().is_string())
            {
                outargs.str_data(0).size = strlen((char*)it.value().dump().c_str());
                outargs.str_data(0).data = csound->strdup((char*)it.value().dump().c_str());
                firstTimeSuccess = true;
            }
        }

        if (firstTimeSuccess == false)
        {
            //csound->message("Could not find value for " + channelKey + "?\nCheck JSON channel data.\n");
            outargs.str_data(0).size = 0;
            outargs.str_data(0).data = csound->strdup("");
            firstTimeSuccess = true;
        }
    }
};

struct GetStateStringValueArray : csnd::Plugin<1, 1>
{
    int init()
    {
        readData(I_RATE);
        return OK;
    }

    int kperf()
    {
        readData(K_RATE);
        return OK;
    }

    void readData(int mode)
    {
        std::string chString;
        json j;
        MYFLT* value;
        bool firstTimeSuccess = false;
        std::string channelKey(inargs.str_data(0).data);
        std::string jsonData;

        if(channelKey.empty())
            if(mode == K_RATE)
                csound->perf_error("Key is empty\n", this);
            else
                csound->init_error("Key is empty\n");
        
        csnd::Vector<STRINGDAT>& out = outargs.vector_data<STRINGDAT>(0);


        CabbagePersistentData** pd = (CabbagePersistentData**)csound->query_global_variable("cabbageData");
        auto perData = *pd;
        if (perData != nullptr)
        {
            jsonData = perData->data;
        }
        else
        {
            csound->message("Internal JSON global var is not valid.\n");
            return;
        }


        if (json::accept(jsonData) == false)
        {
            csound->message("Invalid JSON data:" + jsonData + "\n");
            out.init(csound, 1);
            out[0].data = "";
            return;
        }

        j = json::parse(jsonData);

        for (json::iterator it = j.begin(); it != j.end(); ++it)
        {
            if (it.key() == channelKey && it.value().is_array())
            {
                out.init(csound, (int)it.value().size());
                int index = 0;
                for (json::iterator tt = it.value().begin(); tt != it.value().end(); ++tt)
                {
                    if (tt.value().is_string())
                        out[index++].data = csound->strdup((char*)tt.value().dump().c_str());
                }
                firstTimeSuccess = true;
            }
        }

        if (firstTimeSuccess == false)
        {
            //csound->message("Could not find value for " + channelKey + ". Check JSON channel data.\n");
            firstTimeSuccess = true;
        }
    }
};

//====================================================================================================
// Get numberic values..
//====================================================================================================
struct GetStateFloatValue : csnd::Plugin<1, 1>
{
    int init()
    {
        readData(I_RATE);
        return OK;
    }

    int kperf()
    {
        readData(K_RATE);
        return OK;
    }

    void readData(int mode)
    {
        std::string chString;
        json j;
        MYFLT* value;
        bool firstTimeSuccess = false;
        std::string channelKey(inargs.str_data(0).data);
        
        if(channelKey.empty())
            if(mode == I_RATE)
                csound->init_error("Key is empty\n");
            else
                csound->perf_error("Key is empty\n", this);
        
        std::string jsonData;

        CabbagePersistentData** pd = (CabbagePersistentData**)csound->query_global_variable("cabbageData");
        auto perData = *pd;
        if (perData != nullptr)
        {
            jsonData = perData->data;
        }
        else
        {
            csound->message("Internal JSON global var is not valid.\n");
            return;
        }

        
        if (json::accept(jsonData) == false)
        {
            csound->message("Invalid JSON data:" + jsonData + "\n");
            outargs[0] = -1;
            return;
        }

        j = json::parse(jsonData);

        for (json::iterator it = j.begin(); it != j.end(); ++it)
        {
            if (it.key() == channelKey && it.value().is_number_float())
            {
                outargs[0] = it.value();
                firstTimeSuccess = true;
            }
        }

        if (firstTimeSuccess == false)
        {
            //csound->message("Could not find value for "+ channelKey + "?\nCheck JSON channel data.\n");
            firstTimeSuccess = true;
        }
    }
};

struct GetStateFloatValueArray : csnd::Plugin<1, 1>
{
    int init()
    {
        readData(I_RATE);
        return OK;
    }

    int kperf()
    {
        readData(K_RATE);
        return OK;
    }

    void readData(int mode)
    {
        std::string chString;
        json j;
        bool firstTimeSuccess = false;
        std::string channelKey(inargs.str_data(0).data);
        std::string jsonData;

        if(channelKey.empty()){
            if(mode == K_RATE){
                csound->perf_error("Key is empty\n", this);
            }
            else{
                csound->init_error("Key is empty\n");
            }
        }
        
        csnd::Vector<MYFLT>& out = outargs.myfltvec_data(0);


        CabbagePersistentData** pd = (CabbagePersistentData**)csound->query_global_variable("cabbageData");
        auto perData = *pd;
        if (perData != nullptr)
        {
            jsonData = perData->data;
        }
        else
        {
            csound->message("Internal JSON global var is not valid.\n");
            return;
        }


        if (json::accept(jsonData) == false)
        {
            csound->message("Invalid JSON data:" + jsonData + "\n");
            out.init(csound, 1);
            out[0] = -1;
            return;
        }

        j = json::parse(jsonData);

        for (json::iterator it = j.begin(); it != j.end(); ++it)
        {
            if (it.key() == channelKey && it.value().is_array())
            {
                out.init(csound, (int)it.value().size());
                int index = 0;
                for (json::iterator tt = it.value().begin(); tt != it.value().end(); ++tt)
                {
                    out[index++] = tt.value();
                }
                firstTimeSuccess = true;
            }
        }

        if (firstTimeSuccess == false)
        {
            //csound->message("Could not find value for " + channelKey + "?\nCheck JSON channel data.\n");
            firstTimeSuccess = true;
        }
    }
};


//===========================================================================
// Channel State Save/Recall
//===========================================================================

struct ChannelStateSave : csnd::Plugin<1, 1>
{
    int init()
    {
        return writeDataToDisk(I_RATE);
    }

    int kperf()
    {
        return writeDataToDisk(K_RATE);
    }

    int writeDataToDisk(int mode)
    {
        
        String filename(inargs.str_data(0).data);
        
        if(filename.isEmpty()){
            //if(mode == I_RATE)
                csound->message("channelSaveState - Filename is empty\n");
            //else
            //    csound->init_error("Filename is empty\n");
            return NOTOK;
        }
        
        json j;

        controlChannelInfo_s* csoundChanList;
        int numberOfChannels = csound->get_csound()->ListChannels(csound->get_csound(), &csoundChanList);

        for (int i = 0; i < numberOfChannels; i++)
        {
            //            const float min = csoundChanList[i].hints.min;
            //            const float max = (csoundChanList[i].hints.max == 0 ? 1 : csoundChanList[i].hints.max);
            //            const float defaultValue = csoundChanList[i].hints.dflt;
            std::string name;

            MYFLT* value;
            char* chString;

            if (csound->get_csound()->GetChannelPtr(csound->get_csound(), &value, csoundChanList[i].name,
                CSOUND_CONTROL_CHANNEL | CSOUND_OUTPUT_CHANNEL) == CSOUND_SUCCESS)
            {
                j[csoundChanList[i].name] = *value;
            }

            if (csound->get_csound()->GetChannelPtr(csound->get_csound(), &value, csoundChanList[i].name,
                CSOUND_STRING_CHANNEL | CSOUND_OUTPUT_CHANNEL) == CSOUND_SUCCESS)
            {
                chString = ((STRINGDAT*)value)->data;
                String s(chString);
                j[csoundChanList[i].name] = s.replace("\\\\", "/").toStdString();
            }
        }


        
        std::ofstream file;
        file.open(filename.replace("\\\\", "/").toStdString());
        if (file.is_open() == false)
            outargs[0] = 0;
        else
            outargs[0] = 1;

        file << std::setw(4) << j << std::endl;
        file.close();
        //csound->message(j.dump());
        
        return OK;
    }

};


struct ChannelStateRecall : csnd::Plugin<1, 2>
{
    int init()
    {
        readDataFromDisk(I_RATE);
        return OK;
    }

    int kperf()
    {
        readDataFromDisk(K_RATE);
        return OK;
    }

    void readDataFromDisk(int mode)
    {
        json j;
        std::string filename(inargs.str_data(0).data);
        std::vector<std::string> ignoreStrings;

        if (in_count() == 2)
        {
            csnd::Vector<STRINGDAT>& in = inargs.vector_data<STRINGDAT>(1);
            for (int i = 0; i < in.len(); i++)
            {
                ignoreStrings.push_back(std::string(in[i].data));
                DBG(in[i].data);
            }
        }

        std::ifstream file(filename);
        
        if (file.fail())
        {
            if(mode == K_RATE)
                csound->perf_error("Unable to open file\n", this);
            else
                csound->init_error("Unable to open file\n");
            outargs[0] = 0;
            return;
        }

        j << file;
        if (json::accept(j.dump()) == false) 
        {
            if(mode == K_RATE)
                csound->perf_error("Found invalid JSON data in "+filename+"\n", this);
            else
                csound->init_error("Found invalid JSON data in "+filename);
            return;
        }


        MYFLT* value;

        for (json::iterator it = j.begin(); it != j.end(); ++it)
        {
            bool ignore = false;
            std::string channelName = it.key();

            for (int i = 0; i < ignoreStrings.size(); i++)
            {
                if (channelName == ignoreStrings[i])
                    ignore = true;
            }

            if (ignore == false)
            {
                if (it.value().is_number_float())
                {
                    if (csound->get_csound()->GetChannelPtr(csound->get_csound(), &value, channelName.c_str(),
                        CSOUND_CONTROL_CHANNEL | CSOUND_OUTPUT_CHANNEL) == CSOUND_SUCCESS)
                    {
                        DBG(channelName);
                        *value = it.value();
                    }
                }
                else if (it.value().is_string())
                {
                    if (csound->get_csound()->GetChannelPtr(csound->get_csound(), &value, channelName.c_str(),
                        CSOUND_STRING_CHANNEL | CSOUND_OUTPUT_CHANNEL) == CSOUND_SUCCESS)
                    {
                        DBG(channelName);
                        std::string string = it.value();
                        ((STRINGDAT*)value)->data = csound->strdup((char*)string.c_str());
                    }
                }
            }
        }
        outargs[0] = 1;
        file.close();
        //csound->message(j.dump());
    }

};

//===========================================================================
// FileToStr
//===========================================================================
struct FileToStr : csnd::Plugin<1, 1>
{
    int init()
    {
        std::string line;
        std::string lines;
        char* fileName = inargs.str_data(0).data;
        std::ifstream fileStream(fileName);

        if (fileStream.is_open())
        {
            while (getline(fileStream, line))
            {
                lines.append(line);
                lines.append("\n");
            }
        }
        else
        {
            csound->message("fileToStr could not open file for reading");
            return NOTOK;
        }

        fileStream.close();
        outargs.str_data(0).size = strlen((char*)lines.c_str());
        outargs.str_data(0).data = csound->strdup((char*)lines.c_str());
        return OK;
    }
};

//===========================================================================
//removes a number of occurrences of one string from another
//===========================================================================
struct StrRemove : csnd::Plugin<1, 3>
{
    int init()
    {
        return parseStringAndFillStruct(this);
    }

    int parseStringAndFillStruct(Plugin* opcodeData)
    {
        int occurrences = -1;
        int strsRemoved = 0;
        char* inString = opcodeData->inargs.str_data(0).data;
        char* inStripString = opcodeData->inargs.str_data(1).data;

        if (opcodeData->in_count() > 2)
            occurrences = opcodeData->inargs[2];

        std::string input(inString);
        std::string stripString(inStripString);
        std::string::size_type index = input.find(stripString);

        while (index != std::string::npos)
        {
            input.erase(index, stripString.length());
            index = input.find(stripString, index);
            strsRemoved += 1;

            if (strsRemoved == occurrences)
                break;
        }

        opcodeData->outargs.str_data(0).size = strlen((char*)input.c_str());
        opcodeData->outargs.str_data(0).data = csound->strdup((char*)input.c_str());

        return OK;
    }
};

//===========================================================================
// StrToArray
//===========================================================================
struct StrToArray : csnd::Plugin<1, 2>
{
    int init()
    {
        return parseStringAndFillStruct();
    }

    int parseStringAndFillStruct()
    {
        csnd::Vector<STRINGDAT>& out = outargs.vector_data<STRINGDAT>(0);

        char* inString = inargs.str_data(0).data;
        char* inStringDeLimiter = inargs.str_data(1).data;


        std::string input(inString);
        std::string delimiter(inStringDeLimiter);
        std::vector<std::string> tokens;

        size_t pos = 0;

        while ((pos = input.find(delimiter)) != std::string::npos)
        {
            tokens.push_back(input.substr(0, pos));
            input.erase(0, pos + delimiter.length());
        }

        if (input.size() > 0)
            tokens.push_back(input);

        out.init(csound, (int)tokens.size());

        for (int i = 0; i < tokens.size(); i++)
        {
            out[i].data = csound->strdup((char*)tokens[i].c_str());
        }

        tokens.clear();
        return OK;
    }
};
//===========================================================================
// StrToFile
//===========================================================================

struct StrToFile : csnd::Plugin<1, 3>
{
    int init()
    {

        int mode = 0;
        if (in_count() < 2)
        {
            csound->message("You did not pass have enough arguments to strtofile\n");
            return NOTOK;
        }

        char* inString = inargs.str_data(0).data;
        char* fileName = inargs.str_data(1).data;


        if (in_count() > 2)
            mode = inargs[2];

        std::ofstream fileStream;

        if (mode == 1)
        {
            fileStream.open(fileName, std::ios::in | std::ios::app);
        }
        else
            fileStream.open(fileName, std::ios::in | std::ios::trunc);

        fileStream << inString;

        if (!fileStream.is_open())
        {
            csound->message("*** strToFile could not open file for writing ***");
            outargs[0] = 0;
        }

        fileStream.close();
        outargs[0] = 1;
        return OK;
    }
};
//void csnd::on_load (Csound* csound)
//{
//    csnd::plugin<channelStateSave> (csound, "channelStateSave.i", "i", "S", csnd::thread::i);
//    csnd::plugin<channelStateSave>(csound, "channelStateSave.k", "k", "S", csnd::thread::k);
//    csnd::plugin<channelStateRecall> (csound, "channelStateRecall.i", "i", "S", csnd::thread::i);
//    csnd::plugin<channelStateRecall> (csound, "channelStateRecall.k", "k", "SO", csnd::thread::k);
//    csnd::plugin<channelStateRecall>(csound, "channelStateRecall.k", "k", "SS[]", csnd::thread::k);
//}