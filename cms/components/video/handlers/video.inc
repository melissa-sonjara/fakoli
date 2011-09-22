<?php
/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/
 
$video_id = checkNumeric($_GET["video_id"]);
$video = new Video($video_id);

$width = $video->width;
$height = $video->height;
$swf = "/".Settings::getValue("video", "video_folder")."/".$video->video_file;

?>
<div style="width: <?echo $width?>px; height: <?echo $height?>px; margin: 0; padding: 0;">
            <object id="csSWF" classid="clsid:d27cdb6e-ae6d-11cf-96b8-444553540000" width="<?echo $width?>" height="<?echo $height?>" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,115,0">
                <param name="src" value="http://<?echo $config['http_host']?><?echo $swf?>"/>
                <param name="bgcolor" value="#1a1a1a"/>
                <param name="quality" value="best"/>
                <param name="allowScriptAccess" value="always"/>
                <param name="allowFullScreen" value="true"/>
                <param name="scale" value="showall"/>
                <param name="flashVars" value="autostart=true"/>
                <embed name="csSWF" src="http://<?echo $config['http_host']?><?echo $swf?>" width="<?echo $width?>" height="<?echo $height?>" bgcolor="#1a1a1a" quality="best" allowScriptAccess="always" allowFullScreen="true" scale="showall" flashVars="autostart=true&thumb=FirstFrame.png&thumbscale=45&showendscreen=false&color=0x1A1A1A,0x1A1A1A" pluginspage="http://www.macromedia.com/shockwave/download/index.cgi?P1_Prod_Version=ShockwaveFlash"></embed>
            </object>
</div>           
