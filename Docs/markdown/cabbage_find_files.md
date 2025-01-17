# cabbageFindFiles

This opcode will search a given directory for files, folder, or files and folders. If you wish to use this opcode at k-rate use the version with the kTriggerFind input parameter. 

<blockquote style="font-style:italic;border-left:10px solid #93d200;color:rgb(3, 147, 210);padding:1px;padding-left:10px;margin-top:0px;margin-bottom:1px;border-left-width:0.25rem"> Added in Cabbage v2.6.6</blockquote>

### Syntax

<pre>SFiles[] <b>cabbageFindFiles</b> SLocation [, SType, SExtension]</pre>
<pre>SFiles[] <b>cabbageFindFiles</b> kTriggerFind, SLocation [, SType, SExtension]</pre>

#### Initialization

* `SLocation` -- the directory you wish to search
* `kTriggerFind` -- will trigger a search each time this is 1. 
* `SType` (optional) -- what you want to search for, `"files"`, `"directories"`, `"fileAndDirectories"`
* `SExtension` (optional) -- a file extension in the form of `"*.wav"`. This can be a `;` delimited list, i.e, `"*.wav;*.ogg"`

#### Performance

* `SFiles[]` -- a string array of files found


### Example

```csharp
<Cabbage>
form caption("Find Files") size(400, 300), guiMode("queue") pluginId("def1")
filebutton bounds(8, 208, 384, 43) channel("browseButton"), corners(5), mode("directory") file("/Users/walshr/sourcecode/cabbage/Examples/Miscellaneous")
listbox bounds(8, 10, 383, 192) channel("fileBrowser"), colour(147, 210, 0), highlightColour("black"), channelType("string")
label bounds(35, 265, 335, 16) channel("label1"), text("Please select a file"), colour("white"), fontColour(3, 147, 210)
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-n -d -+rtmidi=NULL -M0 -m0d 
</CsOptions>
<CsInstruments>
; Initialize the global variables. 
ksmps = 32
nchnls = 2
0dbfs = 1


instr 1
    SDirectory, kTrigger cabbageGetValue "browseButton"
    SFiles[] cabbageFindFiles kTrigger, SDirectory, "files"
    cabbageSet kTrigger, "fileBrowser", sprintfk("populate(\"*\", \"%s\")", SDirectory)    
    SSelectedItem, kFileSelected cabbageGetValue "fileBrowser"
    SSelectedFile strcatk cabbageGetFilename(SSelectedItem), cabbageGetFileExtension(SSelectedItem)
    cabbageSet kFileSelected, "label1", "text", cabbageGetFilename(SSelectedItem)
endin

</CsInstruments>
<CsScore>
;causes Csound to run for about 7000 years...
f0 z
;starts instrument 1 and runs it for a week
i1 0 [60*60*24*7] 
</CsScore>
</CsoundSynthesizer>

```
