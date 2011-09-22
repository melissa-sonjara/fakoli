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
 
Fakoli::using("video", "settings");

class VideoManager
{
	function VideoManager()
	{
		trace("Creating VideoManager", 3);
	}
	
	static function setDefaults()
	{
		Settings::setDefaultValue("video", "video_folder", "videos", String, "Directory inside the web folder that is used to store the video files");
		Settings::setDefaultValue("video", "flowplayer_script", "", String, "URI for the Flowplayer script");
		Settings::setDefaultValue("video", "flowplayer_player", "", String, "URI for the Flowplayer SWF file");
		Settings::setDefaultValue("video", "ffmpeg_path", "", String, "Directory containing the ffmpeg binary executables");
	}
	
	static function upgradeComponent()
	{
		$mgr = new VideoUpgradeManager();
		$mgr->upgrade();
	}
	
	function hasFFMPEG()
	{
		return (extension_loaded("ffmpeg") && Settings::getValue("video", "ffmpeg_path"));
	}
	
	function open($file)
	{
		trace("Opening video '$file'", 3);
		$this->ffmpeg = new ffmpeg_movie($file);
	}
	
	function getFrameWidth()
	{
		$width = $this->makeMultipleTwo($this->ffmpeg->getFrameWidth());
		trace("Width: $width", 3);
		return $width;	
	}
	
	function getFrameHeight()
	{
		$height = $this->makeMultipleTwo($this->ffmpeg->getFrameHeight());
		trace("Height: $height", 3);
		return $height;	
	}
	
	function getFrameRate()
	{
		return $this->ffmpeg->getFrameRate();
	}
	
	function getNextKeyFrame()
	{
		return $this->ffmpeg->getNextKeyFrame();
	}	
	
	function makeMultipleTwo($value)
	{
		return intval($value) >> 1 << 1;
	}
}
?>